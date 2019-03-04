#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "strlib.h"
#include "game.h"

#include "server.h"

/* Server implementation */

enum{ QLEN_MAX = 5 };
enum{ buffer_size = 128};

static int send2(int fd, const char *str)
{
	return write(fd, str, str_len(str)+1);
}

static struct pl_arr init_pl(int max_size)
{
	struct pl_arr res;
	int i;
	res.max_size = max_size;
	res.size = 0;
	res.d = malloc(max_size*sizeof(struct pl_data));
	for(i = 0; i < max_size; i++){
		res.d[i].name = NULL;
		res.d[i].buf_ptr = 0;
		res.d[i].buf = malloc(buffer_size);
	}
	return res;
}

typedef struct sockaddr_in addr_s;

static addr_s* get_addrs(int port)
{
	addr_s *res = malloc(sizeof(*res));
	res->sin_family = AF_INET;
	res->sin_port = htons(port);
	res->sin_addr.s_addr = INADDR_ANY;
	return res;
}

static int select2(int n, fd_set *fds)
{
	return select(n, fds, NULL, NULL, NULL);
}

static int make_fds(struct pl_arr players, int listener, fd_set *clnt_set)
{
	int sd_max = listener;
	FD_ZERO(clnt_set);
	FD_SET(listener, clnt_set);
	FD_SET(0, clnt_set);
	while(players.size--){
		FD_SET(players.d[players.size].fd, clnt_set);
		if(players.d[players.size].fd > sd_max)
			sd_max = players.d[players.size].fd;
	}
	return sd_max;
}

/* User operations */

static void broadcast(const struct pl_arr players, const char *msg)
{
	int i;
	for(i = 0; i < players.size; i++)
		write(players.d[i].fd, msg, str_len(msg)+1);
}

static void login(struct pl_arr *players, int new_fd)
{
	char buffer[buffer_size], buffer2[buffer_size], got_bts;
	send2(new_fd,"Choose a nickname (up to 10 characters): ");
	got_bts = read(new_fd, buffer, buffer_size-2);
	buffer[got_bts>12?10:got_bts-2] = 0;
	players->d[players->size].fd = new_fd;
	players->d[(players->size)++].name = str_unconst(buffer);
	send2(new_fd,"Welcome to the Jew Game!\n");
	printf("[%s joined the game]\n", buffer);
	sprintf(buffer2,"*** %s joined the game ***\n",buffer);
	broadcast(*players, buffer2);
	sprintf(buffer,"*** Total connected: %d of %d players ***\n",
			players->size, players->max_size);
	send2(new_fd,buffer);
}

static void rm_player(struct pl_arr *players, char *name)
{
	int i, id;
	for(id = 0; id < players->size; id++){
		if(!str_cmp(players->d[id].name, name)){
			for(i = id; i < players->size - 1; i++)
				players->d[i] = players->d[i+1];
			players->size--;
			return;
		}
	}
}

static void disconnect(struct pl_arr *players, char *name, int nolos)
{
	int id;
	for(id = 0; id < players->size; id++){
		if(!str_cmp(players->d[id].name, name)){
			char buffer[buffer_size];
			if(!players->max_size && !nolos)
				send2(players->d[id].fd,"*** You have spent all your money, "
					"goodbye, loser! ***\n");
			shutdown(players->d[id].fd,2);
			close(players->d[id].fd);
			printf("[%s left the game]\n", players->d[id].name);
			sprintf(buffer, "\n*** %s left the game ***\n> ", players->d[id].name);
			rm_player(players, players->d[id].name);
			if(!(players->size) && !(players->max_size)){
				printf("*** Game ended ***\n");
				printf("*** Server stopped ***\n");
				exit(0);
			}
			broadcast(*players, buffer);
			if(!players->max_size){
				bye(name);
			}
			return;
		}
	}
}

static void reject(int clnt_new)
{
	const char error[] = "Sorry, the game has already started\n";
	write(clnt_new, error, sizeof(error));
	shutdown(clnt_new, 2);
	close(clnt_new);
}

