#ifndef GAME_H
#define GAME_H

enum play_types{
	PLAY_USL = 0, /* Do nothing */
	PLAY_DLG = 1, /* Output response */
	PLAY_BRC = 2, /* Broadcasting mesage */
	PLAY_DLG_NOGR = 3 /* Output without greeting */
};

struct pl_data
{
	int fd;
	char *name;
	char *buf;
	int buf_ptr;
};

struct pl_arr
{
	/*int *fd;
	char **names;
	char **buf;
	int *buf_ptr;*/
	int size;
	int max_size;
	struct pl_data *d;
};

int play(int id, const char *cmd, char ***response);
void game_start(int max_players, struct pl_data *data);
void bye(char *name);

#endif
