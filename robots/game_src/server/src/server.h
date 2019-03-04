#ifndef SERVER_H
#define SERVER_H

enum sockerr{
	SOCKERR_OPEN = -1,
	SOCKERR_BIND = -2,
	SOCKERR_LSTN = -3
};

enum serverr{
	SERVERR_SLCT = -1,
	SERVERR_ACPT = -2
};

int lstn_init(int port);
int serv_main(int listener, int max_players, int port);

#endif
