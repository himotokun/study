#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "strlib.h"

#include "game.h"

/* Structures and enums */

enum{ buffer_size = 128 };

enum prices
{
	PRC_PROD = 2000,
	PRC_BUILD = 2500,
	MON_RAW = 300,
	MON_PROD = 500,
	MON_FACT = 100
};

enum turn_st
{
	TRN_OK = 0,
	TRN_NOMONEY = 1,
	TRN_TOOCHP = 2,
	TRN_TOOEXP = 3,
	TRN_NORES = 4,
	TRN_TOOMANY = 5,
	TRN_NOFACT = 6
};

struct auct_lst
{
	int id, prc, qty;
	struct auct_lst *next;
};

struct lot_s
{
	int id, req, lot;
};

struct turn_s
{
	int s_prc,
		s_qty,
		b_prc,
		b_qty,
		prod_qty,
		build_qty,
		done;
};

struct player
{
	int raw;
	int prod;
	int fact;
	long money;

	int *fact_st;
	int build_fact;

	struct turn_s *turn;

	int fd;
	char *name;
};

struct game_s
{
	int buy_prc,
		sell_prc,
		buy_qty,
		sell_qty,
		lvl,
		pl_num,
		left;

	struct player **pl;
} *game;

/* Initializations */

static void set_lvl(struct game_s *g)
{
	g->sell_prc = 7000 - 500*g->lvl;
	g->sell_qty = (4-(g->lvl+1)/2)*g->pl_num;
	g->buy_qty = ((g->lvl+1)/2)*g->pl_num;
	switch(g->lvl){
		case 1:
			g->buy_prc = 800;
			break;
		case 2:
			g->buy_prc = 650;
			break;
		case 3:
			g->buy_prc = 500;
			break;
		case 4:
			g->buy_prc = 400;
			break;
		case 5:
			g->buy_prc = 300;
	}
}

static struct turn_s* init_turn(struct turn_s *trn)
{
	struct turn_s *res = trn?trn:malloc(sizeof(*res));
	res->s_prc = 0;
	res->s_qty = 0;
	res->b_prc = 0;
	res->b_qty = 0;
	res->prod_qty = 0;
	res->build_qty = 0;
	res->done = 0;
	return res;
}

static struct player* init_pls()
{
	struct player *res = malloc(sizeof(*res));
	int i;
	res->raw = 4;
	res->fact = 2;
	res->prod = 2;
	res->build_fact = 0;
	res->money = 10000;
	res->fact_st = malloc(res->fact*sizeof(int));
	res->turn = init_turn(NULL);
	for(i = 0; i < res->fact; i++)
		res->fact_st[i] = 0;
	return res;
}

static struct player** init_plarr(int size, struct pl_data *data)
{
	struct player **res = malloc(size*sizeof(*res));
	int i;
	for(i = 0; i < size; i++){
		res[i] = init_pls();
		res[i]->fd = data[i].fd;
		res[i]->name = str_unconst(data[i].name);
	}
	return res;
}

static struct game_s* init_game(int max_players, struct pl_data *data)
{
	struct game_s *g = malloc(sizeof(*g));
	g->pl = init_plarr(max_players, data);
	g->lvl = 1;
	g->pl_num = max_players;
	g->left = g->pl_num;
	set_lvl(g);
	return g;
}

/* Auction functions */

static int auct_smax(struct game_s *g)
{
	int res = 0, i, flag = 0;
	for(i = 0; i < g->pl_num; i++)
		if((g->pl[i]->turn->s_prc > g->pl[res]->turn->s_prc || flag == 0) &&
				g->pl[i]->turn->s_qty > 0){
			res = i;
			flag = 1;
		}
	if(!flag)
		return -1;
	return res;
}

static int auct_bmin(struct game_s *g)
{
	int res = 0, i, flag = 0;
	for(i = 0; i < g->pl_num; i++)
		if((g->pl[i]->turn->b_prc < g->pl[res]->turn->b_prc || flag == 0) &&
				g->pl[i]->turn->b_qty > 0){
			res = i;
			flag = 1;
		}
	if(!flag)
		return -1;
	return res;
}

