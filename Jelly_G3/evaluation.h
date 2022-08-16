#pragma once
#include "include.h"


#define MOVE_HOLD 'H'
#define MOVE_HARDDROP 'Q'
#define MOVE_SOFTDROP 'S'
#define MOVE_MOVE_LEFT 'L'
#define MOVE_MOVE_RIGHT 'R'
#define MOVE_MOVE_DOWN 'D'
#define MOVE_ROTATE_LEFT 'M'
#define MOVE_ROTATE_RIGHT 'K'

//piece.h MOVES_BUFF_SIZE_MAXも更新する
#define MOVES_SIZE_MAX 5
#define MOVES_BUFF_MAX 100

#define UPPER_MOVE_Y_TH 16

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
	std::vector<class Piece_move_data> operation_data;
	//最終的な評価値
};


class Evaluation {
private:
public:
	std::random_device rnd;

	/*最適解を計算*/
	Solution find_optimal_solution(class Board initial_board, short spawn_x, short spawn_y, short spawn_r);

	/*盤面の評価値を計算*/
	float get_evaluation_value(class Board *board);

	/*あるミノの操作パターンを与え、盤面に設置することによる報酬を計算*/
	float get_reward_value(class Piece_move_data *pmd);

	/*操作可能なパターンを作成（start_pieceの場所から移動可能なもの）
	引数: 盤面(すべての情報を入れたもの(next_piece, btbなど))、スポーン時のx,y,r、ホールド使用の有無を指定
	戻り値: 操作可能なパターン(Piece_move_data)のvector、操作後の盤面の情報(盤面、btb, combo、ライン消去数など)も含む
			必要なミノ情報がない場合空のvectorを返す
	*/
	std::vector<class Piece_move_data*> find_peice_moves(class Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold);

	bool is_meaningful_move(class Board* board, class Piece piece);

	enum Rotation_type get_rotation_type(class Board* board, class Piece piece, enum Rotation rotation);

};