#include <stdio.h>
#include "list.hpp"
#include "game.h"
#include "lexlib.h"
#include "syntax.h"
#include "errors.h"
#include "string.h"
#include "parser.h"
#define SA SyntaxAnalyzer 

extern GameClass Game;

var_s::var_s(const char *new_name, int new_type, void *new_value)
{
	name = new_name;
	type = new_type;
	size = -1;
	value = new_value;
}

var_s::var_s(const char *new_name, int new_type, int new_size)
{
	name = new_name;
	type = new_type;
	size = new_size;
	switch(type){
		case VAR_INT_ARR:
			value = new int[size];
			break;
		case VAR_FLOAT_ARR:
			value = new double[size];
			break;
		default:
			value = 0;
	}
}

int var_s::operator==(const var_s& _) const
{
	return !strcmp(name, _.name);
}

SA::SA()
{
}

SA::SA(const List<lexem>& new_program)
{
	Load(new_program);
}

void SA::Load(const List<lexem>& new_program)
{
	program = new_program;
	ptr = program.Begin();
}

void SA::Step()
{
	if(!ptr)
		throw ProgramEndException();
	if(!TryLabel()){
	}else if(!TryPrint()){
	}else if(!TryEndturn()){
	}else if(!TryBuild()){
	}else if(!TryProd()){
	}else if(!TrySell()){
	}else if(!TryBuy()){
	}else if(!TryInt()){
	}else if(!TryFloat()){
	}else if(!TryGoto()){
	}else if(!TryWhile()){
	}else if(!TryIf()){
	}else if(!TryBlock()){
	}else if(!TryEmpty()){
	}else if(!TryExpr()){
	}else{
		throw StatementExpectedException(ptr);
	}
}

int SA::TryLabel()
{
	if(!ptr->next)
		return 1;
	if(ptr->item.type == LX_NAME && ptr->next->item.type == LX_LBL){
		node<var_s> *_ptr = table.Find(var_s(ptr->item.data));
		if(!_ptr)
			table.PushStack(var_s(ptr->item.data, VAR_LABEL,
						(void*)(ptr->next->next)));
		else if(_ptr->item.value != ptr->next->next)
			throw IdentifierRedefinitionException(ptr);
		ptr = ptr->next->next;
		return 0;
	}else
		return 1;
}

int SA::TryPrint()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "print")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		while(ptr->item.type != LX_OPER || strcmp(ptr->item.data, ")")){
			if(ptr->item.type == LX_STR){
				if(running)
					printf("%s ", ptr->item.data);
				ptr = ptr->next;
			}else{
				int type;
				node<lexem>* _ptr = ptr;
				GetExpr(type);
				if(running){
					ptr = _ptr;
					if(type == EXPR_FLOAT)
						printf("%lf ", ParseFloatExpr());
					else
						printf("%d ", ParseIntExpr());
				}
			}
			if(!ptr)
				throw ClosebracketExpectedException(ptr);
			if(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ","))
				ptr = ptr->next;
			else if(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")"))
				break;
			else
				throw ClosebracketExpectedException(ptr);
		}
		if(running)
			printf("\n");
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
			throw SemicolonExpectedException(ptr);
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TryEndturn()
{
	if(ptr->item.type == LX_KEYWORD &&
			!strcmp(ptr->item.data, "endturn")){
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")){
			if(running)
				Game.EndTurn();
			ptr = ptr->next;
			return 0;
		}else
			throw SemicolonExpectedException(ptr);
	}else
		return 1;
}

