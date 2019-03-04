#include "server.h"
#include "keylib.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	int max_players, port, listener;
	proc_keys(argv);
	max_players = get_int_key(argv,"--max-players");
	port = get_int_key(argv,"--port");
	if( (listener = lstn_init(port)) < 0 ){
		printf("*** Error starting server: code %d ***\n", listener);
		return 1;
	}

	serv_main(listener, max_players, port);

	return 0;
}
