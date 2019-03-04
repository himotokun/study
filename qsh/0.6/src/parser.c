#include <stdlib.h>
#include <stdio.h>

#include "strlib.h"
#include "defs.h"

#include "parser.h"

/* ----- Constants and structues ----- */

enum{ std_buffer = 32 };

struct w_lst{
	char *word;
	int ctrl;
	struct w_lst *next;
};

struct buffer{
	char *data;
	int ptr;
	int size;
};

enum get_wlst_flags{
	CF_ESC = 1,
	CF_QUOT = 2,
	CF_APOS = 4,
	CF_QTED = 8,
	CF_CTRL = 16,


	CF_EA = CF_ESC|CF_APOS,
	CF_EQ = CF_ESC|CF_QUOT,
	CF_AQ = CF_APOS|CF_QUOT,
	CF_AEQ = CF_APOS|CF_ESC|CF_QUOT
};

enum ctypes{
	CT_USL = 0,
	CT_WSEP = 1,
	CT_CSEP = 2,
	CT_EOF = EOF
};

/* ----- Common functions ----- */

static struct buffer* new_buf()
{
	struct buffer *buf = malloc(sizeof(*buf));
	buf->data = malloc(buf->size = std_buffer);
	buf->ptr = 0;
	return buf;
}

static void free_buf(struct buffer *buf)
{
	if(buf){
		if(buf->data){
			free(buf->data);
		}
		free(buf);
	}
}

/*static struct buffer* double_buf(struct buffer *buf)
{
	struct buffer *new_buf = malloc(sizeof(*new_buf));
	new_buf->ptr = buf->ptr;
	new_buf->data = malloc(new_buf->size = 2*(buf->size));
	str_cpy(new_buf->data, buf->data, buf->ptr);
	free_buf(buf);
	return new_buf;
} */

static void double_buf(struct buffer *buf)
{
	buf->data = realloc(buf->data, buf->size *= 2);
}

static void char2buf(struct buffer *buf, int chr)
{
	(buf->data)[(buf->ptr)++] = chr;
	if(buf->ptr == buf->size){
		double_buf(buf);
	}
}

static struct w_lst* buf2word(struct buffer *buf, struct w_lst *list, int ctrl)
{
	struct w_lst *word = malloc(sizeof(*word)), *t_list = list;
	if(buf){
		word->word = malloc(buf->ptr+1);
		char2buf(buf,0);
		str_cpy(word->word, buf->data);
		buf->ptr = 0;
	}else{
		word->word = malloc(1);
		*(word->word) = 0;
	}
	word->next = NULL;
	if(list){
		while(t_list->next)
			t_list = t_list->next;
		t_list->next = word;
	}else{
		list = word;
	}
	word->ctrl = ctrl?1:0;
	return list;
}

/*static void print_wlst(struct w_lst *list)
{
	while(list){
		printf("[\"%s\", %d]\n", list->word, list->ctrl);
		list = list->next;
	}
}*/

static void free_wlst(struct w_lst *lst)
{
	while(lst){
		struct w_lst *tmp = lst;
		lst = lst->next;
		if(tmp->word)
			if(*tmp->word)
				free(tmp->word);
		free(tmp);
	}
}

/*static void sdfree_wlst(struct w_lst *list)
{
	while(list){
		struct w_lst *tmp = list;
		list = list->next;
		free(tmp);
	}
}*/


static int ctype(int tmp_char)
{
	switch(tmp_char){
		case EOF:return CT_EOF;
		case ' ':
		case '\t':return CT_WSEP;
		case '\n':
		case ';':
		case '&':
		case '|':
		case '>':
		case '<':return CT_CSEP;
		default:return CT_USL;
	}
}

static int proc_quotes(int c, int *flags)
{
	if(c == '\\' && !((*flags)&CF_ESC)){
		(*flags) |= CF_ESC;
		return CF_ESC;
	}
	if(c == '"' && !((*flags)&CF_EA)){
		(*flags) ^= CF_QUOT;
		(*flags) |= CF_QTED;
		return CF_QUOT;
	}
	if(c == '\'' && !((*flags)&CF_EQ)){
		(*flags) ^= CF_APOS;
		(*flags) |= CF_QTED;
		return CF_APOS;
	}
	return 0;
}

/* The main functions */

static struct w_lst* get_wlst()
{
	int tmp_char, t_flags = 0;
	struct w_lst *l_head = NULL;
	struct buffer *buf = new_buf();

	for(;;){
		tmp_char = getchar();
		if(proc_quotes(tmp_char,&t_flags))
			continue;
		if(ctype(tmp_char) && !(t_flags&CF_AEQ)){
			switch(ctype(tmp_char)){
				case CT_EOF:
					return NULL;
				case CT_WSEP:
					if(buf->ptr || t_flags&CF_QTED)
						l_head = buf2word(buf, l_head, t_flags&CF_CTRL);
					t_flags &= ~(CF_CTRL|CF_QTED);
				break;
				case CT_CSEP:
					if(tmp_char == '\n' && !(t_flags&CF_AEQ)){
						if(buf->ptr || t_flags&CF_QTED)
							l_head = buf2word(buf, l_head , t_flags&CF_CTRL);
						free_buf(buf);
						return l_head?l_head:buf2word(buf,NULL,0);
					}
					if(t_flags&CF_CTRL && tmp_char == (buf->data)[buf->ptr-1]){
						char2buf(buf, tmp_char);
					}else{
						if(buf->ptr || t_flags&CF_QTED)
								l_head = buf2word(buf, l_head, t_flags&CF_CTRL);
						char2buf(buf, tmp_char);
					}
					t_flags |= CF_CTRL;
			}
		}else{
			if(t_flags&CF_CTRL){
				l_head = buf2word(buf, l_head, 1);
				t_flags &= ~(CF_CTRL|CF_QTED);
			}
			char2buf(buf,tmp_char);
			t_flags &= ~CF_ESC;
			if(tmp_char == '\n')
				printf("> ");
		}
	}
}

