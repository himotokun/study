#ifndef __ERRORS_H
#define __ERRORS_H

#include "list.hpp"
#include "lexlib.h"

class ProgramEndException{
};

class SyntaxException{
protected:
	const char *type;
public:
	node<lexem> *lex;
	SyntaxException(node<lexem> *new_lex = 0);
	const char* Type();
	const char* Lexem();
	int String();
};

class IdentifierRedefinitionException:public SyntaxException{
public:
	IdentifierRedefinitionException(node<lexem> *new_lex = 0);
};

class ExpressionExpectedException:public SyntaxException{
public:
	ExpressionExpectedException(node<lexem> *new_lex = 0);
};

class IdentifierExpectedException:public SyntaxException{
public:
	IdentifierExpectedException(node<lexem> *new_lex = 0);
};

class SemicolonExpectedException:public SyntaxException{
public:
	SemicolonExpectedException(node<lexem> *new_lex = 0);
};

class ClosebracketExpectedException:public SyntaxException{
public:
	ClosebracketExpectedException(node<lexem> *new_lex = 0);
};

class OpenbracketExpectedException:public SyntaxException{
public:
	OpenbracketExpectedException(node<lexem> *new_lex = 0);
};

class CommaExpectedException:public SyntaxException{
public:
	CommaExpectedException(node<lexem> *new_lex = 0);
};

class InitializerExpectedException:public SyntaxException{
public:
	InitializerExpectedException(node<lexem> *new_lex = 0);
};

class LabelExpectedException:public SyntaxException{
public:
	LabelExpectedException(node<lexem> *new_lex = 0);
};

class IdentifierNotFoundException:public SyntaxException{
public:
	IdentifierNotFoundException(node<lexem> *new_lex = 0);
};

class StatementExpectedException:public SyntaxException{
public:
	StatementExpectedException(node<lexem> *new_lex = 0);
};

class ClosebraceExpectedException:public SyntaxException{
public:
	ClosebraceExpectedException(node<lexem> *new_lex = 0);
};

class CloseindexExpectedException:public SyntaxException{
public:
	CloseindexExpectedException(node<lexem> *new_lex = 0);
};

class ExpressionException:public SyntaxException{
public:
	ExpressionException(node<lexem> *new_lex = 0);
};

class NotImplementedException:public SyntaxException{
public:
	NotImplementedException(node<lexem> *new_lex = 0);
};

class LvalueExpectedException:public ExpressionException{
public:
	LvalueExpectedException(node<lexem> *new_lex = 0);
};

class ArrayExpectedException:public ExpressionException{
public:
	ArrayExpectedException(node<lexem> *new_lex = 0);
};

class IntegerLvalueExpectedException:public LvalueExpectedException{
public:
	IntegerLvalueExpectedException(node<lexem> *new_lex = 0);
};

class IntegerExpectedException:public ExpressionException{
public:
	IntegerExpectedException(node<lexem> *new_lex = 0);
};

class WrongPrototypeException:public SyntaxException{
public:
	WrongPrototypeException(node<lexem> *new_lex = 0);
};

class OutOfRangeException{
};

#endif
