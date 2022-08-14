#pragma once
#include "include.h"

#define PIECE_TYPE_S 0
#define PIECE_TYPE_Z 1
#define PIECE_TYPE_J 2
#define PIECE_TYPE_L 3
#define PIECE_TYPE_T 4
#define PIECE_TYPE_O 5
#define PIECE_TYPE_I 6

typedef enum Rotation {
	Rotate_right,
	Rotate_left
};

typedef struct SRS_data {
	short index = -1;
	short dx = 0;
	short dy = 0;
	short to_r = 0;
};

class Piece {
private:
public:
	unsigned short type;
	short x;
	short y;
	short r;

	Piece(unsigned short type, short x, short y, short r) {
		this->type = type;
		this->x = x;
		this->y = y;
		this->r = r;
	}

	/*�u���邩�u���Ȃ����ɂ�����炸�ړ�*/
	void move_left();
	void move_right();
	void move_down();
	void rotate_left();
	void rotate_right();

	/*�u����ꍇ�͈ړ�*/
	bool move(class Board *board, short dx, short dy);
	bool rotate(class Board *board, Rotation rotation);	//tspin����ς����悤�ɂ���
	short place(class Board* board);	//���C����������Ԃ��B
};