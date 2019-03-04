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

int str_to_int(const char *str)
{
	int res = 0, sign;
	if(!str)
		return 0;
	sign = (*str == '-')?str++,1:0;
	while(*str){
		res *= 10;
		res += *(str++) - '0';
	}
	return sign?-res:res;
}

void rm_arr(int *arr, int size, int index)
{
	int i;
	for(i = index; i < size-1; i++)
		arr[i] = arr[i+1];
}

void rm_argv(char **arr, int *size, int index)
{
	int i;
	if(arr[index])
		free(arr[index]);
	for(i = index; i < (*size); i++)
		arr[i] = arr[i+1];
	(*size)--;
}

int str_beg(const char *more, const char *less)
{
	while(*more == ' ')
		more++;
	while(*less == ' ')
		less++;
	if(!str_cmp(more,less))
		return 1;
	while(*(more++) == *(less++)){
	}
	return !*(--less);
}

char *int_to_str(int n)
{
	char buf_data[50];
	char *buffer = buf_data;
	buffer+=49;
	*(buffer--) = 0;
	while(n){
		*(buffer--) = n%10 + '0';
		n/=10;
	}
	return str_unconst(buffer+1);
}