static struct auct_lst* add_alst(struct auct_lst *lst,
		int id, int prc, int qty)
{
	struct auct_lst *tmp = malloc(sizeof(*tmp));
	tmp->id = id;
	tmp->prc = prc;
	tmp->qty = qty;
	tmp->next = lst;
	return tmp;
}

static struct auct_lst* lst_s_asc(struct game_s *g)
{
	struct auct_lst *res = NULL;
	int cur_id;
	while( (cur_id = auct_smax(g)) != -1){
		res = add_alst(res, cur_id,
				g->pl[cur_id]->turn->s_prc, g->pl[cur_id]->turn->s_qty);
		g->pl[cur_id]->turn->s_qty = 0;
	}
	return res;
}

static struct auct_lst* lst_b_desc(struct game_s *g)
{
	struct auct_lst *res = NULL;
	int cur_id;
	while( (cur_id = auct_bmin(g)) != -1){
		res = add_alst(res, cur_id,
				g->pl[cur_id]->turn->b_prc, g->pl[cur_id]->turn->b_qty);
		g->pl[cur_id]->turn->b_qty = 0;
	}
	return res;
}

/* Gaming functions */

static int build(struct player *p, int qty)
{
	if(p->money >= PRC_BUILD*qty){
		p->turn->build_qty += qty;
		p->money -= PRC_BUILD*qty;
		return 0;
	}else{
		return TRN_NOMONEY;
	}
}

static int prod(struct player *p, int qty)
{
	if(qty > p->fact - p->build_fact)
	   return TRN_NOFACT;
	else if( qty > p->raw){
		return TRN_NORES;
	}else if( qty*PRC_PROD > p->money){
		return TRN_NOMONEY;
	}else{
		p->turn->prod_qty = qty;
		return 0;
	}
}

static int buy(struct game_s *g, int id, int prc, int qty)
{
	if(prc*qty > g->pl[id]->money)
		return TRN_NOMONEY;
	else if(prc < g->buy_prc)
		return TRN_TOOCHP;
	else if(qty > g->buy_qty)
		return TRN_TOOMANY;
	else{
		g->pl[id]->turn->b_prc  = prc;
		g->pl[id]->turn->b_qty = qty;
		return 0;
	}
}

static int sell(struct game_s *g, int id, int prc, int qty)
{
	if(qty > g->pl[id]->prod)
		return TRN_NORES;
	else if(prc > g->sell_prc)
		return TRN_TOOEXP;
	else if(qty > g->sell_qty)
		return TRN_TOOMANY;
	else{
		g->pl[id]->turn->s_prc = prc;
		g->pl[id]->turn->s_qty = qty;
		return 0;
	}
}

/* Same price processing */

static int same_cnt(struct auct_lst *pos)
{
	int cnt = 0, price = pos->prc;
	while(pos && pos->prc == price){
		cnt++;
		pos = pos->next;
	}
	return cnt;
}

static int same_sum(struct auct_lst *pos)
{
	int sum = 0, price = pos->prc;
	while(pos && pos->prc == price){
		sum += pos->qty;
		pos = pos->next;
	}
	return sum;
}

struct lot_s* same_arr(struct auct_lst *pos)
{
	int cnt = same_cnt(pos), i;
	struct lot_s *lots = malloc(cnt*sizeof(*lots));
	for(i = 0; i < cnt; i++){
		lots[i].id = pos->id;
		lots[i].lot = 0;
		lots[i].req = pos->qty;
		pos = pos->next;
	}
	return lots;
}

struct auct_lst* next_prc(struct auct_lst *lst)
{
	int price = lst->prc;
	while(lst && lst->prc == price){
		struct auct_lst *tmp = lst;
		lst = lst->next;
		free(tmp);
	}
	return lst;
}

int rand_in(int a, int b)
{
	int r =  a + /*(int)((double)b*rand()/(RAND_MAX+1.0));*/ rand()%(b-a+1);
	return r;
}

/* Most important */

