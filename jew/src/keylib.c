#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "strlib.h"

#include "keylib.h"

static void print_help(int name_flg)
{
	if(name_flg)
		printf("SANIO Jew Game, version 0.1\n");
	printf(
"Usage: jew [options]\n"
"Options:\n"
"	-h | --help \n"
"	--max-players <number>\n"
"	--port <port> \n"
);
}

static int find_key(const char *key,const char *params)

{
	if(!str_cmp(key,"--help")||!str_cmp(key,"-h")){
		print_help(1);
		exit(0);
	}else if(!str_cmp(key,"--port")||!str_cmp(key,"--max-players")){
		if(!params){
			fprintf(stderr,"jew: %s: option requires a path\n",key);
			print_help(0);
			exit(1);
		}else{
			return 2;
		}
	}else{
		fprintf(stderr,"jew: %s: invalid option\n",key);
		print_help(0);
		exit(1);
	}
}

static char* get_key(char **argv, const char *key)
{
	while(*(++argv))
		if(!str_cmp(*argv,key))
			return *(argv+1);
	return NULL;
}

/* ----- Interface ----- */

void proc_keys(char **argv)
{
	argv++;
	while(*argv){
		argv+=(find_key(*argv,*(argv+1)));
	}
}

int get_int_key(char **argv, const char *key)
{
	return str_to_int( get_key(argv, key) );
}