static int connect_usr(struct pl_arr *players, int listener,
		addr_s *addrs, fd_set *clnt_set)
{
	if(FD_ISSET(listener, clnt_set)){
		int addrlen = sizeof(*addrs);
		int clnt_new = accept(listener, addrs, &addrlen);
		if(clnt_new == -1){
			printf("*** Conection accepting error ***\n");
			return SERVERR_ACPT;
		}
		if(!(players->max_size)){
			reject(clnt_new);
		}else{
			login(players, clnt_new);
		}
		if(players->size == players->max_size){
			game_start(players->max_size, players->d);
			broadcast(*players,"*** Game started ***\n> ");
			printf("*** Game started ***\n");
			players->max_size = 0;
		}
	}
	return 0;
}

static int get_turns(struct pl_arr *players, int listener,
		addr_s *addrs, fd_set *clnt_set)
{
	int i;
	for(i = 0; i < players->size; i++){
		if(FD_ISSET(players->d[i].fd, clnt_set)){
			players->d[i].buf_ptr += read(players->d[i].fd,
					players->d[i].buf, buffer_size-2-players->d[i].buf_ptr);
			if(players->d[i].buf_ptr >= buffer_size-10){
				send2(players->d[i].fd,
						"*** Don't even try to DoS us, hahaha! ***\n");
				disconnect(players, players->d[i].name, 1);
			}
			if(!players->d[i].buf_ptr){
				disconnect(players, players->d[i].name, 0);
				if(!(players->size) && !(players->max_size)){
					printf("*** Game ended ***\n");
					printf("*** Server stopped ***\n");
					exit(0);
				}
				continue;
			}
			if(players->d[i].buf[players->d[i].buf_ptr-1] != '\n')
				continue;
			players->d[i].buf[players->d[i].buf_ptr-2] = 0;
			/*if(!str_cmp(players->names[i],"admin")){
				char *res = admin(players->buf[i]);
				send2(players->fd[i], res);
				free(res);
				players->buf_ptr[i] = 0;
			}
			else */
			if(players->max_size){
				const char error[] = "Please wait for other players\n";
				write(players->d[i].fd, error, sizeof(error));
			}else if(players->d[i].buf[0]){
				char **res = NULL;
				int pl;
				switch( (pl = play(i, players->d[i].buf, &res)) ){
						case 0:
							break;
						case PLAY_DLG:
						case PLAY_DLG_NOGR:
							while(*res){
								send2(players->d[i].fd, *(res));
								printf("%s",*(res++));
							}
							break;
						case PLAY_BRC:
							printf("*** Broadcasting message ***");
							while(*res){
								broadcast(*players, *(res));
								printf("%s",*(res++));
							}
				}
				players->d[i].buf_ptr = 0;
				if(pl != PLAY_DLG_NOGR && str_cmp("end",players->d[i].buf))
						send2(players->d[i].fd, "> ");
			}
		}
	}
	return 0;
}

static int process_queue(struct pl_arr *players, int listener,
		addr_s *addrs, fd_set *clnt_set)
{
	if( connect_usr(players, listener, addrs, clnt_set) )
		return SERVERR_ACPT;

	if( get_turns(players, listener, addrs, clnt_set) )
		return -2;

	return 0;
}

/* Interfaces */

int lstn_init(int port)
{
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	addr_s *addrs = get_addrs(port);
	if( listener == -1 )
		return SOCKERR_OPEN;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if( bind(listener, addrs, sizeof(*addrs)) )
		return SOCKERR_BIND;
	if( listen(listener, QLEN_MAX) == -1)
		return SOCKERR_LSTN;
	free(addrs);
	printf("*** Listener opened on port %d ***\n",port);
	return listener;
}

int serv_main(int listener, int max_players, int port)
{
	fd_set clnt_set;
	int sd_max = listener;
	struct pl_arr players = init_pl(max_players);
	addr_s *addrs = get_addrs(port);
	printf("*** Server started ***\n");
	for(;;){
		sd_max = make_fds(players, listener, &clnt_set);
		if( select2(sd_max+1, &clnt_set) == -1)
			return SERVERR_SLCT;
		if(process_queue(&players, listener, addrs, &clnt_set))
			return 1;
	}

	return 0;
}
