#pragma once
#include "include.h"

//piece.h MOVES_BUFF_SIZE_MAX���X�V����
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

	/*�œK�����v�Z*/
	/*�߂�l�F struct Solution	�g�p��Amoves_data���f�[�^���ׂĂɑ΂��āAPiece_move_data����board����������� -> Piece_move_data�����������*/
	Solution find_optimal_solution(class Board initial_board, short spawn_x, short spawn_y, short spawn_r, int *expand_max);

	/*�Ֆʂ̕]���l���v�Z*/
	float get_evaluation_value(class Board *board);

	/*����~�m�̑���p�^�[����^���A�Ֆʂɐݒu���邱�Ƃɂ���V���v�Z*/
	float get_reward_value(class Board* board, class Piece_move_data *pmd);

	/*����\�ȃp�^�[�����쐬�istart_piece�̏ꏊ����ړ��\�Ȃ��́j
	����: �Ֆ�(���ׂĂ̏�����ꂽ����(next_piece, btb�Ȃ�))�A�X�|�[������x,y,r�A�z�[���h�g�p�̗L�����w��
	�߂�l: ����\�ȃp�^�[��(Piece_move_data)��vector�A�����̔Ֆʂ̏��(�ՖʁAbtb, combo�A���C���������Ȃ�)���܂�
			�K�v�ȃ~�m��񂪂Ȃ��ꍇ���vector��Ԃ�
	*/
	std::vector<class Piece_move_data*> find_peice_moves(class Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold);

	bool is_meaningful_move(class Board* board, class Piece piece);

	enum Rotation_type get_rotation_type(class Board* board, class Piece piece, enum Rotation rotation);



	short get_heights(class Board* board, short x);
	short get_holes(class Board* board, short heights[10]);
	short get_attk(class Board* board, class Piece_move_data* pmd, Attk_type attk_type);
};