#ifndef STRLIB_H
#define STRLIB_H

int str_cmp(const char *src, const char *dst);
void str_cpy(char *dst, const char *src);
int str_len(const char *str);
char* str_cat(const char *str1, const char *str2);
char* str_unconst(const char *str);
char* repl_home(const char *src);
void free_argv(char **argv);

#endif
