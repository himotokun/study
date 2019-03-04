#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "array.hpp"
#include "game.h"

GameClass::res_s::res_s()
{
	prod.qty = prod.price = raw.qty = raw.price = 0;
}

GameClass::GameClass()
{
	turn = 0;
}

void GameClass::send(const char *cmd)
{
	write(fd, cmd, strlen(cmd));
}

int GameClass::recv_char()
{
	char c;
	int n = read(fd, &c, 1);
	if(!n){
		printf("Connection closed\n");
		exit(0);
	}
	return c;
}

void GameClass::skip_line()
{
	while(recv_char() != '\n'){
	}
}

void GameClass::recv_line(char *buf, int max_size)
{
	int ptr = 0;
	char c;
	while( (c = recv_char()) != '\n' && ptr < max_size-1 )
		buf[ptr++] = c;
	buf[ptr] = 0;
}

void GameClass::just_recv()
{
	char buf[std_buf_size];
	read(fd, buf, std_buf_size);
}

void GameClass::recv_to_amp(char *buf, int size)
{
	char _buf[std_buf_size];
	if(!buf)
		buf = _buf;
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	if(!strncmp(buf+2, "BANKRUPT", 8)){
		printf("I have lost, sorry :(\n");
		kill(getpid(), SIGKILL);
	}
}

void GameClass::recv_to_amp_ch()
{
	while(recv_char() != '&');
}

void GameClass::EndTurn()
{
	send("turn\n");
	GetResults();
	prev_info = info;
	GetInfo();
	GetMarket();
	if(ActivePlayers() == 1){
		printf("I have won, hurray! :)\n");
		kill(getpid(), SIGKILL);
	}
	turn++;
}

void GameClass::Build(int qty)
{
	while(qty--){
		send("build\n");
		recv_to_amp();
	}
}

void GameClass::Prod(int qty)
{
	char buf[100];
	sprintf(buf, "prod %d\n", qty);
	send(buf);
	recv_to_amp();
}

void GameClass::Buy(int qty, int price)
{
	char buf[100];
	sprintf(buf, "buy %d %d\n", qty, price);
	send(buf);
	recv_to_amp();
}

void GameClass::Sell(int qty, int price)
{
	char buf[100];
	sprintf(buf, "sell %d %d\n", qty, price);
	send(buf);
	recv_to_amp();
}


int GameClass::MyId()
{
	return GetId(my_nick);
}

int GameClass::GetId(const char *nick)
{
	for(int i = 0; i < players.Size(); i++)
		if(!strcmp(nick, players[i]))
			return i;
	return -1;
}

int GameClass::Turn()
{
	return turn;
}

int GameClass::Players()
{
	return players_num;
}

int GameClass::ActivePlayers()
{
	return active_players_num;
}

int GameClass::Supply()
{
	return market.raw.qty;
}

int GameClass::RawPrice()
{
	return market.raw.price;
}

int GameClass::Demand()
{
	return market.prod.qty;
}

int GameClass::ProductionPrice()
{
	return market.prod.price;
}

int GameClass::Money(int player_id)
{
	return info[player_id].money;
}

int GameClass::Raw(int player_id)
{
	return info[player_id].raw;
}

int GameClass::Production(int player_id)
{
	return info[player_id].prod;
}

int GameClass::Factories(int player_id)
{
	return info[player_id].plants;
}

int GameClass::AutoFactories(int player_id)
{
	return info[player_id].auto_plants;
}

int GameClass::Manufactured(int player_id)
{
	return info[player_id].prod - prev_info[player_id].prod +
		ResultProdBought(player_id);
}

int GameClass::ResultRawSold(int player_id)
{
	return results[player_id].raw.qty;
}

int GameClass::ResultRawPrice(int player_id)
{
	return results[player_id].raw.price;
}

int GameClass::ResultProdPrice(int player_id)
{
	return results[player_id].prod.price;
}

int GameClass::ResultProdBought(int player_id)
{
	return results[player_id].prod.qty;
}

void GameClass::Open(const char *ip, int port)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1){
		printf("Can't open socket\n");
		exit(1);
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if( !inet_aton(ip, &(addr.sin_addr)) ){
		printf("Wrong ip address\n");
		exit(1);
	}
	if( connect(fd, (sockaddr*)&addr, sizeof(addr)) )
	{
		printf("Can't connect to the game\n");
	}
}

void GameClass::Start(const char *nick)
{
	my_nick = strdup(nick);
	send(nick);
	send("\n");
	just_recv();
	send(".join 1\n");
	char buf[std_buf_size];
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	GetInfo();
	GetMarket();
}

void GameClass::GetInfo()
{
	send("info\n");
	char buf[std_buf_size];
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	while(buf[0] == '&'){
		char name[std_buf_size];
		memset(name, 0, sizeof(name));
		info_s _;
		sscanf(buf, "& INFO %s %d %d %d %d %d", name, &(_.raw), &(_.prod),
				&(_.money), &(_.plants), &(_.auto_plants));
		players.PushBack(strdup(name));
		info.PushBack(_);
		recv_line(buf);
	}
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	if(sscanf(buf, "& PLAYERS %d WATCHERS %d", &active_players_num, &players_num) != 2)
		throw 5;
	players_num += active_players_num;
}

void GameClass::GetMarket()
{
	char buf[std_buf_size];
	send("market\n");
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	sscanf(buf, "& MARKET %d %d %d %d", &(market.raw.qty), &(market.raw.price),
			&(market.prod.qty), &(market.prod.price));
}

void GameClass::GetResults()
{
	char buf[std_buf_size];
	do{
		recv_line(buf);
	} while(buf[0] != '&');
	while(buf[0] == '&'){
		results.Resize(active_players_num);
		for(int i = 0; i < results.Size(); i++)
			results[i] = res_s();
		char name[std_buf_size];
		int q, p;
		memset(name, 0, sizeof(name));
		switch(buf[2]){
			case 'B':
				if(!strncmp(buf+2, "BANKRUPT", 8))
					break;
				sscanf(buf, "& BOUGHT %s %d %d", name, &q, &p);
				results[GetId(name)].raw.qty = q;
				results[GetId(name)].raw.price = p;
				break;
			case 'S':
				sscanf(buf, "& SOLD %s %d %d", name, &q, &p);
				results[GetId(name)].prod.qty = q;
				results[GetId(name)].prod.price = p;
				break;
		}
		recv_line(buf);
	}
	recv_to_amp_ch();
	just_recv();
}

void GameClass::Close()
{
	close(fd);
}
