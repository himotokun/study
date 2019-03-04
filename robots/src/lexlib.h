#ifndef __LEXLIB_H
#define __LEXLIB_H

#include "list.hpp"
#include "buffer.h"

enum lexem_type{
	LX_DFL = 1,
	LX_OPER = 3, // + - * / % < > = == <= >= != && || ! ( ) [ ]
	LX_STR = 4, // without quotes
	LX_LBL = 5, // ":"
	LX_KEYWORD = 6, // if else while goto int float
   				// buy sell build prod upgrade endturn print	
	LX_STDNAME = 7, // my_id etc
	LX_MISC = 8, // { } , ;
	LX_ERR = 0,

	LX_INT = 32,
	LX_FLOAT = 16,
	LX_LVAL = 64,
	LX_PTR = 128,

	//LX_DATA_TYPE = LX_INT|LX_FLOAT,
	//LX_VAL_TYPE = LX_LVAL|LX_PTR,
	//LX_TYPE = LX_DATA_TYPE|LX_VAL_TYPE,

	LX_INT_PTR,// = LX_INT|LX_LVAL|LX_PTR,
	LX_INT_DIR,// = LX_INT|LX_PTR,
	LX_FLOAT_PTR,// = LX_FLOAT|LX_LVAL|LX_PTR,
	LX_FLOAT_DIR,// = LX_FLOAT|LX_PTR,

	LX_NAME// = LX_LVAL
};

struct lexem{
	char *data;
	int type;
	int string;
	lexem(char *new_data, int new_type = LX_DFL, int new_string = -1);
	operator char*();
	int operator==(const lexem& _) const;
	lexem Dup();
};

class LexicalAnalyzer{
	Buffer buf;
	List<lexem> list;
	int status;
	int string;
	void PushDfl(char c);
	void PushName(char c);
	void PushInt(char c);
	void PushFloat(char c);
	void PushOper(char c);
	void PushStr(char c);
public:
	List<lexem>& GetList();
	LexicalAnalyzer();
	void Push(char c);
	void Print();
};

#endif
