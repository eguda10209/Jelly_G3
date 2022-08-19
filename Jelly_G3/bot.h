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

	PPT_Status get_ppt_status();

	Bot(int n);
	
	~Bot() {
		delete board;
		delete evaluation;
	}
};