static char** lst2argv2(struct w_lst *lst, int w_cnt)
{
	int i;
	char **argv;
	if(!w_cnt)
		return NULL;
	argv = malloc((w_cnt+1)*sizeof(char*));
	for(i = 0; i<w_cnt; i++){
		argv[i] = str_unconst(lst->word);
		lst = lst->next;
	}
	argv[w_cnt] = NULL;
	return argv;
}

/* static char** lst2argv(struct w_lst *lst)
{
	struct w_lst *l_tmp = lst;
	int w_cnt = 0;
	if(!lst)
		return NULL;
	for(; l_tmp; l_tmp = l_tmp->next)
		w_cnt++;
	return lst2argv2(lst, w_cnt);
} */


static struct c_lst* add_cmd(struct c_lst *lst,
		char **argv, int flags, char **io_path)
{
	struct c_lst *cmd = malloc(sizeof(*cmd));
	struct c_lst *head = lst;
	cmd->argv = argv;
	cmd->flags = flags;
	if(!io_path)
		cmd->io_path = NULL;
	else if(io_path[0] || io_path[1]){
		cmd->io_path = malloc(2*sizeof(char*));
		(cmd->io_path)[0] = io_path[0];
		(cmd->io_path)[1] = io_path[1];
	} else
		cmd->io_path = NULL;
	cmd->next = NULL;
	if(!lst)
		return cmd;
	while(lst->next)
		lst = lst->next;
	lst->next = cmd;
	return head;
}

static int flags(const char *str)
{
	if(!str_cmp(str,";"))
		return 0;
	if(!str_cmp(str,"&"))
		return C_BG;
	if(!str_cmp(str,"&&"))
		return C_AND;
	if(!str_cmp(str,"||"))
		return C_OR;
	if(!str_cmp(str,"|"))
		return C_CONV;
	if(!str_cmp(str,"<"))
		return C_IN;
	if(!str_cmp(str,">"))
		return C_OUT;
	if(!str_cmp(str,">>"))
		return C_APP;
	return 0;
}



/* ----- External interfaces ----- */



void free_clst(struct c_lst *lst)
{
	while(lst){
		struct c_lst *tmp = lst->next;
		free_argv(lst->argv);
		if(lst->io_path&&(lst->flags&C_IO)){
			if((lst->io_path)[0]&&(lst->flags&C_IN))
				free((lst->io_path)[0]);
			if((lst->io_path)[1]&&(lst->flags&(C_OUT|C_APP)))
				free((lst->io_path)[1]);
			free(lst->io_path);
		}
		free(lst);
		lst = tmp;
	}
}

void print_clst(struct c_lst *lst)
{
	if(!lst)
		printf("<NULL>\n");
	while(lst){
		printf("<");
		if(lst->argv){
			int i;
			for(i=0;lst->argv[i];i++)
				printf("\"%s\", ",lst->argv[i]);
			if(lst->flags&C_IN)
				printf("i: %s ",lst->io_path[0]);
			if(lst->flags&(C_OUT|C_APP))
				printf("o: %s ",lst->io_path[1]);

		}
		printf("%d> ",lst->flags);
		lst = lst->next;
	}
	printf("\n");
}

struct c_lst* get_clst()
{
	struct c_lst *res = NULL;
	struct w_lst *lst = get_wlst();
	struct w_lst *l_head = lst, *c_head = lst;
	char f_cur = 0, f_new = 0, w_cnt = 0, err = 0, *io_path[2] = {NULL, NULL};
	if(!lst)
		return NULL;
	if(!*lst->word)
		return add_cmd(NULL, NULL, 0, NULL);
	for(;;lst = lst->next){
		if(err){
			free_clst(res);
			free_wlst(l_head);
			return add_cmd(NULL, NULL, err, NULL);
		}
		if(!lst){
			if(w_cnt)
				res = add_cmd(res, lst2argv2(c_head, w_cnt), f_cur, io_path);
			free_wlst(l_head);
			return res;
		}
		if(!lst->ctrl){
			w_cnt++;
			continue;
		}
		/* Controls */
		if(!w_cnt /*&& (f_cur&~C_IO)*/){
			err = CERR_SEVCTRL;
			continue;
		}
		f_new = flags(lst->word);
		if(f_new&C_IO){
			/* If no usual word after io redir */
			if( (f_new&f_cur&C_IO) 
					|| ((f_new|f_cur)&(C_OUT|C_APP))==(C_OUT|C_APP)
					|| (!lst->next) || (lst->next->ctrl) ){
				err = CERR_NOIO;
				continue;
			}
			io_path[(f_new==C_IN)?0:1] = str_unconst((lst = lst->next)->word);
			/* If several words in io redir */
			if(lst->next)
				if(!lst->next->ctrl)
					err = CERR_SEVIO;
			f_cur |= f_new;
			continue;
		}
		if((f_new&C_SEP) || !f_new){
			/* If the last word is ctrl except & */
			if( (!lst->next && !(f_new&C_BG) && f_new)
				   || ((lst->next) && (lst->next->ctrl)) ){
				err = CERR_ENDCTRL;
				continue;
			}
			if(((f_cur|f_new)&C_IO) && ((f_cur|f_new)&C_CONV)==C_CONV ){
				err = CERR_CONVIO;
				continue;
			}
			res = add_cmd(res, lst2argv2(c_head, w_cnt), f_cur|f_new, io_path);
			w_cnt = 0;
			f_cur = 0;
			c_head = lst->next;
		}
	}
}
