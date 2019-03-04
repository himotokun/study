#ifndef DEFS_H
#define DEFS_H

/* Data structures */

struct c_lst{
	char **argv;
	char flags;
	int pid;
	char **io_path;

	struct c_lst *next;
};

/* Const enums */

enum cmd_flags{
	C_BG = 1,
	C_AND = 2,
	C_OR = 4,
	C_CONV = 6,
	C_IN = 8,
	C_OUT = 16,
	C_APP = 32,
	C_ERR = -128,

	C_IO = C_IN|C_OUT|C_APP,
	C_SEP = C_BG|C_AND|C_OR|C_CONV,
	C_NOTS = C_AND|C_OR
};

enum cmd_errs{
	CERR_NOIO = -1,
	CERR_SEVCTRL = -2,
	CERR_ENDCTRL = -3,
	CERR_SEVIO = -4,
	CERR_CONVIO = -5
};

#endif