int SA::TryBuild()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "build")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		int n = ParseIntExpr();
		if(!ptr)
			throw ClosebracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
			throw ClosebracketExpectedException(ptr);
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
			throw SemicolonExpectedException(ptr);
		if(running)
			Game.Build(n);
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TryProd()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "prod")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		int n = ParseIntExpr();
		if(!ptr)
			throw ClosebracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
			throw ClosebracketExpectedException(ptr);
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
			throw SemicolonExpectedException(ptr);
		if(running)
			Game.Prod(n);
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TrySell()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "sell")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		int n = ParseIntExpr();
		if(!ptr)
			throw CommaExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ",")))
			throw CommaExpectedException(ptr);
		ptr = ptr->next;
		int m = ParseIntExpr();
		if(!ptr)
			throw ClosebracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
			throw ClosebracketExpectedException(ptr);
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
			throw SemicolonExpectedException(ptr);
		if(running)
			Game.Sell(n, m);
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TryBuy()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "buy")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		int n = ParseIntExpr();
		if(!ptr)
			throw CommaExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ",")))
			throw CommaExpectedException(ptr);
		ptr = ptr->next;
		int m = ParseIntExpr();
		if(!ptr)
			throw ClosebracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
			throw ClosebracketExpectedException(ptr);
		if(!(ptr = ptr->next))
			throw SemicolonExpectedException(ptr);
		if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
			throw SemicolonExpectedException(ptr);
		if(running)
			Game.Buy(n, m);
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TryInt()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "int")){
		ptr = ptr->next;
		while(TryNewInt()){
		}
		return 0;
	}else
		return 1;
}

int SA::TryFloat()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "float")){
		ptr = ptr->next;
		while(TryNewFloat()){
		}
		return 0;
	}else
		return 1;
}

node<lexem>* SA::GetLabel(node<lexem> *lex)
{
	node<var_s> *lbl = table.Find(var_s(lex->item.data));
	if(lbl){
		if(lbl->item.type != VAR_LABEL)
			throw LabelExpectedException(lex);
		return (node<lexem>*)(lbl->item.value);	
	}
	node<lexem> *n_lex = program.Begin();
	for(;;){
		n_lex = program.Find(lex->item, n_lex);
		if(n_lex == lex)
			throw IdentifierNotFoundException(lex);
		if(n_lex->item.type != LX_NAME)
			continue;
		if(!n_lex->next)
			throw LabelExpectedException(lex);
		if(n_lex->next->item.type != LX_LBL)
			throw LabelExpectedException(lex);
		break;
	}
	n_lex = n_lex->next->next;
	table.PushStack(var_s(n_lex->item.data, VAR_LABEL, n_lex));
	return n_lex;
}

int SA::TryGoto()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "goto")){
		if(!(ptr = ptr->next))
			throw IdentifierExpectedException(ptr);
		if(ptr->item.type != LX_NAME)
			throw IdentifierExpectedException(ptr);
		node<lexem> *lbl = GetLabel(ptr);
		if(!ptr->next)
			throw SemicolonExpectedException(ptr->next);
		if(!(ptr->next->item.type == LX_MISC &&
				!strcmp(ptr->next->item.data, ";")))
			throw SemicolonExpectedException(ptr->next);
		if(running)
			ptr = lbl;
		return 0;
	}else
		return 1;
}

int SA::TryWhile()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "while")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		if(running){
			node<lexem> *c_ptr = ptr, *s_ptr = 0, *e_ptr = 0;
			while(ptr = c_ptr,  ParseIntExpr()){
				if(!s_ptr){
					if(!ptr)
						throw ClosebracketExpectedException(ptr);
					if(!(ptr->item.type == LX_OPER &&
								!strcmp(ptr->item.data, ")")))
						throw ClosebracketExpectedException(ptr);
					if(!(s_ptr = ptr->next))
						throw StatementExpectedException(ptr->next);
				}
				ptr = s_ptr;
				Step();
				if(!e_ptr)
					e_ptr = ptr;
			}
			ptr = e_ptr;
			return 0;
		}else{
			ParseIntExpr();
			if(!ptr)
				throw ClosebracketExpectedException(ptr);
			if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
				throw ClosebracketExpectedException(ptr);
			ptr = ptr->next;
			Step();
			return 0;
		}
	}else
		return 1;
}

