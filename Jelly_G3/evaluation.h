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

//piece.h MOVES_BUFF_SIZE_MAX���X�V����
#define MOVES_SIZE_MAX 5
#define MOVES_BUFF_MAX 100

#define UPPER_MOVE_Y_TH 16

//MCTS�m�[�h
typedef struct MCT_node {
	float eval_value = 0;
	float reward_value = 0;
	class Piece_move_data *pmd = NULL;
	MCT_node* parent_node = NULL;
	//std::vector<MCT_node*> children_node;
	MCT_node* children_node[MOVES_BUFF_MAX * 2];
	int children_size = 0;
}MCT_node;
//�]���l�A��V�A�ՖʁA�~�m�f�[�^�APiece_move_data �e,�q�m�[�h�|�C���^�i4 + 4 + 80 + 1 + ? + 8 bytes?�j


typedef struct Solution{
	std::vector<class Piece_move_data> operation_data;
	//�ŏI�I�ȕ]���l
};


class Evaluation {
private:
public:
	std::random_device rnd;

	/*�œK�����v�Z*/
	Solution find_optimal_solution(class Board initial_board, short spawn_x, short spawn_y, short spawn_r);

	/*�Ֆʂ̕]���l���v�Z*/
	float get_evaluation_value(class Board *board);

	/*����~�m�̑���p�^�[����^���A�Ֆʂɐݒu���邱�Ƃɂ���V���v�Z*/
	float get_reward_value(class Piece_move_data *pmd);

	/*����\�ȃp�^�[�����쐬�istart_piece�̏ꏊ����ړ��\�Ȃ��́j
	����: �Ֆ�(���ׂĂ̏�����ꂽ����(next_piece, btb�Ȃ�))�A�X�|�[������x,y,r�A�z�[���h�g�p�̗L�����w��
	�߂�l: ����\�ȃp�^�[��(Piece_move_data)��vector�A�����̔Ֆʂ̏��(�ՖʁAbtb, combo�A���C���������Ȃ�)���܂�
			�K�v�ȃ~�m��񂪂Ȃ��ꍇ���vector��Ԃ�
	*/
	std::vector<class Piece_move_data*> find_peice_moves(class Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold);

	bool is_meaningful_move(class Board* board, class Piece piece);

	enum Rotation_type get_rotation_type(class Board* board, class Piece piece, enum Rotation rotation);

};