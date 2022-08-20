#pragma once
#include "include.h"

typedef enum PPT_Status {
	InMenu_Main,
	InMenu_Select_Character,
	InGame_Playing,
	InGame_Pauseing
};

class Bot {
private:
	int before_score;
	int before_frame;
	bool is_opening;
public:
	short player_num;
	short nexts;
	void run_bot();
	class Board *board;
	class Evaluation *evaluation;
	struct Solution *next_opt_sol;
	bool is_find_next_opt_sol;
	bool executed_free_opt_sol;

	PPT_Status get_ppt_status();
	static void move_thread_func(Piece_move_data pmd, int player_num);

	Bot(int n);
	
	~Bot();
};