#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "lexlib.h"
#include "list.hpp"
#include "errors.h"
#include "syntax.h"
#include "parser.h"

#define EP ExpressionParser

extern GameClass Game;

enum ep_status{
	EP_MODE_INT,
	EP_MODE_FLOAT
};

EP::EP(node<lexem> *start, List<var_s> &var_table):
	ptr(start), table(var_table), type_status(EP_MODE_INT)
{
}

static int std_param(const char *op)
{
	return !(
			!strcmp(op, "my_id") || !strcmp(op, "turn") ||
			!strcmp(op, "players") || !strcmp(op, "active_players") ||
			!strcmp(op, "supply") || !strcmp(op, "raw_price") ||
			!strcmp(op, "demand") || !strcmp(op, "production_price")
	);
}

static int std1(const char *op)
{
	if(!strcmp(op, "my_id"))
		return Game.MyId();
	if(!strcmp(op, "turn"))
		return Game.Turn();
	if(!strcmp(op, "players"))
		return Game.Players();
	if(!strcmp(op, "active_players"))
		return Game.ActivePlayers();
	if(!strcmp(op, "supply"))
		return Game.Supply();
	if(!strcmp(op, "raw_price"))
		return Game.RawPrice();
	if(!strcmp(op, "demand"))
		return Game.Demand();
	if(!strcmp(op, "production_price"))
		return Game.ProductionPrice();
	return 0;
}

static int std2(const char *op, int id)
{
	if(!strcmp(op, "money"))
		return Game.Money(id);
	if(!strcmp(op, "raw"))
		return Game.Raw(id);
	if(!strcmp(op, "production"))
		return Game.Production(id);
	if(!strcmp(op, "factories"))
		return Game.Factories(id);
	if(!strcmp(op, "auto_factories"))
		return Game.AutoFactories(id);
	if(!strcmp(op, "manufactured"))
		return Game.Manufactured(id);
	if(!strcmp(op, "result_raw_sold"))
		return Game.ResultRawSold(id);
	if(!strcmp(op, "result_raw_price"))
		return Game.ResultRawPrice(id);
	if(!strcmp(op, "result_prod_bought"))
		return Game.ResultProdBought(id);
	if(!strcmp(op, "result_prod_price"))
		return Game.ResultProdPrice(id);
	return 0;
}

void EP::Step()
{
	const char *op = op_stack.Begin()->item->item.data;
	//printf("[%s]\n", op);
	if(!strcmp(op, "["))
		Index();
	else if(!strcmp(op, "++")){
		if(op_stack.Begin()->item->next == val_stack.Begin()->item)
			IncPre();
		else
			IncPost();
	}else if(!strcmp(op, "--")){
		if(op_stack.Begin()->item->next == val_stack.Begin()->item)
			DecPre();
		else
			DecPost();
	}else if(!strcmp(op, "!"))
		Not();
	else if(!strcmp(op, "+")){
		if(!val_stack.Begin()->next)
			UnPlus();
		else if(val_stack.Begin()->next->item->next == op_stack.Begin()->item)
			BinPlus();
		else
			UnPlus();
	}else if(!strcmp(op, "-")){
		if(!val_stack.Begin()->next)
			UnMinus();
		else if(val_stack.Begin()->next->item->next == op_stack.Begin()->item)
			BinMinus();
		else
			UnMinus();
	}else if(!strcmp(op, "*"))
		Mul();
	else if(!strcmp(op, "/"))
		Div();
	else if(!strcmp(op, "%"))
		Mod();
	else if(!strcmp(op, ">"))
		Gt();
	else if(!strcmp(op, "<"))
		Lt();
	else if(!strcmp(op, ">="))
		Ge();
	else if(!strcmp(op, "<="))
		Le();
	else if(!strcmp(op, "=="))
		Eq();
	else if(!strcmp(op, "!="))
		Ne();
	else if(!strcmp(op, "&&"))
		And();
	else if(!strcmp(op, "||"))
		Or();
	else if(!strcmp(op, "="))
		Ass();
	else if(!strcmp(op, "+="))
		AssPlus();
	else if(!strcmp(op, "-="))
		AssMinus();
	else if(!strcmp(op, "*="))
		AssMul();
	else if(!strcmp(op, "/="))
		AssDiv();
	else if(!strcmp(op, "%="))
		AssMod();
	else
		StdName();
	op_stack.PopStack();
}

