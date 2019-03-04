#ifndef __GAME_H
#define __GAME_H

#include "array.hpp"

const int std_buf_size = 83;
class GameClass{
	char *my_nick;
	void send(const char *cmd);
	int recv_char();
	void skip_line();
	void recv_line(char *buf, int max_size = std_buf_size);
	void just_recv();
	int turn;
	int fd;
	int active_players_num, players_num;
	void recv_to_amp(char *buf = 0, int size = std_buf_size);
	void recv_to_amp_ch();

	Array<char*> players; // array of nicknames, index == id
	int GetId(const char *nick);
	struct res_s{
		struct _{
			int qty, price;
		} raw, prod;
		res_s();
	};
	Array<res_s> results;
	void GetResults();
	struct info_s{
		int raw, prod, money, plants, auto_plants;
	};
	Array<info_s> info, prev_info;
	void GetInfo();
	struct market_s{
		struct _{
			int qty, price;
		} raw, prod;
	} market;
	void GetMarket();


public:
	GameClass();
	void Open(const char *ip, int port);
	void Start(const char *nick);
	void Close();

	void EndTurn();
	void Build(int qty);
	void Prod(int qty);
	void Buy(int qty, int price);
	void Sell(int qty, int price);
	
	int MyId();
	int Turn();
	int Players();
	int ActivePlayers();
	int Supply();
	int RawPrice();
	int Demand();
	int ProductionPrice();
	int Money(int player_id);
	int Raw(int player_id);
	int Production(int player_id);
	int Factories(int player_id);
	int AutoFactories(int player_id);
	int Manufactured(int player_id);
	int ResultRawSold(int player_id);
	int ResultRawPrice(int player_id);
	int ResultProdBought(int player_id);
	int ResultProdPrice(int player_id);
};

#endif
