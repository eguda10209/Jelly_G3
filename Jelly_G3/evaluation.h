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

#define MOVES_SIZE_MAX 5
#define MOVES_BUFF_MAX 300

//MCTS�m�[�h
typedef struct MCT_node {

};
//�]���l�A��V�A�ՖʁA�~�m�f�[�^�APiece_move_data �e�m�[�h�|�C���^�i4 + 4 + 80 + 1 + ? + 8 bytes?�j

//MCT
typedef struct MCT {

};
//�q�m�[�h�|�C���^�z��

typedef struct Solution{
	std::vector<class Piece_move_data> operation_data;
	//�ŏI�I�ȕ]���l
};


class Evaluation {
private:
public:
	/*�œK�����v�Z*/
	Solution find_optimal_solution(short next_num, std::vector<char> pieces);

	/*�Ֆʂ̕]���l���v�Z*/
	int get_evaluation_value(class Board *board);

	/*����~�m�̑���p�^�[����^���A�Ֆʂɐݒu���邱�Ƃɂ���V���v�Z*/
	int get_reward_value(class Board *board, class Piece_move_data pmd);

	/*����\�ȃp�^�[�����쐬�istart_piece�̏ꏊ����ړ��\�Ȃ��́j*/
	std::vector<class Piece_move_data> find_peice_moves(class Board *board, class Piece start_piece, bool is_upper_move);

	bool is_meaningful_move(class Board* board, class Piece piece);

	enum Rotation_type get_rotation_type(class Board* board, class Piece piece, enum Rotation rotation);

};