int EP::LastPriority()
{
	if(op_stack.Empty())
		return 255;
	const char *op = op_stack.Begin()->item->item.data;
	if(!strcmp(op, "[") || !strcmp(op, "("))
		return 255;
	else if(!strcmp(op, "++") || !strcmp(op, "--")){
		if(val_stack.Begin()->item->next == op_stack.Begin()->item)
			return 2;
		else
			return 3;
	}else if(!strcmp(op, "!"))
		return 4;
	else if(!strcmp(op, "+") || !strcmp(op, "-")){
		if(val_stack.Begin()->item->next == op_stack.Begin()->item)
			return 7;
		else
			return 5;
	}else if(!strcmp(op, "*") || !strcmp(op, "/") || !strcmp(op, "%"))
		return 6;
	else if(!strcmp(op, ">") || !strcmp(op, "<") ||
				!strcmp(op, ">=") || !strcmp(op, "<="))
		return 8;
	else if(!strcmp(op, "==") || !strcmp(op, "!="))
		return 9;
	else if(!strcmp(op, "&&"))
		return 10;
	else if(!strcmp(op, "||"))
		return 11;
	else if(!strcmp(op, "=") || !strcmp(op, "+=") || !strcmp(op, "-=") ||
			!strcmp(op, "*=") || !strcmp(op, "/=") || !strcmp(op, "%="))
		return 12;
	else
		return -1;
}

int EP::CurrentPriority()
{
	const char *op = ptr->item.data;
	if(!strcmp(op, "["))
		return 1;
	else if(!strcmp(op, "++") || !strcmp(op, "--")){
		if(val_stack.Empty())
			return 3;
		if(val_stack.Begin()->item->next == ptr)
			return 2;
		else
			return 3;
	}else if(!strcmp(op, "!"))
		return 4;
	else if(!strcmp(op, "+") || !strcmp(op, "-")){
		if(val_stack.Empty())
			return 5;
		if(val_stack.Begin()->item->next == ptr)
			return 7;
		else
			return 5;
	}else if(!strcmp(op, "*") || !strcmp(op, "/") || !strcmp(op, "%"))
		return 6;
	else if(!strcmp(op, ">") || !strcmp(op, "<") ||
				!strcmp(op, ">=") || !strcmp(op, "<="))
		return 8;
	else if(!strcmp(op, "==") || !strcmp(op, "!="))
		return 9;
	else if(!strcmp(op, "&&"))
		return 10;
	else if(!strcmp(op, "||"))
		return 11;
	else if(!strcmp(op, "=") || !strcmp(op, "+=") || !strcmp(op, "-=") ||
			!strcmp(op, "*=") || !strcmp(op, "/=") || !strcmp(op, "%="))
		return 12;
	else
		return -1;
}

int EP::CalculateInt()
{
	for(; ptr/*->next*/; ptr = ptr->next){
		//printf("{%s}\n", ptr->item.data);
		switch(ptr->item.type){
			case LX_NAME:
			case LX_INT:
			case LX_FLOAT:
				val_stack.PushStack(ptr);
				break;
			case LX_OPER:
				if(!strcmp(ptr->item.data, ")")){
					if(!op_stack.Empty()){
						GoUntilBracket('(');
						break;
					}else
						return MakeInt();
				}else if(!strcmp(ptr->item.data, "]"))
					GoUntilBracket('[');
				else if(!strcmp(ptr->item.data, "[") ||
							!strcmp(ptr->item.data, "(")){
					op_stack.PushStack(ptr);
					break;
				}else if(CurrentPriority() >= LastPriority()){
						Step();
						op_stack.PushStack(ptr);
				}else
					op_stack.PushStack(ptr);
				break;
			case LX_STDNAME:
				if(std_param(ptr->item.data)){
					if(strcmp(ptr->next->item.data, "("))
						throw OpenbracketExpectedException(ptr);
					op_stack.PushStack(ptr);
				}else
					val_stack.PushStack(ptr);
			break;
			default:
				throw ExpressionException(ptr);
		}
	}
	return MakeInt();
}

