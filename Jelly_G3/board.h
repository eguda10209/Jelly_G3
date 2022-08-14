#pragma once
#include "include.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 40

#define BOARD_EMPTY 0b0000000000
#define BOARD_FILL 0b1111111111

class Board {
private:

public:
	unsigned short board[BOARD_HEIGHT];

	Board(){
		for (int y = 0; y < BOARD_HEIGHT; y++) board[y] = BOARD_EMPTY;
	}

	void output_board();
	void set_board(unsigned short board[BOARD_HEIGHT]);
	void reset_board();
	short clear_line();
	void place_piece(class Piece piece);
	bool can_place(class Piece piece, short dx, short dy);
	struct SRS_data can_rotate(class Piece piece, enum Rotation rotation);
};