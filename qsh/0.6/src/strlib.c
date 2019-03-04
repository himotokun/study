#include <stdlib.h>
#include <stdio.h>

#include "strlib.h"

int str_cmp(const char *src, const char *dst)
{
	while((*dst) || (*src)){
		if(*(src) > *(dst)){
			return 1;
		}
		if(*(src++) < *(dst++)){
			return -1;
		}
	}
	return 0;
}

void str_cpy(char *dst, const char *src)
{
	while((*(dst++) = *(src++))){
	}
}

int str_len(const char *str)
{
	const char *tmp = str;
	while(*(tmp++)){
	}
	return tmp-str-1;
}

char* str_cat(const char *str1, const char *str2)
{
	char *tmp = malloc(str_len(str1)+str_len(str2)+1);
	char *res = tmp;
	while((*(tmp++) = *(str1++))){
	}
	tmp--;
	while((*(tmp++) = *(str2++))){
	}
	return res;
}

char* str_unconst(const char *str)
{
	char *res = malloc(str_len(str)+1);
	str_cpy(res,str);
	return res;
}

char* repl_home(const char *src)
{
	const char *home = getenv("HOME");
	if(*src == '~'){
		if(!*(src+1)){
			return str_unconst(home);
		}
		return str_cat(home,src+1);
	}else{
		const char *tmp = src;
		if(!str_cmp(src,home)){
			return str_unconst("~");
		}
		while(*(tmp++) == *(home++)){
		}
		tmp--,home--;
		if(*home){
			return str_unconst(src);
		}else{
			return str_cat("~",tmp);
		}
	}
}

void free_argv(char **argv)
{
	char **tmp = argv;
	if(!argv)
		return;
	while(*tmp){
		free(*(tmp++));
	}
	free(argv);
}