double EP::CalculateFloat()
{
	for(; ptr/*->next*/; ptr = ptr->next){
		//printf("{%s}\n", ptr->item.data);
		switch(ptr->item.type){
			case LX_NAME:
			case LX_INT:
			case LX_FLOAT:
				val_stack.PushStack(ptr);
				break;
			case LX_OPER:
				if(!strcmp(ptr->item.data, ")")){
					if(!op_stack.Empty()){
						GoUntilBracket('(');
						break;
					}else
						return MakeInt();
				}else if(!strcmp(ptr->item.data, "]"))
					GoUntilBracket('[');
				else if(!strcmp(ptr->item.data, "[") ||
							!strcmp(ptr->item.data, "(")){
					op_stack.PushStack(ptr);
					break;
				}else if(CurrentPriority() >= LastPriority()){
						Step();
						op_stack.PushStack(ptr);
				}else
					op_stack.PushStack(ptr);
				break;
			default:
				throw ExpressionException(ptr);
		}
	}
	return MakeFloat();
}

int EP::GetIntRvalue()
{
	node<lexem> *lex = val_stack.Begin()->item;
	switch(lex->item.type){
		case LX_INT:
			return atoi(lex->item.data);
		case LX_FLOAT:
			return (int)atof(lex->item.data);
		case LX_INT_DIR:
		case LX_INT_PTR:
			return *((int*)(lex->item.data));
		case LX_FLOAT_DIR:
		case LX_FLOAT_PTR:
			return (int)*((double*)(lex->item.data));
		case LX_NAME:{
			node<var_s> *tbl = table.Find(lex->item.data);
			if(!tbl)
				throw IdentifierNotFoundException(lex);
			switch(tbl->item.type){
				case VAR_INT:
					return *((int*)(tbl->item.value));
				case VAR_FLOAT:
					return (int)*((double*)(tbl->item.value));
				default:
					throw ExpressionException(lex);
			}
		}
		break;
		case LX_STDNAME:
			return std1(lex->item.data);
		default:
			throw ExpressionException(lex);
	}
}

double EP::GetFloatRvalue()
{
	node<lexem> *lex = val_stack.Begin()->item;
	switch(lex->item.type){
		case LX_INT:
			return (double)atoi(lex->item.data);
		case LX_FLOAT:
			return atof(lex->item.data);
		case LX_INT_PTR:
		case LX_INT_DIR:
			return (double)*((int*)(lex->item.data));
		case LX_FLOAT_PTR:
		case LX_FLOAT_DIR:
			return *((double*)(lex->item.data));
		case LX_NAME:{
			node<var_s> *tbl = table.Find(lex->item.data);
			if(!tbl)
				throw IdentifierNotFoundException(lex);
			switch(tbl->item.type){
				case VAR_INT:
					return (double)*((int*)(tbl->item.value));
				case VAR_FLOAT:
					return *((double*)(tbl->item.value));
				default:
					throw ExpressionException(lex);
			}
		}
		case LX_STDNAME:
			return (double)std1(lex->item.data);
		break;
		default:
			throw ExpressionException(lex);
	}
}

void* EP::GetLvalue(int &type)
{
	node<lexem> *lex = val_stack.Begin()->item;
	switch(lex->item.type){
		case LX_INT: case LX_INT_DIR:
		case LX_FLOAT: case LX_FLOAT_DIR:
			throw LvalueExpectedException(lex);
		case LX_NAME:{
			node<var_s> *tbl = table.Find(lex->item.data);
			if(!tbl)
				throw IdentifierNotFoundException(lex);
			type = tbl->item.type;
			return tbl->item.value;
		}
		break;
		case LX_INT_PTR:
			type = VAR_INT;
			return (void*)(lex->item.data);
		case LX_FLOAT_PTR:
			type = VAR_FLOAT;
			return (void*)(lex->item.data);
		default:
			throw ExpressionException(lex);
	}
}

