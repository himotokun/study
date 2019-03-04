#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "strlib.h"
#include "interface.h"
#include "defs.h"

#include "cmdlib.h"

static struct t_lst{
	int pid;
	char bg_flag;
	struct t_lst *next;
} *pids = NULL;

static int lock = 0;
static int last_pid = 0;
static int ret_st = 0;

#if 0
static void print_tlst()
{
	struct t_lst *lst = pids;
	if(!lst)
		printf("[]\n");
	while(lst){
		printf("[%d,%d], ",lst->pid,lst->bg_flag);
		lst = lst->next;
	}
	printf("\n");
}
#endif

static void add_pid(int pid, char bg_flag)
{
	struct t_lst *tmp = malloc(sizeof(*tmp));
	tmp->pid = pid;
	tmp->bg_flag = bg_flag;
	tmp->next = NULL;
	if(pids){
		struct t_lst *tmp2 = pids;
		while(tmp2->next)
			tmp2 = tmp2->next;
		tmp2->next = tmp;
	}else{
		pids = tmp;
	}
	if(!bg_flag)
		lock++;
	last_pid = pid;
}

static void print_st(int c_pid,int status)
{
	if(WIFEXITED(status)){
		printf("[%d exited with status %d]\n",c_pid,
			WEXITSTATUS(status));
	}else if(WIFSIGNALED(status)){
		printf("[%d terminated by signal %d]\n",c_pid,
			WTERMSIG(status));
	}else{
		printf("[%d stopped with strange reason]\n",c_pid);
	}
}

static void rm_pid(int pid,int status)
{
	struct t_lst *pre = pids, *tmp = pids;
	while(tmp && tmp->pid != pid){
		pre = tmp;
		tmp = tmp->next;
	
	}
	if(!tmp)
		return;
	if(tmp->bg_flag)
		print_st(tmp->pid,status);
	if(pre == tmp){
		struct t_lst *tmp2 = pre->next;
		if(!tmp->bg_flag)
			lock--;
		free(pre);
		pids = tmp2;
		return;
	}
	if(tmp){
		if(!tmp->bg_flag)
			lock--;
		pre->next = tmp->next;
		free(tmp);
	}
}

static void rm_chld(int signo)
{
	int pid,status;
	while((pid = waitpid(-1,&status,WNOHANG))>0){
		rm_pid(pid,status);
		if(pid == last_pid && WIFEXITED(status))
			if(!WEXITSTATUS(status))
				ret_st = 0;
	}
}

static void cd(char *const *argv){
	if(!argv[1]){
		char *home = getenv("HOME");
		if(!home){
			perror(home);
		}else if(chdir(home)){
			perror(home);
		}
	}else if(!argv[2]){
		char *tmp;
		if(chdir(tmp = repl_home(argv[1]))){
			fprintf(stderr,"qsh: cd: ");
			perror(tmp);
		}
		free(tmp);
	}else{
		printf("Error: too many arguments for \"cd\"\n");
	}
}

static void killall(int signo)
{
}

static void close2(int a[2])
{
	if(a[0]!=0)
		close(a[0]);
	if(a[1]!=1)
		close(a[1]);
}

static int exec_wfd(char **argv, int f_pre[2], int f_cur[2])
{	
	int pid = fork();
	if(pid == -1){
		printf("[%s: cannot fork]\n",argv[0]);
		exit(2);
	}
	if(!pid){ /* CHILD */
		dup2(f_pre[0],0);
		dup2(f_cur[1],1);
		close2(f_pre);
		close2(f_cur);
		execvp(argv[0],argv);
		perror(argv[0]);
		exit(1);
	}
	return pid;
}

static int open_fd(int fd[2], char **io_path, char app_flg)
{
	if(io_path[0]){
		fd[0] = open(io_path[0], O_RDONLY);
		if(fd[0] == -1){
			fprintf(stderr,"qsh: ");
			perror(io_path[0]);
			return -1;
		}
	}else{
		fd[0] = 0;
	}
	if(io_path[1]){
		if(app_flg)
			fd[1] = open(io_path[1], O_WRONLY|O_CREAT|O_APPEND, 0666);
		else
			fd[1] = open(io_path[1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if(fd[1] == -1){
			fprintf(stderr,"qsh: ");
			perror(io_path[0]);
			return -1;
		}
	}else{
		fd[1] = 1;
	}
	return 0;
}

static struct c_lst* exec_clst1(struct c_lst *lst, char* flags)
{
	struct c_lst *end = lst;
	int pid, p_cur[2] = {0,1}, p_pre[2] = {0,1};
	char bg_flag=0;
	while( (!((end->flags)&C_BG)) && (((end->flags)&C_NOTS)==C_CONV) && end)
		end = end->next;
	if(end){
		if(end->flags&C_BG)
			bg_flag = 1;
		*flags = end->flags;
		end = end->next;
	}else{
		*flags = 0;
	}
	while(lst != end){
		p_pre[0] = p_cur[0], p_pre[1] = p_cur[1];
		if((lst->flags&C_CONV) == C_CONV)
			pipe(p_cur);
		else
			p_cur[0] = 0, p_cur[1] = 1;
		if(lst->flags&C_IO)
			if(open_fd(p_cur, lst->io_path, lst->flags&C_APP))
				return NULL;
		pid = exec_wfd(lst->argv, p_pre, p_cur);
		add_pid(pid,bg_flag);
		if(p_cur[1]!=1)
			close(p_cur[1]);
		if(p_pre[0]!=0)
			close(p_pre[0]);
		if(!( (lst->flags&C_BG) || ((lst->flags&C_CONV)==C_CONV)  )){
			while(lock){
				sleep(1);
			}
		}else if(bg_flag){
			printf("[%s: run with pid %d]\n",lst->argv[0],pid);
		}
		lst = lst->next;
	}
	return end;
}

static void print_err(char errno)
{
	fprintf(stderr,"qsh: syntax error: ");
	switch(errno){
		case CERR_NOIO:
			fprintf(stderr,"error in i/o redirect path\n");
			break;
		case CERR_SEVCTRL:
			fprintf(stderr,"command expected but control word got\n");
			break;
		case CERR_ENDCTRL:
			fprintf(stderr,"command expected after control word\n");
			break;
		case CERR_SEVIO:
			fprintf(stderr,"i/o redirect path must contain a word\n");
			break;
		case CERR_CONVIO:
			fprintf(stderr,"i/o redirect with pipeline is not allowed\n");
			break;
		default:
			fprintf(stderr,"unknown error\n");
	}
}

/* ----- Interface ----- */

void exec_clst2(struct c_lst *lst)
{
	char end_type = 0;
	if(lst->flags&C_ERR){
		print_err(lst->flags);
		return;
	}
	if(!lst->argv)
		return;
	if(!str_cmp(lst->argv[0],"exit"))
		exit(0);
	if(!str_cmp(lst->argv[0],"cd")){
		cd(lst->argv);
		return;
	}

	signal(SIGCHLD,&rm_chld);
	signal(SIGINT,&killall);
	while((lst = exec_clst1(lst,&end_type))){
		if( (ret_st&&((end_type&C_NOTS)==C_AND))
				|| ((!(ret_st))&&((end_type&C_NOTS)==C_OR)) ){
			break;
		}
	}
}
