#pragma once
#include "include.h"

//piece.h MOVES_BUFF_SIZE_MAXも更新する
#define MOVES_SIZE_MAX 5
#define MOVES_BUFF_MAX 100

#define UPPER_MOVE_Y_TH 16

typedef enum Attk_type {
	None,
	LineClear1,
	LineClear2,
	LineClear3,
	Tetris,
	TSpinMini_,
	TSpin1,
	TSpin2,
	TSpin3,
	PerfectClear
};

//MCTSノード
typedef struct MCT_node {
	float eval_value = 0;
	float reward_value = 0;
	class Piece_move_data *pmd = NULL;
	MCT_node* parent_node = NULL;
	//std::vector<MCT_node*> children_node;
	MCT_node* children_node[MOVES_BUFF_MAX * 2];
	int children_size = 0;
}MCT_node;
//評価値、報酬、盤面、ミノデータ、Piece_move_data 親,子ノードポインタ（4 + 4 + 80 + 1 + ? + 8 bytes?）


typedef struct Solution{
	float score = 0.0;
	int depth = 0;
	int nodes = 0;
	class Piece_move_data* moves_data[10];
	int moves_data_size = 0;
	//std::vector<class Piece_move_data*> moves_data;
};


class Evaluation {
private:
public:
	std::random_device rnd;

	/*最適解を計算*/
	/*戻り値： struct Solution	使用後、moves_data内データすべてに対して、Piece_move_data内のboardをメモリ解放 -> Piece_move_dataをメモリ解放*/
	Solution find_optimal_solution(class Board initial_board, short spawn_x, short spawn_y, short spawn_r, int *expand_max);

	/*盤面の評価値を計算*/
	float get_evaluation_value(class Board *board);

	/*あるミノの操作パターンを与え、盤面に設置することによる報酬を計算*/
	float get_reward_value(class Board* board, class Piece_move_data *pmd);

	/*操作可能なパターンを作成（start_pieceの場所から移動可能なもの）
	引数: 盤面(すべての情報を入れたもの(next_piece, btbなど))、スポーン時のx,y,r、ホールド使用の有無を指定
	戻り値: 操作可能なパターン(Piece_move_data)のvector、操作後の盤面の情報(盤面、btb, combo、ライン消去数など)も含む
			必要なミノ情報がない場合空のvectorを返す
	*/
	std::vector<class Piece_move_data*> find_peice_moves(class Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold);

	bool is_meaningful_move(class Board* board, class Piece piece);

	enum Rotation_type get_rotation_type(class Board* board, class Piece piece, enum Rotation rotation);



	short get_heights(class Board* board, short x);
	short get_holes(class Board* board, short heights[10]);
	short get_attk(class Board* board, class Piece_move_data* pmd, Attk_type attk_type);
};