void EP::Index()
{
	int type;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int idx = GetIntRvalue();
	val_stack.PopStack();
	node<lexem> *where_to = val_stack.Begin()->item;
	void *v_arr = GetLvalue(type);
	void *new_ptr;
	switch(type){
		case VAR_INT_ARR:
			new_ptr = (int*)v_arr + idx;
			break;
		case VAR_FLOAT_ARR:
			new_ptr = (double*)v_arr + idx;
			break;
		default:
			throw ArrayExpectedException(ptr);
	}
	where_to->item.type = type == VAR_INT_ARR ? LX_INT_PTR : LX_FLOAT_PTR;
	where_to->item.data = (char*)new_ptr;
}

void EP::IncPost()
{
	int type;
	void *v_arr = GetLvalue(type);
	if(type != VAR_INT)
		throw IntegerLvalueExpectedException(ptr);
	val_stack.Begin()->item->item.type = LX_INT_DIR;
	val_stack.Begin()->item->item.data = (char*)(new int((*(int*)v_arr)++));
}

void EP::IncPre()
{
	int type;
	void *v_arr = GetLvalue(type);
	if(type != VAR_INT)
		throw IntegerLvalueExpectedException(ptr);
	val_stack.Begin()->item->item.type = LX_INT_DIR;
	val_stack.Begin()->item->item.data = (char*)(new int(++(*(int*)v_arr)));
}

void EP::DecPost()
{
	int type;
	void *v_arr = GetLvalue(type);
	if(type != VAR_INT)
		throw IntegerLvalueExpectedException(ptr);
	val_stack.Begin()->item->item.type = LX_INT_DIR;
	val_stack.Begin()->item->item.data = (char*)(new int((*(int*)v_arr)--));
}

void EP::DecPre()
{
	int type;
	void *v_arr = GetLvalue(type);
	if(type != VAR_INT)
		throw IntegerLvalueExpectedException(ptr);
	val_stack.Begin()->item->item.type = LX_INT_DIR;
	val_stack.Begin()->item->item.data = (char*)(new int(--(*(int*)v_arr)));
}

void EP::Not()
{
	int v = GetIntRvalue();
	val_stack.Begin()->item->item.type = LX_INT_DIR;
	val_stack.Begin()->item->item.data = (char*)(new int(!v));

}

void EP::UnPlus()
{
	switch(VarType(val_stack.Begin()->item)){
		case LX_INT:{
			int op = GetIntRvalue();
			val_stack.Begin()->item->item.type = LX_INT_DIR;
			val_stack.Begin()->item->item.data = (char*)(new int(op));
		}
		break;
		case LX_FLOAT:{
			double op = GetFloatRvalue();
			val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
			val_stack.Begin()->item->item.data = (char*)(new double(op));
		}
		break;
	}
}

void EP::UnMinus()
{
	switch(VarType(val_stack.Begin()->item)){
		case LX_INT:{
			int op = GetIntRvalue();
			val_stack.Begin()->item->item.type = LX_INT_DIR;
			val_stack.Begin()->item->item.data = (char*)(new int(-op));
		}
		break;
		case LX_FLOAT:{
			double op = GetFloatRvalue();
			val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
			val_stack.Begin()->item->item.data = (char*)(new double(-op));
		}
		break;
	}
}

int EP::VarType(node<lexem> *lex)
{
	switch(lex->item.type){
		case LX_FLOAT: case LX_FLOAT_PTR: case LX_FLOAT_DIR:
			return LX_FLOAT;
		case LX_INT: case LX_INT_PTR: case LX_INT_DIR:
			return LX_INT;
		case LX_NAME:{
			var_s v = table.Find(var_s(lex->item.data))->item;
			switch(v.type){
				case VAR_INT:
					return LX_INT;
				case VAR_FLOAT:
					return LX_FLOAT;
				default:
					throw ExpressionException(lex);
			}
		}
		default:
			throw ExpressionException(lex);
	};
}

