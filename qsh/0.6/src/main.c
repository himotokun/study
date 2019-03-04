#include <stdio.h>

#include "parser.h"
#include "interface.h"
#include "defs.h"
#include "cmdlib.h"
#include "keylib.h"

int main(int argc, char **argv)
{
	struct c_lst *lst;
	proc_keys(argv);
	while(print_greet(), (lst = get_clst())){
		exec_clst2(lst);
		free_clst(lst);
	}
	printf("exit\n");
	return 0;
}