static void auct_buy(struct game_s *g)
{
	struct auct_lst *lst = lst_b_desc(g), *tmp;
	int left = game->buy_qty;
	int i;
	tmp = lst;
	while(tmp){
		tmp = tmp->next;
	}
	while(lst && left){
		struct lot_s *lots = same_arr(lst);
		int sum = same_sum(lst);
		int cnt = same_cnt(lst);
		if(sum > left)
			sum = left;
		while(left--,sum--){
			int r;
			while(r = rand_in(0,cnt-1), lots[r].lot >= lots[r].req){
			}
			lots[r].lot++;
		}
		left++;
		for(i = 0; i < cnt; i++)
			g->pl[lots[i].id]->turn->b_qty = lots[i].lot;
		lst = next_prc(lst);
	}
	for(i = 0; i < g->pl_num; i++){
		g->pl[i]->money -=
			g->pl[i]->turn->b_prc * g->pl[i]->turn->b_qty;
		g->pl[i]->raw += g->pl[i]->turn->b_qty;
	}
}

static void auct_sell(struct game_s *g)
{
	struct auct_lst *lst = lst_s_asc(g);
	int left = game->sell_qty;
	int i;
	while(lst && left){
		struct lot_s *lots = same_arr(lst);
		int sum = same_sum(lst);
		int cnt = same_cnt(lst);
		if(sum > left)
			sum = left;
		while(left--,sum--){
			int r;
			while(r = rand_in(0,cnt-1), lots[r].lot >= lots[r].req){
			}
			lots[r].lot++;
		}
		if(left<0)
			left++;
		for(i = 0; i < cnt; i++)
			g->pl[lots[i].id]->turn->s_qty = lots[i].lot;
		lst = next_prc(lst);
	}
	for(i = 0; i < g->pl_num; i++){
		g->pl[i]->money +=
			g->pl[i]->turn->s_prc * g->pl[i]->turn->s_qty;
		g->pl[i]->prod -= g->pl[i]->turn->s_qty;
	}
}

static void do_build(struct player *p)
{
	int i;
	for(i = 0; i < p->fact; i++)
	for(i = 0; i < p->fact; i++){
		switch(p->fact_st[i]){
			case 0:
				break;
			case 1:
				p->money -= PRC_BUILD;
				p->build_fact--;
			default:
				p->fact_st[i]--;
		}
	}
	p->fact_st = realloc(p->fact_st,
			(p->fact + p->turn->build_qty)*sizeof(int));
	for(i = p->fact; i < p->fact + p->turn->build_qty;
			i++){
		p->fact_st[i] = 5;
	}
	p->build_fact += p->turn->build_qty;
	p->fact += p->turn->build_qty;
}

static void do_prod(struct player *p)
{
	p->money -= PRC_PROD*p->turn->prod_qty;
	p->prod += p->turn->prod_qty;
	p->raw -= p->turn->prod_qty;
}

static void m_cst(struct player *p)
{
	p->money -=
		p->raw * MON_RAW +
		p->prod * MON_PROD +
		p->fact * MON_FACT;
}

static int new_lvl(int lvl)
{
	const int table[5][5] = {
		{ 4, 4, 2, 1, 1 },
		{ 3, 4, 3, 1, 1 },
		{ 1, 3, 4, 3, 1 },
		{ 1, 1, 3, 4, 3 },
		{ 1, 1, 2, 4, 4 }
	};
	int r = rand_in(1,12);
	int res, sum;
	for(res = 0, sum = 0; sum < r; sum+=table[lvl-1][res++]){
	}
	return res;
}

static char** argv_single(const char *str)
{
	char **res = malloc(2*sizeof(char*));
	res[0] = str_unconst(str);
	res[1] = NULL;
	return res;
}

