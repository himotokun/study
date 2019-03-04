#include <stdio.h>
#include <string.h>
#include "lexlib.h"

#define LA LexicalAnalyzer

enum analyzer_status{
	ST_DFL,
	ST_NAME,
	ST_INT,
	ST_FLOAT,
	ST_OPER,
	ST_STR,
	ST_ERR
};

enum char_type{
	CH_NAME,
	CH_DIGIT,
	CH_SYM,
	CH_MISC,
	CH_SEP,
	CH_OTH,
	CH_LBL = ':',
	CH_FLP = '.',
	CH_STR = '"'
};



lexem::lexem(char *new_data, int new_type, int new_string)
{
	//printf("added %s, type %d, string %d\n",
			//new_data, new_type, new_string);
	data = new_data;
	type = new_type;
	string = new_string;
	if(type == LX_ERR)
		throw *this;
}

lexem::operator char*()
{
	return data;
}

int lexem::operator==(const lexem& _) const
{
	return !strcmp(data, _.data);
}

lexem lexem::Dup()
{
	return lexem(strdup(data), type, string);
}

static int is_digit(char c)
{
	return c >= '0' && c <= '9';
}

static int is_name_ch(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int lex_ch_type(char c)
{
	switch(c){
		case ':':
		case '.':
		case '"':
			return c;
		case '+': case '-': case '*': case '/': case '%':
		case '>': case '<': case '=': case '!': case '&': case '|':
		case '[': case ']': case '(': case ')':
			return CH_SYM;
		case '{': case '}': case ';': case ',':
			return CH_MISC;
		case ' ': case '\t': case '\n':
			return CH_SEP;
		default:
			if(is_name_ch(c))
				return CH_NAME;
			if(is_digit(c))
				return CH_DIGIT;
			return CH_OTH;
	}
}

static int is_oper_lexem(const char *lex)
{
	return (
		!strcmp(lex, "+") || !strcmp(lex, "+=") || !strcmp(lex, "++") ||
		!strcmp(lex, "-") || !strcmp(lex, "-=") || !strcmp(lex, "--") ||
		!strcmp(lex, "*") || !strcmp(lex, "*=") ||
		!strcmp(lex, "/") || !strcmp(lex, "/=") ||
		!strcmp(lex, "%") || !strcmp(lex, "%=") ||
		!strcmp(lex, ">") || !strcmp(lex, ">=") || !strcmp(lex, "==") ||
		!strcmp(lex, "<") || !strcmp(lex, "<=") || !strcmp(lex, "!=") ||
		!strcmp(lex, "!") || !strcmp(lex, "&&") || !strcmp(lex, "||") ||
		!strcmp(lex, "[") || !strcmp(lex, "]") ||
		!strcmp(lex, "(") || !strcmp(lex, ")") || !strcmp(lex, "=")
	);

}

static int is_keyword_lexem(const char *lex)
{
	return (
		!strcmp(lex, "if") || !strcmp(lex, "else") ||
		!strcmp(lex, "while") || !strcmp(lex, "goto") ||
		!strcmp(lex, "int") || !strcmp(lex, "float") ||
		!strcmp(lex, "buy") || !strcmp(lex, "sell") ||
		!strcmp(lex, "build") || !strcmp(lex, "prod") ||
		!strcmp(lex, "upgrade") || !strcmp(lex, "endturn") ||
		!strcmp(lex, "print")
	);
}

static int is_stdname_lexem(const char *lex)
{
	return (
		!strcmp(lex, "my_id") || !strcmp(lex, "turn") ||
		!strcmp(lex, "players") || !strcmp(lex, "active_players") ||
		!strcmp(lex, "supply") || !strcmp(lex, "raw_price") ||
		!strcmp(lex, "demand") || !strcmp(lex, "production_price") ||
		!strcmp(lex, "money") || !strcmp(lex, "raw") ||
		!strcmp(lex, "production") || !strcmp(lex, "factories") ||
		!strcmp(lex, "auto_factories") || !strcmp(lex, "manufactured") ||
		!strcmp(lex, "result_raw_sold") ||
		!strcmp(lex, "result_raw_price") ||
		!strcmp(lex, "result_prod_bought") ||
		!strcmp(lex, "result_prod_price")
	);
}

static int is_name_lexem(const char *lex)
{
	if(!is_name_ch(lex[0]))
		return 0;
	for(int i = 1, len = strlen(lex); i < len; i++)
		if(!is_name_ch(lex[i]) && !is_digit(lex[i]))
			return 0;
	return 1;
}

static int is_misc_lexem(const char *lex)
{
	return (
		!strcmp(lex, "{") || !strcmp(lex, "}") ||
		!strcmp(lex, ";") || !strcmp(lex, ",")
	);
}

static int number_lexem_type(const char *lex)
{
	int point = 0;
	for(int i = 0, len = strlen(lex); i < len; i++){
		if(lex[i] == '.'){
			if(!point){
				point = 1;
				continue;
			}else{
				return LX_ERR;
			}
		}
		if(!is_digit(lex[i]))
			return LX_ERR;
	}
	return point ? LX_FLOAT : LX_INT;
}
	  
static int lexem_type(const char *lex)
{
	if(is_oper_lexem(lex))
		return LX_OPER;
	if(is_misc_lexem(lex))
		return LX_MISC;
	if(is_keyword_lexem(lex))
		return LX_KEYWORD;
	if(is_stdname_lexem(lex))
		return LX_STDNAME;
	if(is_name_lexem(lex))
		return LX_NAME;
	int n;
	if((n = number_lexem_type(lex)) != LX_ERR)
		return n;
	return LX_ERR;
}

/* Pushers */

void LA::PushDfl(char c)
{
	int t = lex_ch_type(c);
	int lt = lexem_type(buf);
	if(!buf.Empty())
		list.Push(lexem(buf.Make(), lt, string));
	status = ST_DFL;
	switch(t){
		case CH_SEP:
			if(c == '\n')
				string++;
			break;
		case CH_LBL: case CH_MISC:
			list.Push(lexem(buf.Push(c).Make(),
						t == CH_LBL ? LX_LBL : LX_MISC, string));
			break;
		case CH_STR:
			status = ST_STR;
			break;
		case CH_FLP:
			buf.Push(c);
			status = ST_FLOAT;
			break;
		default:
			buf.Push(c);
			status = (t == CH_NAME ? ST_NAME : t == CH_DIGIT ? ST_INT :
					t == CH_SYM ? ST_OPER : ST_ERR);
	}
}

void LA::PushName(char c)
{
	int t = lex_ch_type(c);
	switch(t){
		case CH_NAME:
		case CH_DIGIT:
			buf.Push(c);
			break;
		default:
			PushDfl(c);
	}
}

void LA::PushInt(char c)
{
	int t = lex_ch_type(c);
	switch(t){
		case CH_FLP:
			status = ST_FLOAT;
		case CH_DIGIT:
			buf.Push(c);
			break;
		case CH_NAME:
			buf.Push(c);
		default:
			PushDfl(c);
	}
}

void LA::PushFloat(char c)
{
	int t = lex_ch_type(c);
	switch(t){
		case CH_DIGIT:
			buf.Push(c);
			break;
		case CH_NAME:
			buf.Push(c);
		default:
			PushDfl(c);
	}
}

void LA::PushOper(char c)
{
	int t = lex_ch_type(c);
	switch(t){
		case CH_SYM:
			buf.Push(c);
			if(lexem_type(buf) == LX_ERR){
				buf.Pop();
				PushDfl(c);
			}
			break;
		default:
			PushDfl(c);
	}
}

void LA::PushStr(char c)
{
	int t = lex_ch_type(c);
	switch(t){
		case CH_STR:
			list.Push(lexem(buf.Make(), LX_STR, string));
			status = ST_DFL;
			break;
		default:
			buf.Push(c);
	}
}

/* Interface */

LA::LA()
{
	status = LX_DFL;
	string = 1;
}

void LA::Push(char c)
{
	switch(status){
		case ST_DFL:
			PushDfl(c);
			break;
		case ST_NAME:
			PushName(c);
			break;
		case ST_INT:
			PushInt(c);
			break;
		case ST_FLOAT:
			PushFloat(c);
			break;
		case ST_OPER:
			PushOper(c);
			break;
		case ST_STR:
			PushStr(c);
			break;
		default:
			throw lexem(buf.Make(), LX_ERR, string);
	}
}

void LA::Print()
{
	for(node<lexem> *iter = list.Begin(); iter; iter = iter->next){
		printf("[%s]: t %d, s %d\n", iter->item.data,
				iter->item.type, iter->item.string);
	}
}

List<lexem>& LA::GetList()
{
	return list;
}

#undef LA