int SA::TryIf()
{
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "if")){
		if(!(ptr = ptr->next))
			throw OpenbracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, "(")))
			throw OpenbracketExpectedException(ptr);
		ptr = ptr->next;
		int cond = ParseIntExpr();
		if(!ptr)
			throw ClosebracketExpectedException(ptr);
		if(!(ptr->item.type == LX_OPER && !strcmp(ptr->item.data, ")")))
			throw ClosebracketExpectedException(ptr);
		ptr = ptr->next;
		if(running){
			if(cond){
				Step();
				running = 0;
				if(TryElse())
					ptr = ptr->next;
				running = 1;
			}else{
				running = 0;
				Step();
				running = 1;
				if(TryElse())
					ptr = ptr->next;
			}
		}else{
			Step();
			if(TryElse())
				ptr = ptr->next;
		}
		return 0;
	}else
		return 1;
}

int SA::TryElse()
{
	if(!ptr)
		return 1;
	if(ptr->item.type == LX_KEYWORD && !strcmp(ptr->item.data, "else")){
		ptr = ptr->next;
		Step();
		return 0;
	}else
		return 1;
}

int SA::TryBlock()
{
	if(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, "{")){
		ptr = ptr->next;
		if(!ptr)
			throw ClosebraceExpectedException(ptr);
		while(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, "}"))){
			Step();
			if(!ptr)
				throw ClosebraceExpectedException(ptr);
		}
		ptr = ptr->next;
		return 0;
	}else
		return 1;
}

int SA::TryEmpty()
{
	if(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";"))
		return 0;
	else
		return 1;
}

int SA::TryExpr()
{
	ParseIntExpr();
	if(!ptr)
		throw SemicolonExpectedException();
	if(!(ptr->item.type == LX_MISC && !strcmp(ptr->item.data, ";")))
		throw SemicolonExpectedException();
	ptr = ptr->next;
	return 0;

}

List<lexem> SA::GetExpr(int &type)
{
	List<lexem> expr;
	int sqr_bal = 0, brc_bal = 0;
	int float_flag = 0;
	for(; ptr; expr.Push(ptr->item), ptr = ptr->next){
		if(ptr->item.type == LX_MISC &&
				(!strcmp(ptr->item.data, ",") || !strcmp(ptr->item.data, ";"))){
			if(sqr_bal || brc_bal)
				throw ExpressionException(ptr);
			type = float_flag ? EXPR_FLOAT : EXPR_INT;
			return expr;
		}else if(ptr->item.type == LX_OPER){
			if(!strcmp(ptr->item.data, "["))
				sqr_bal++;
			if(!strcmp(ptr->item.data, "]")){
				if(sqr_bal)
					sqr_bal--;
				else{
					type = float_flag ? EXPR_FLOAT : EXPR_INT;
					return expr;
				}
			}
			if(!strcmp(ptr->item.data, "("))
				brc_bal++;
			if(!strcmp(ptr->item.data, ")")){
				if(brc_bal)
					brc_bal--;
				else{
					type = float_flag ? EXPR_FLOAT : EXPR_INT;
					return expr;
				}
			}
		}else if(ptr->item.type == LX_FLOAT){
			float_flag = 1;
		}else if(ptr->item.type == LX_INT){
		}else if(ptr->item.type == LX_NAME){
			node<var_s> *_ = table.Find(ptr->item.data);
			if(!_)
				throw IdentifierNotFoundException(ptr);
			if(_->item.type == VAR_FLOAT || _->item.type == VAR_FLOAT_ARR)
				float_flag = 1;
		}else if(ptr->item.type == LX_STDNAME){
		}else{
			throw ExpressionException(ptr);
		}
	}
	type = float_flag ? EXPR_FLOAT : EXPR_INT;
	return expr;
}

int SA::ParseIntExpr()
{
	int type;
	List<lexem> expr = GetExpr(type);
	if(running){
		switch(type){
			case EXPR_INT:
				return ExpressionParser(expr.Begin(), table).CalculateInt();
			case EXPR_FLOAT:
				return (int)ExpressionParser(expr.Begin(), table).CalculateFloat();
		}
		throw ExpressionException(ptr);
	}else
		return 1;
}