static char** turn_res(struct game_s *g)
{
	char buf[buffer_size], **res;
	int i;
	res = malloc((2*g->pl_num+3)*sizeof(char*));
	res[0] = str_unconst("\n***** Round results *****\n");
	for(i = 0; i < g->pl_num; i++){
		sprintf(buf, "%s bought %d for %d\n", g->pl[i]->name,
				g->pl[i]->turn->b_qty, g->pl[i]->turn->b_prc);
		res[2*i+1] = str_unconst(buf);
		sprintf(buf, "... and sold %d for %d\n",
				g->pl[i]->turn->s_qty, g->pl[i]->turn->s_prc);
		res[2*i+2] = str_unconst(buf);
	}
	sprintf(buf, "New level: %d\n\n> ", g->lvl);
	res[2*g->pl_num+1] = str_unconst(buf);
	res[2*g->pl_num+2] = NULL;
	return res;
}

void bye(char *name)
{
	int i, id;
	struct game_s *g = game;
	for(id = 0; id < g->pl_num; id++)
		if(!str_cmp(g->pl[id]->name, name) ){
			shutdown(g->pl[id]->fd,0);
			for(i = id; i < g->pl_num-1 ; i++)
				g->pl[i] = g->pl[i+1];
			g->pl_num--;
			g->left--;
			return;
		}
}

static char** turn(struct game_s *g)
{
	int i;
	char **res;
	for(i = 0; i < g->pl_num; i++){
		do_build(g->pl[i]);
		do_prod(g->pl[i]);
		m_cst(g->pl[i]);
		if(g->pl[i]->money < 0)
			bye(g->pl[i]->name);
	}
	auct_buy(g);
	auct_sell(g);
	g->lvl = new_lvl(g->lvl);
	set_lvl(g);
	g->left = g->pl_num;
	res = turn_res(g);
	for(i = 0; i< g->pl_num; i++)
		g->pl[i]->turn = init_turn(g->pl[i]->turn);
	printf("got\n");
	return res;
}

static char** get_market(struct game_s *g)
{
	char buf[buffer_size], **res;
	res = malloc(7*sizeof(char*));
	sprintf(buf,"Active players: %d\n", g->pl_num);
	res[0] = str_unconst(buf);
	sprintf(buf, "Market status (level %d): \n", g->lvl);
	res[1] = str_unconst(buf);
	res[2] = str_unconst("Selling:\n");
	sprintf(buf, "\tmax price: %d, max qty: %d\n", g->sell_prc, g->sell_qty);
	res[3] = str_unconst(buf);
	res[4] = str_unconst("Buying:\n");
	sprintf(buf, "\tmin price: %d, max qty: %d\n", g->buy_prc, g->buy_qty);
	res[5] = str_unconst(buf);
	res[6] = NULL;
	return res;
}

static char** get_info(struct game_s *g, char *name)
{
	char buf[buffer_size], **res;
	int id;
	for(id = 0; id < g->pl_num; id++)
		if(!str_cmp(g->pl[id]->name, name))
			break;
	if(id == g->pl_num)
		return argv_single("Error: no such player\n");
	res = malloc(4*sizeof(char*));
	sprintf(buf,"Player's name: %s\n", g->pl[id]->name);
	res[0] = str_unconst(buf);
	res[1] = str_unconst("Player's resources: \n");
	sprintf(buf, "\t%ld$, %d active and %d building factories,\n\t"
			"%d raw material and %d products\n",
			g->pl[id]->money, g->pl[id]->fact - g->pl[id]->build_fact,
			g->pl[id]->build_fact, g->pl[id]->raw, g->pl[id]->prod);
	res[2] = str_unconst(buf);
	res[3] = NULL;
	return res;
}

static char** get_help()
{
	char **res;
	res = malloc(9*sizeof(char*));
	res[0] = str_unconst("\nAvailable commands:\n");
	res[1] = str_unconst("\tinfo - info about market and your resources\n");
	res[2] = str_unconst("\tinfo <NAME> - same but shows NAME's info\n");
	res[3] = str_unconst("\tbuild <N> - build N new factories\n");
	res[4] = str_unconst("\tprod <N> - produce N units of product\n");
	res[5] = str_unconst("\tbuy <P> <Q> - buy Q units of raw material"
			" for P dollars\n");
	res[6] = str_unconst("\tsell <P> <Q> - sell Q products for P dollars\n");
	res[7] = str_unconst("\tend - end your turn\n");
	res[8] = str_unconst("\thelp - get this help\n\n");
	res[9] = NULL;
	return res;
}

