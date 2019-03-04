#include "list.hpp"
#include "lexlib.h"
#include "errors.h"

SyntaxException::SyntaxException(node<lexem> *new_lex)
{
	type = "Syntax error";
	lex = new_lex;
}

const char* SyntaxException::Type()
{
	return type;
}

const char* SyntaxException::Lexem()
{
	if(!lex)
		return 0;
	return lex->item.data;
}

int SyntaxException::String()
{
	if(!lex)
		return -1;
	return lex->item.string;
}

IdentifierRedefinitionException::
		IdentifierRedefinitionException(node<lexem> *new_lex)
{
	type = "Identifier redefiniton";
	lex = new_lex;
}

ExpressionExpectedException::
		ExpressionExpectedException(node<lexem> *new_lex)
{
	type = "Expression expected";
	lex = new_lex;
}

IdentifierExpectedException::
		IdentifierExpectedException(node<lexem> *new_lex)
{
	type = "Identifier expected";
	lex = new_lex;
}

SemicolonExpectedException::
		SemicolonExpectedException(node<lexem> *new_lex)
{
	type = "';' expected";
	lex = new_lex;
}

ClosebracketExpectedException::
		ClosebracketExpectedException(node<lexem> *new_lex)
{
	type = "')' expected";
	lex = new_lex;
}

OpenbracketExpectedException::
		OpenbracketExpectedException(node<lexem> *new_lex)
{
	type = "'(' expected";
	lex = new_lex;
}

CommaExpectedException::
		CommaExpectedException(node<lexem> *new_lex)
{
	type = "',' expected";
	lex = new_lex;
}

InitializerExpectedException::
		InitializerExpectedException(node<lexem> *new_lex)
{
	type = "Initializer expected";
	lex = new_lex;
}

LabelExpectedException::
		LabelExpectedException(node<lexem> *new_lex)
{
	type = "Label expected";
	lex = new_lex;
}

IdentifierNotFoundException::
		IdentifierNotFoundException(node<lexem> *new_lex)
{
	type = "Identifier not found";
	lex = new_lex;
}

StatementExpectedException::
		StatementExpectedException(node<lexem> *new_lex)
{
	type = "Statement expected";
	lex = new_lex;
}

ClosebraceExpectedException::
		ClosebraceExpectedException(node<lexem> *new_lex)
{
	type = "'}' expected";
	lex = new_lex;
}

CloseindexExpectedException::
		CloseindexExpectedException(node<lexem> *new_lex)
{
	type = "']' expected";
	lex = new_lex;
}

ExpressionException::
		ExpressionException(node<lexem> *new_lex)
{
	type = "Expression error";
	lex = new_lex;
}

NotImplementedException::
		NotImplementedException(node<lexem> *new_lex)
{
	type = "Feature not implemented yet";
	lex = new_lex;
}

LvalueExpectedException::
		LvalueExpectedException(node<lexem> *new_lex)
{
	type = "Lvalue expected";
	lex = new_lex;
}

ArrayExpectedException::
	ArrayExpectedException(node<lexem> *new_lex)
{
	type = "Array expected";
	lex = new_lex;
}

IntegerLvalueExpectedException::
	IntegerLvalueExpectedException(node<lexem> *new_lex)
{
	type = "Integer lvalue expected";
	lex = new_lex;
}

IntegerExpectedException::
	IntegerExpectedException(node<lexem> *new_lex)
{
	type = "Integer expected";
	lex = new_lex;
}

WrongPrototypeException::
	WrongPrototypeException(node<lexem> *new_lex)
{
	type = "Wrong function prototype";
	lex = new_lex;
}
