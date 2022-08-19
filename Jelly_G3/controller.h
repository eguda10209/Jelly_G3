#pragma once
#include "include.h"

#define MOVE_NONE 'N'
#define MOVE_HOLD 'H'
#define MOVE_HARDDROP 'Q'
#define MOVE_SOFTDROP 'S'
#define MOVE_MOVE_LEFT 'L'
#define MOVE_MOVE_RIGHT 'R'
#define MOVE_MOVE_DOWN 'D'
#define MOVE_MOVE_UP 'U'
#define MOVE_ROTATE_LEFT 'M'
#define MOVE_ROTATE_RIGHT 'K'

class Controller {
private:
public:
	static int move(class Piece_move_data pmd, short player_num);
	static int input(char input1, char input2, short player_num);
	static int wait_frame(short frames);
};