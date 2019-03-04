#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "strlib.h"

#include "keylib.h"

static void go_home()
{
	char *home = getenv("HOME");
	if(home){
		if(chdir(home)){
			perror(home);
		}
	}
}

static void print_help(int name_flg)
{
	if(name_flg)
		printf("SANIO qsh, version 0.6\n");
	printf(
"Usage: qsh [options]\n"
"Options:\n"
"	-d | --dir <directory>\n"
"	-h | --help\n"
"	-H | --home\n"
);
}

static int find_key(const char *key,const char *params)
{
	if(!str_cmp(key,"--help")||!str_cmp(key,"-h")){
		print_help(1);
		exit(0);
	}else if(!str_cmp(key,"--home")||!str_cmp(key,"-H")){
		go_home();
		return 1;
	}else if(!str_cmp(key,"--dir")||!str_cmp(key,"-d")){
		if(!params){
			fprintf(stderr,"qsh: %s: option requires a path\n",key);
			print_help(0);
			exit(1);
		}else{
			if(chdir(params)){
				fprintf(stderr,"qsh: ");
				perror(params);
				exit(1);
			}
			return 2;
		}
	}else{
		fprintf(stderr,"qsh: %s: invalid option\n",key);
		print_help(0);
		exit(1);
	}
}

/* ----- Interface ----- */

void proc_keys(char **argv)
{
	argv++;
	while(*argv){
		argv+=(find_key(*argv,*(argv+1)));
	}
}
