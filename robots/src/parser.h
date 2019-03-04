#ifndef __PARSER_H
#define __PARSER_H

#include "lexlib.h"
#include "syntax.h"
#include "list.hpp"

class ExpressionParser{
	node<lexem> *ptr;
	List<var_s> &table;

	int type_status;
	List<node<lexem>*> op_stack;
	List<node<lexem>*> val_stack;
	//int* ParseIdxInt(node<lexem> *name);
	//double* ParseIdxFloat(node<lexem> *name);
	void Step(); // makes one operation
	void GoUntilBracket(char br_type); // steps until '(' or '[', saves [
	int MakeInt(); // final step
	double MakeFloat(); // same
	int CurrentPriority();
	int LastPriority();
	int VarType(node<lexem> *lex);

	int GetIntRvalue();
	double GetFloatRvalue();
	void* GetLvalue(int &type);
	
	typedef void (*v2v)();
	v2v GetFunction(const char *op);

	void Index();
	void IncPost();
	void DecPost();
	void IncPre();
	void DecPre();
	void Not();
	void UnPlus();
	void UnMinus();
	void Mul();
	void Div();
	void Mod();
	void BinPlus();
	void BinMinus();
	void Gt();
	void Lt();
	void Ge();
	void Le();
	void Eq();
	void Ne();
	void And();
	void Or();
	void Ass();
	void AssPlus();
	void AssMinus();
	void AssMul();
	void AssDiv();
	void AssMod();
	void StdName();

public:
	ExpressionParser(node<lexem> *start, List<var_s> &var_table);
	int CalculateInt();
	double CalculateFloat();

};

#endif
