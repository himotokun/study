#ifndef __SYMBOLS_H
#define __SYMBOLS_H

enum symbols{
	SYM_PLUS = 0x01,
	SYM_PLUS_EQ = 0x02,
	SYM_INC = 0x03,
	SYM_MINUS = 0x04,
	SYM_MINUS_EQ = 0x05,
	SYM_DEC = 0x06,
	SYM_MUL = 0x07,
	SYM_MUL_EQ = 0x08,
	SYM_DIV = 0x09,
	SYM_DIV_EQ = 0x0A,
	SYM_MOD = 0x0B,
	SYM_MOD_EQ = 0x0C,
	SYM_GT = 0x10,
	SYM_GE = 0x11,
	SYM_EQ = 0x12,
	SYM_LT = 0x13,
	SYM_LE = 0x14,
	SYM_NE = 0x15,
	SYM_NOT = 0x18,
	SYM_AND = 0x19,
	SYM_OR = 0x1A,
	SYM_IDX_OP = 0x20,
	SYM_IDX_CL = 0x21,
	SYM_BRC_OP = 0x30,
	SYM_BRC_CL = 0x31,
	SYM_ASSIGN = 0x40,

	SYM_IF = 0x50,
	SYM_ELSE = 0x51,
	SYM_WHILE = 0x52,
	SYM_GOTO = 0x53,
	SYM_INT = 0x54,
	SYM_FLOAT = 0x55,
	SYM_SEMICOLON = 0x56,
	SYM_COMMA = 0x57,
	SYM_BLOCK_OP = 0x58,
	SYM_BLOCK_CL = 0x59,

	SYM_BUY = 0x80,
	SYM_SELL = 0x81,
	SYM_BUILD = 0x82,
	SYM_PROD = 0x83,
	SYM_UPGRADE = 0x84,
	SYM_ENDTURN = 0x85,
	SYM_PRINT = 0x86,

	SYM_MYID = 0x90,
	SYM_TURN = 0x91,
	SYM_PLAYERS = 0x92,
	SYM_ACTIVE_PLAYERS = 0x93,
	SYM_SUPPLY = 0x94,
	SYM_RAW_PRICE = 0x95,
	SYM_DEMAND = 0x96,
	SYM_PRODUCTION_PRICE = 0x97,
	SYM_MONEY = 0x98,
	SYM_RAW = 0x99,
	SYM_PRODUCTION = 0x9A,
	SYM_FACTORIES = 0x9B,
	SYM_AUTO_FACTORIES = 0x9C,
	SYM_MANUFACTURED = 0x9D,
	SYM_RESULT_RAW_SOLD = 0x9E,
	SYM_RESULT_RAW_PRICE = 0x9F,
	SYM_RESULT_PROD_BOUGHT = 0xA0,
	SYM_RESULT_PROD_PRICE = 0xA1,

	SYM_MEMORY = 0xB0;
};

#endif