/* Executing commands */

static char** try_build(int id, const char *cmd)
{
		int qty;
		sscanf(cmd, "build %d", &qty);
		switch( build(game->pl[id], qty) ){
			case TRN_OK:
				return argv_single("Request accepted\n");
			case TRN_NOMONEY:
				return argv_single("Error: not enough money\n");
			default:
				return argv_single("Unknown error\n");
		}
}

static char** try_prod(int id, const char *cmd)
{
		int qty;
		sscanf(cmd, "prod %d", &qty);
		switch( prod(game->pl[id], qty) ){
			case TRN_OK:
				return argv_single("Request accepted\n");
			case TRN_NOFACT:
				return argv_single("Error: not enough active factories\n");
			case TRN_NORES:
				return argv_single("Error: not enough resources\n");
			case TRN_NOMONEY:
				return argv_single("Error: not enough money\n");
			default:
				return argv_single("Unknown error\n");
		}
}

static char** try_buy(int id, const char *cmd)
{
		int prc, qty;
		if( sscanf(cmd, "buy %d %d", &prc, &qty) != 2 )
			return argv_single("Wrong command!\nType \"help\" to see more\n");
		switch( buy(game, id, prc, qty) ){
			case TRN_OK:
				return argv_single("Request accepted\n");
			case TRN_NOMONEY:
				return argv_single("Error: not enough money\n");
			case TRN_TOOCHP:
				return argv_single("Error: too low price\n");
			case TRN_TOOMANY:
				return argv_single("Error: too big qty\n");
			default:
				return argv_single("Unknown error\n");
		}
}

static char** try_sell(int id, const char *cmd)
{
		int prc, qty;
		if( sscanf(cmd, "sell %d %d", &prc, &qty) != 2 )
			return argv_single("Wrong command!\nType \"help\" to see more\n");
		switch( sell(game, id, prc, qty) ){
			case TRN_OK:
				return argv_single("Request accepted\n");
			case TRN_TOOEXP:
				return argv_single("Error: too high price\n");
			case TRN_TOOMANY:
				return argv_single("Error: too big qty\n");
			case TRN_NORES:
				return argv_single("Error: not enough products\n");
			default:
				return argv_single("Unknown error\n");
		}
}

/* Interfaces */

int play(int id, const char *cmd, char ***response)
{
	struct game_s *g = game;
	printf("<%s: %s>\n", g->pl[id]->name, cmd);
	if(!*cmd){
		*response = NULL;
		return PLAY_DLG;
	}
	if(g->pl[id]->turn->done){
		*response = argv_single("Turn already finished\n");
		return PLAY_DLG_NOGR;
	}else if(str_beg(cmd,"market")){
		*response = get_market(g);
	}else if(str_beg(cmd,"player")){
		char buf[buffer_size];
		int res;
		res = sscanf(cmd, "player %11s", buf);
		*response = get_info(g, res>0?buf:g->pl[id]->name);
	}else if(!str_cmp(cmd,"end")){
		g->pl[id]->turn->done = 1;
		g->left--;
		*response = argv_single("Turn finished\n");
	}else if(str_beg(cmd,"build")){
		*response = try_build(id, cmd);
	}else if(str_beg(cmd,"prod")){
		*response = try_prod(id, cmd);
	}else if(str_beg(cmd,"buy")){
		*response = try_buy(id, cmd);
	}else if(str_beg(cmd,"sell")){
		*response = try_sell(id, cmd);
	}else if(str_beg(cmd,"help")){
		*response = get_help();
	}else{
		*response = argv_single("Wrong command!\nType \"help\" to see more\n");
	}
	if(!g->left){
		*response = turn(g);
		return PLAY_BRC;
	}
	return PLAY_DLG;
}

void game_start(int max_players, struct pl_data *data)
{
	game = init_game(max_players, data);
}

/*int admin(const char *cmd, char ***response)
{
	return 0;
}*/
