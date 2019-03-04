#ifndef __SYNTAX_H
#define __SYNTAX_H
#include "list.hpp"
#include "lexlib.h"

enum var_types{
	VAR_INT,
	VAR_INT_ARR,
	VAR_FLOAT,
	VAR_FLOAT_ARR,
	VAR_LABEL
};

enum expr_types{
	EXPR_INT,
	EXPR_FLOAT,
	EXPR_ERR
};

struct var_s{
	const char *name;
	int type, size;
	void *value;
	var_s(const char *new_name, int new_type = -1, void *new_value = 0);
	var_s(const char *new_name, int new_type, int new_size);
	int operator==(const var_s& _) const;
};

class SyntaxAnalyzer{
	int running;
	List<var_s> table;
	List<lexem> program;
	node<lexem> *ptr;
	int TryLabel(); // +
	int TryPrint();
	int TryEndturn(); // +
	int TryBuild(); // +
	int TryProd(); // +
	int TrySell(); // +
	int TryBuy(); // +
	int TryInt(); // +
	int TryFloat(); // +
	int TryGoto(); // +
	int TryWhile(); // +
	int TryIf(); // +
	int TryElse(); // +
	int TryBlock(); // +
	int TryEmpty(); // +
	int TryExpr(); // +
	int ParseIntExpr(); // if !running just check and return 1 if ok
	double ParseFloatExpr(); // same
	int TryNewInt(); // +
	int TryNewFloat(); // +
	node<lexem>* GetLabel(node<lexem> *lex); // + ??
	void Step(); // +
	List<lexem> GetExpr(int &type);

public:
	SyntaxAnalyzer();
	SyntaxAnalyzer(const List<lexem>& new_program);
	void Load(const List<lexem>& new_program);
	void Run();
	void Check();
};

#endif