double SA::ParseFloatExpr()
{
	int type;
	List<lexem> expr = GetExpr(type);
	if(running){
		switch(type){
			case EXPR_INT:
				return (double)ExpressionParser(expr.Begin(), table).CalculateInt();
			case EXPR_FLOAT:
				return ExpressionParser(expr.Begin(), table).CalculateFloat();
		}
		throw ExpressionException(ptr);
	}else
		return 1;
}

int SA::TryNewInt()
{
	if(!(ptr->item.type == LX_NAME))
		throw IdentifierExpectedException(ptr);
	const char *name = ptr->item.data;
	if(table.Find(var_s(name)) != 0 && !running)
		throw IdentifierRedefinitionException(ptr);
	if(!(ptr = ptr->next))
		throw SemicolonExpectedException(ptr);
	switch(ptr->item.type){
		case LX_OPER: // 'name = value' or 'name[size]'
			if(!strcmp(ptr->item.data, "=")){
				ptr = ptr->next;
				/*printf("%d\n", ParseIntExpr());
				throw 1;*/
				table.PushStack(var_s(name, VAR_INT,
							(void*)(new int(ParseIntExpr()))));
			}else if(!strcmp(ptr->item.data, "[")){
				ptr = ptr->next;
				table.PushStack(var_s(name, VAR_INT_ARR, ParseIntExpr()));
				if(!(ptr->item.type == LX_OPER && 
							!strcmp(ptr->item.data, "]")))
					throw CloseindexExpectedException(ptr);
				if(!(ptr = ptr->next))
					throw CloseindexExpectedException(ptr);
			}else
				throw InitializerExpectedException(ptr);
			break;
		case LX_MISC: // just 'name,' or 'name;'
			table.PushStack(var_s(name, VAR_INT, (void*)(new int(0))));
			break;
		default:
			throw SemicolonExpectedException(ptr);
	}
	if(!strcmp(ptr->item.data, ",")){
		ptr = ptr->next;
		return 1;
	}else if(!strcmp(ptr->item.data, ";")){
		ptr = ptr->next;
		return 0;
	}else
		throw SemicolonExpectedException(ptr);
}

int SA::TryNewFloat()
{
	if(!(ptr->item.type == LX_NAME))
		throw IdentifierExpectedException(ptr);
	const char *name = ptr->item.data;
	if(table.Find(var_s(name)) != 0)
		throw IdentifierRedefinitionException(ptr);
	if(!(ptr = ptr->next))
		throw SemicolonExpectedException(ptr);
	switch(ptr->item.type){
		case LX_OPER: // 'name = value' or 'name[size]'
			if(!strcmp(ptr->item.data, "=")){
				ptr = ptr->next;
				table.PushStack(var_s(name, VAR_FLOAT,
							(void*)(new int(ParseFloatExpr()))));
			}else if(!strcmp(ptr->item.data, "[")){
				ptr = ptr->next;
				table.PushStack(var_s(name, VAR_FLOAT_ARR, ParseIntExpr()));
				if(!(ptr->item.type == LX_OPER &&
							!strcmp(ptr->item.data, "]")))
					throw CloseindexExpectedException(ptr);
				if(!(ptr = ptr->next))
					throw CloseindexExpectedException(ptr);
			}else
				throw InitializerExpectedException(ptr);
			break;
		case LX_MISC: // just 'name,' or 'name;'
			table.PushStack(var_s(name, VAR_FLOAT, (void*)(new float(0))));
			break;
		default:
			throw SemicolonExpectedException(ptr);
	}
	if(!strcmp(ptr->item.data, ",")){
		ptr = ptr->next;
		return 1;
	}else if(!strcmp(ptr->item.data, ";")){
		ptr = ptr->next;
		return 0;
	}else
		throw SemicolonExpectedException(ptr);
}


void SA::Run()
{
	running = 1;
	table.Clear();
	ptr = program.Begin();
	while(ptr)
		Step();
}

void SA::Check()
{
	running = 0;
	table.Clear();
	ptr = program.Begin();
	while(ptr)
		Step();
}