void EP::Mul()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op1*op2));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1*op2));
	}
}

void EP::Div()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2/op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2/op1));
	}
}

void EP::Mod()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	if(float_flag){
		throw IntegerExpectedException(val_stack.Begin()->item);
	}else{
		val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2%op1));
	}
}

void EP::BinPlus()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op1+op2));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1+op2));
	}
}

void EP::BinMinus()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2-op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2-op1));
	}
}

void EP::Gt()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2>op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2>op1));
	}
}

void EP::Lt()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2<op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2<op1));
	}
}

void EP::Ge()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2>=op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2>=op1));
	}
}

void EP::Le()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op2<=op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op2<=op1));
	}
}

void EP::Eq()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op1==op2));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1==op2));
	}
}

void EP::Ne()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		double op2 = GetFloatRvalue();
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new double(op1!=op2));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1!=op2));
	}
}

void EP::And()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	if(float_flag){
		throw IntegerExpectedException(val_stack.Begin()->item);
	}else{
		val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1&&op2));
	}
}

void EP::Or()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->item) == LX_FLOAT ||
			VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	if(float_flag){
		throw IntegerExpectedException(val_stack.Begin()->item);
	}else{
		val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		int op2 = GetIntRvalue();
		val_stack.Begin()->item->item.type = LX_INT_DIR;
		val_stack.Begin()->item->item.data = (char*)(new int(op1||op2));
	}
}

void EP::Ass()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(double*)op2 = op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 = op1));
	}
}

void EP::AssPlus()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(double*)op2 += op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 += op1));
	}
}

void EP::AssMinus()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(double*)op2 -= op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 -= op1));
	}
}

void EP::AssMul()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(double*)op2 *= op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 *= op1));
	}
}

void EP::AssDiv()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		double op1 = GetFloatRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(double*)op2 /= op1));
	}else{
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 /= op1));
	}
}

void EP::AssMod()
{
	int float_flag = 0;
	if(VarType(val_stack.Begin()->next->item) == LX_FLOAT)
		float_flag = 1;
	val_stack.Begin()->next->item->next = val_stack.Begin()->item->next;
	int type;
	if(float_flag){
		throw IntegerLvalueExpectedException(val_stack.Begin()->next->item);
	}else{
		if(VarType(val_stack.Begin()->item) == LX_FLOAT)
			throw IntegerExpectedException(val_stack.Begin()->item);
		int op1 = GetIntRvalue();
		val_stack.PopStack();
		void *op2 = GetLvalue(type);
		val_stack.Begin()->item->item.type = LX_FLOAT_DIR;
		val_stack.Begin()->item->item.data =
			(char*)(new double(*(int*)op2 %= op1));
	}
}

int EP::MakeInt()
{
	while(!op_stack.Empty())
		Step();
	int res = GetIntRvalue();
	val_stack.PopStack();
	//printf("ret %d\n", res);
	return res;
}

double EP::MakeFloat()
{
	while(!op_stack.Empty())
		Step();
	double res = GetFloatRvalue();
	val_stack.PopStack();
	//printf("ret %lf\n", res);
	return res;
}

void EP::GoUntilBracket(char br_type)
{
	switch(br_type){
		case '(':
			while(strcmp(op_stack.Begin()->item->item.data, "("))
				Step();
			op_stack.PopStack();
			break;
		case '[':
			while(strcmp(op_stack.Begin()->item->item.data, "["))
				Step();
			Step();
			break;
		default:
			throw ExpressionException(op_stack.Begin()->item);
	}
}

void EP::StdName()
{
	if(op_stack.Begin()->item->item.type != LX_STDNAME)
		throw ExpressionException(op_stack.Begin()->item);
	int res = std2(op_stack.Begin()->item->item.data, GetIntRvalue());
	val_stack.Begin()->item->item.type = LX_INT_PTR;
	val_stack.Begin()->item->item.data = (char*)(new int(res));
}

#undef EP
