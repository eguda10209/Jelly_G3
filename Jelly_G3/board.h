#pragma once
#include "include.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 40

#define BOARD_EMPTY 0b0000000000
#define BOARD_FILL 0b1111111111

#define NEXT_MAX 8

class Board {
private:

public:
	unsigned short board[BOARD_HEIGHT];
	short max_height;
	short combo;
	bool btb;
	char hold_piece;
	//std::vector<char> next_piece{ 12 };　//->copy_board()でメモリアクセスエラーが起きる
	char next_piece[NEXT_MAX];

	Board(int null){
		for (int y = 0; y < BOARD_HEIGHT; y++) board[y] = BOARD_EMPTY;
		for (int i = 0; i < NEXT_MAX; i++) next_piece[i] = -1;
		max_height = 0;
		combo = -1;
		btb = false;
		hold_piece = -1;
	}

	void output_board();
	void set_board(unsigned short board[BOARD_HEIGHT]);
	void reset_board();
	Board* copy_board();
	short clear_line();
	short get_clear_lines();
	void place_piece(class Piece piece);
	bool can_place(class Piece piece, short dx, short dy);
	struct SRS_data can_rotate(class Piece piece, enum Rotation rotation);

	/*pieceからdx, dy移動、回転後、のライン消去数を返す ret: -1:設定不可能 0 ~ 4:ライン消去数*/
	int try_place(class Piece *piece, short dx, short dy, enum Rotation rotation, bool is_fall);

	/*combo btb pieces max_heightの更新*/
	void updata_board_status(short clear_lines, enum Rotation_type rot_type, bool use_hold);
};