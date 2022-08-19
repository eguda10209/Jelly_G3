#pragma once
#include "include.h"

#define PIECE_TYPE_NONE -1
#define PIECE_TYPE_S 0
#define PIECE_TYPE_Z 1
#define PIECE_TYPE_J 2
#define PIECE_TYPE_L 3
#define PIECE_TYPE_T 4
#define PIECE_TYPE_O 5
#define PIECE_TYPE_I 6


#define PIECE_SPAWN_X 3
#define PIECE_SPAWN_Y 20
#define PIECE_SPAWN_R 0

#define MOVES_BUFF_SIZE_MAX 8

typedef enum Rotation {
	Rotate_null,
	Rotate_right,
	Rotate_left
};

typedef enum Rotation_type {
	Normal,
	TSpinmini,
	TSpin
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
	
	/*�ݒu ret:���C��������*/
	short place(class Board* board);

	/*����(��ԉ��܂ŗ���)*/
	void fall(class Board* board);
};

class Piece_move_data {
private:
public:
	//to_x, to_r:�n�[�h�h���b�v�A�\�t�g�h���b�v�O�܂łɈړ�����ꏊ 
	short to_x;
	short to_r;
	//last_x, last_y, last_r:�ŏI�I�ɐݒu����x,y,r
	short last_x;
	short last_y;
	short last_r;
	//�ړ����@
	bool use_hold;
	char moves[MOVES_BUFF_SIZE_MAX];
	int moves_size;
	//�������郉�C����
	short clear_lines;
	//��]�̎��(���Tspin�ł��邩�̏��)
	Rotation_type rot_type;
	//�ݒu��̔Ֆ�
	Board *board;

	Piece_move_data() {
		clear_lines = 0;
		rot_type = Normal;
		moves_size = 0;
	}
};