#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "strlib.h"

#include "interface.h"

#define TERM "qsh-0.6"
#define NON_ROOT "$"
#define ROOT "#"

char* get_dir()
{
	char *dir = getcwd(NULL,0);
	char *res = repl_home(dir);
	free(dir);
	return res;
}

char* get_last_dir()
{
		char *dir = get_dir();
		char *head = dir;
		if(!dir[1])
			return dir;
		while(*(dir++)){
		}
		while(*dir != '/' && (dir--) != head){
		}
		dir = str_unconst(dir+1);
		free(head);
		return dir;
}

void print_greet()
{
	const char *user = getenv("USER");
	char *dir = get_last_dir();
	printf("%s:%s %s%s ",TERM,dir,user,str_cmp(user,"root")?NON_ROOT:ROOT);
	free(dir);
}
