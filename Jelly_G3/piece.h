#pragma once
#include "include.h"

#define PIECE_TYPE_S 0
#define PIECE_TYPE_Z 1
#define PIECE_TYPE_J 2
#define PIECE_TYPE_L 3
#define PIECE_TYPE_T 4
#define PIECE_TYPE_O 5
#define PIECE_TYPE_I 6


#define PIECE_SPAWN_X 3
#define PIECE_SPAWN_Y 20

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

	/*置けるか置けないかにかかわらず移動*/
	void move_left();
	void move_right();
	void move_down();
	void rotate_left();
	void rotate_right();

	/*置ける場合は移動*/
	bool move(class Board *board, short dx, short dy);
	bool rotate(class Board *board, Rotation rotation);	//tspin情報を変えすようにする
	
	/*設置 ret:ライン消去数*/
	short place(class Board* board);

	/*落下(一番下まで落下)*/
	void fall(class Board* board);
};

class Piece_move_data {
private:
public:
	//to_x, to_r:ハードドロップ、ソフトドロップ前までに移動する場所 
	short to_x;
	short to_r;
	//last_x, last_y:最終的なy座標
	short last_x;
	short last_y;
	short last_r;
	std::vector<char> moves;
	short clear_lines;
	Rotation_type rot_type;

	Piece_move_data() {
		clear_lines = 0;
		rot_type = Normal;
	}
	//to_x to_r
	//操作(HN)[H:Hold, N:通常操作 D:ハードドロップ, S:ソフトドロップ ...]
	//ライン消去数　Tスピンデータ
};