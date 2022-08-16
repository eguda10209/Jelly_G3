#include "evaluation.h"

Solution Evaluation::find_optimal_solution(Board initial_board, short spawn_x, short spawn_y, short spawn_r) {


	std::srand(time(NULL));
	Solution opt_sol;
	//�W�J����m�[�h�̑I��
	//�W�J find_peice_moves(�W�J����m�[�h�̔ՖʁA�ݒu����~�m�A��i���샂�[�h��)
	// 	   �[��vector<Piece_move_data>�F�@�ړ��\�ȃ~�m���(�ړ����@�A)
	//�W�J�����m�[�h�ɑ΂��ĕ]��
	// 
	//�]���̋t�`��

	std::vector<MCT_node*> all_nodes;	//�S�m�[�h


	std::vector<Piece_move_data*> expanded_pmds;
	MCT_node root_nodes;
	MCT_node* opt_leaf_node = NULL;

	//�����m�[�h�̓W�J
	std::vector<Piece_move_data*> pmds;
	expanded_pmds = find_peice_moves(initial_board, spawn_x, spawn_y, spawn_r, false);
	pmds = find_peice_moves(initial_board, spawn_x, spawn_y, spawn_r, true);
	for (int i = 0; i < pmds.size(); i++) expanded_pmds.push_back(pmds[i]);

	//�����m�[�h�̕]�� todo ���S����ꍇ��E��0�ɂ���
	float best_E = 0.0;
	float E = 0.0;
	for (int i = 0; i < expanded_pmds.size(); i++) {
		MCT_node *node = (MCT_node*) malloc(sizeof(MCT_node));
		all_nodes.push_back(node);
		node->parent_node = &root_nodes;
		node->pmd = expanded_pmds[i];
		node->eval_value = get_evaluation_value(node->pmd->board);
		node->reward_value = get_reward_value(node->pmd);
		node->children_size = 0;

		root_nodes.children_node[root_nodes.children_size] = node;
		root_nodes.children_size++;
		E = node->eval_value + node->reward_value;
		if (E > best_E) {
			best_E = E; 
			opt_leaf_node = node;
		}
	}

	//�����m�[�h�]���̋t�`��
	root_nodes.eval_value = best_E;

	std::ofstream ofs("./ev_debug.txt");
	ofs << "start" << std::endl;

	int exp_cnt = 0;
	while (true) {

		//�W�J����m�[�h�̑I��
		MCT_node* select_node = &root_nodes;
		MCT_node* expand_node = NULL;
		while (true) {
			int children_size = select_node->children_size;
			if (children_size == 0) {
				expand_node = select_node;
				break;
			}
			std::vector<std::pair<float, int>> E_rank;
			int* p = (int*)malloc(sizeof(int) * children_size);

			//E(eval_value + reward_value)�̌v�Z
			for (int i = 0; i < children_size; i++) {
				E_rank.push_back({ select_node->children_node[i]->eval_value + select_node->children_node[i]->reward_value, i });
			}
			std::sort(E_rank.begin(), E_rank.end(), std::greater<std::pair<float, int>>());//�~���\�[�g

			//�I�������m���̌v�Z
			float E_min = E_rank[children_size - 1].first;
			int sum = 0.0;
			for (int i = 0; i < children_size; i++) {
				p[i] = (((float)((E_rank[i].first - E_min)) + 10.0) / (float)((i)+1.0)) * 100;
				sum += p[i];
			}

			//�I��
			int rnd_ = rnd() % (int)sum;
			int select_num = 0;
			int s_num = 0;
			for (int i = 0; i < children_size; i++) {
				if (s_num <= rnd_ && rnd_ < s_num + p[i]) {
					select_num = E_rank[i].second;
					break;
				}
				s_num += p[i];
			}

			MCT_node* np = (*select_node).children_node[select_num];
			select_node = np;

			free(p);
		}

		/*�W�J*/
		Board* b_tmp = (*(*expand_node).pmd).board->copy_board();
		std::vector<Piece_move_data*> n_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//expanded_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//��L�֐�������Aexpand_node->pmd�̃f�[�^�����������Ȃ�(�߂�l�����������H) �f�o�b�K�̖��H
		// 
		// Board *b_tmp = expand_node->pmd->board->copy_board();��expand_node���g�p����ƋN����H
		// 
		//�|�C���^��NULL�ŏ���������A�m��/����ł̃G���[�`�F�b�N�A���������NULL
		//const?
		//
		//�����@
		//board.copy_board�̒��g������R�s�[����悤�ɕύX
		//expanded_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true); -> std::vector<Piece_move_data> n_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//���R�͕s��
		pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, false);
		for (int i = 0; i < pmds.size(); i++) n_pmds.push_back(pmds[i]);
		free(b_tmp);

		/*�]�� & �ǉ�*/
		best_E = 0.0;
		MCT_node* best_E_node = NULL;
		for (int i = 0; i < n_pmds.size(); i++) {
			MCT_node* node = (MCT_node*)malloc(sizeof(MCT_node));
			all_nodes.push_back(node);
			node->parent_node = expand_node;
			node->pmd = n_pmds[i];
			node->eval_value = get_evaluation_value(node->pmd->board);
			node->reward_value = get_reward_value(node->pmd);
			node->children_size = 0;

			expand_node->children_node[expand_node->children_size] = node;
			expand_node->children_size++;
			E = node->eval_value + node->reward_value;
			if (E > best_E) {
				best_E = E;
				opt_leaf_node = node;
				best_E_node = node;
			}
		}

		/*�t�`�� (�e�m�[�h��E���X�V����t���œK���Ƃ���)*/
		MCT_node* p_node = expand_node;
		while (true) {
			if (expand_node->eval_value < best_E) {
				expand_node->eval_value = best_E;
			}
			else break;

			if (p_node->parent_node == NULL) {
				opt_leaf_node = best_E_node;
				break;
			}
			else {
				p_node = p_node->parent_node;
			}
		}
		exp_cnt++;
		if (exp_cnt >= 2) break;
	}
	

	for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << std::bitset<10>(opt_leaf_node->pmd->board->board[y]) << std::endl; ofs << std::endl;

	//free
	for (int i = 0; i < all_nodes.size(); i++) {
		if (all_nodes[i]->pmd->board != NULL) free(all_nodes[i]->pmd->board);
		else Comment(TEXT("a"), 0, 0);
		if (all_nodes[i]->pmd != NULL) free(all_nodes[i]->pmd); 
		else Comment(TEXT("b"), 0, 0);
		if(all_nodes[i] != NULL) free(all_nodes[i]);
		else Comment(TEXT("c"), 0, 0);
	}
	return opt_sol;
}

float Evaluation::get_evaluation_value(Board* board) {

	float result = 1.0;
	result = rnd() % 50;
		//�n�`�]��
	//�l�N�X�g�]��
	return result;
}

float Evaluation::get_reward_value(Piece_move_data *pmd) {
	float result = 0.0;
	result = rnd() % 50;
	//�Η�
	//�u���̂ɂ����鎞��
	return result;
}

// todo �����u���������ʂ̑�����@�Œu���Ă���ꍇ������
std::vector<Piece_move_data*> Evaluation::find_peice_moves(Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold) {
	/*
	�n�[�h�h���b�v�̏ꍇ
		O�~�m�͉�]0�̂�
		���̑���4��]��
	*/
	std::vector<Piece_move_data*> result;
	char piece_type = 0;
	if (use_hold && board.hold_piece == PIECE_TYPE_NONE) {
		piece_type = board.next_piece[1];
	}
	else if (use_hold) {
		piece_type = board.hold_piece;
	}else {
		piece_type = board.next_piece[0];
	}
	if (piece_type == PIECE_TYPE_NONE) return result;

	Piece start_piece(piece_type, spawn_x, spawn_y, spawn_r);

	Piece piece_nomove = start_piece;
	Piece piece = start_piece;

	for (int r = 0; r < 4; r++) {
		if (start_piece.type == PIECE_TYPE_O && r == 1) return result;
		{
			Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
			pmd->moves_size = 0;
			pmd->to_r = r;
			pmd->to_x = piece.x;
			pmd->rot_type = Normal;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;

			piece_nomove = start_piece;
			if (r == 1) {
				if (piece_nomove.rotate(&board, Rotate_right) == -1) continue;
			}
			else if (r == 2) {
				if (piece_nomove.rotate(&board, Rotate_right) == -1) continue;
				if (piece_nomove.rotate(&board, Rotate_right) == -1) continue;
			}
			else if (r == 3) {
				if (piece_nomove.rotate(&board, Rotate_left) == -1) continue;
			}

			//�����ʒu�ɐݒu�ł��邩�B
			piece = piece_nomove;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) {
				if (r == 0) {//�X�|�[�����Ay=20�ɐݒu�ł��Ȃ��ꍇ��y=21�ɐݒu�����B
					if (start_piece.y == PIECE_SPAWN_Y) {
						start_piece.y++;
						piece_nomove.y++;
						piece = piece_nomove;
					}
					else break;
					pmd->board = board.copy_board();
					pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_null, true);
					if (pmd->clear_lines == -1) {
						break;
					}
					else {
						pmd->last_x = piece.x;
						pmd->last_y = piece.y;
						pmd->last_r = piece.r;
						pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
						if (pmd->last_y < Defines::Gameover_y[piece.type][piece.r]) {
							result.push_back(pmd);
						}
						else free(pmd->board);
					}
				}
				else continue;
			}
			else {
				pmd->last_x = piece.x;
				pmd->last_y = piece.y;
				pmd->last_r = piece.r;
				pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
				if (pmd->last_y < Defines::Gameover_y[piece.type][piece.r]) {
					result.push_back(pmd);
				}
				else free(pmd->board);
			}
		}

		//���Ɉړ�
		for (int dx = -1; dx > -10; dx--) {
			piece = piece_nomove;
			Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
			pmd->moves_size = 0;
			pmd->to_r = r;
			pmd->rot_type = Normal;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;
			pmd->to_x = piece.x + dx;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) break;
			else {
				pmd->last_x = piece.x;
				pmd->last_y = piece.y;
				pmd->last_r = piece.r;
				pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
				if (pmd->last_y < Defines::Gameover_y[piece.type][piece.r]) {
					result.push_back(pmd);
				}
				else free(pmd->board);
			}
		}

		//�E�Ɉړ�
		for (int dx = 1; dx < 10; dx++) {
			piece = piece_nomove;
			piece = piece_nomove;
			Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
			pmd->moves_size = 0;
			pmd->to_r = r;
			pmd->rot_type = Normal;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;
			pmd->to_x = piece.x + dx;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) break;
			else {
				pmd->last_x = piece.x;
				pmd->last_y = piece.y;
				pmd->last_r = piece.r;
				pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
				if (pmd->last_y < Defines::Gameover_y[piece.type][piece.r]) {
					result.push_back(pmd);
				}
				else free(pmd->board);
			}
		}
	}
	
	//�\�t�g�h���b�v��̒ǉ��ړ�
	//�ݒu���Ď��S����ɂȂ�ꍇ�ƈӖ��̂Ȃ��ړ��̂���is_upper_move�̏ꍇ�͋���
	int Sind = 0, Eind = result.size();
	bool exit_loop = false;
	int add_move_cnt = 0;
	Piece_move_data pmd_i;
	while (true) {
		if (exit_loop) break;
		if (MOVES_SIZE_MAX <= add_move_cnt) break;

		Eind = result.size();
		if (Sind == Eind) break;
		for (int i = Sind; i < Eind; i++) {
			if (i > MOVES_BUFF_MAX) {
				exit_loop = true;
				break;
			}
			
			pmd_i = *result[i];
			if (add_move_cnt == 0) pmd_i.moves[pmd_i.moves_size - 1] = MOVE_SOFTDROP;
			pmd_i.rot_type = Normal;
			//rot_type(t�~�m�A��]�̂�), moves, clear_line, lastx, lasty�X�V

			//���ړ�
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_MOVE_RIGHT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_LEFT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, -1, 0, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height>= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
					else free(pmd->board);
				}
			}

			//�E�ړ�
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_MOVE_LEFT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_RIGHT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 1, 0, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
					else free(pmd->board);
				}
			}

			//���ړ�
			{
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_DOWN;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 0, -1, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
					else free(pmd->board);
				}
			}

			//�E��]
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_ROTATE_LEFT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_ROTATE_RIGHT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				if (piece.type == PIECE_TYPE_T) {
					pmd->rot_type = get_rotation_type(&board, piece, Rotate_right);
				}
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_right, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || pmd->rot_type != Normal || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
					else free(pmd->board);
				}
			}

			//����]
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_ROTATE_RIGHT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_ROTATE_LEFT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				if (piece.type == PIECE_TYPE_T) {
					pmd->rot_type = get_rotation_type(&board, piece, Rotate_left);
				}
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_left, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || pmd->rot_type != Normal || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
					else free(pmd->board);
				}
			}


		}
		Sind = Eind;
		add_move_cnt++;
	}
	
	return result;
}


bool Evaluation::is_meaningful_move(Board* board, Piece piece) {
	for (int dy = 1; dy < BOARD_HEIGHT; dy++) {
		if (piece.y + dy >= BOARD_HEIGHT) break;
		if (!(board->can_place(piece, 0, dy))) return true;
	}
	return false;
}


Rotation_type Evaluation::get_rotation_type( Board* board, Piece piece, Rotation rotation) {
	short srs_ind = board->can_rotate(piece, rotation).index;
	int to_r;
	if (rotation == Rotate_right) to_r = (piece.r + 1) % 4;
	else if (rotation == Rotate_left) to_r = (piece.r + 3) % 4;
	/*
	1o2 r+0or+1
	ooo  o o o
	4 3 r+3 r+2
	*/
	bool tspin_ind[4] = { false, false, false, false };

	tspin_ind[0] = !((board->board[piece.y] & (0b1000000000 >> (piece.x))) == 0);
	tspin_ind[1] = !((board->board[piece.y] & (0b0010000000 >> (piece.x))) == 0);
	tspin_ind[3] = !((board->board[piece.y - 2] & (0b1000000000 >> (piece.x))) == 0);
	tspin_ind[2] = !((board->board[piece.y - 2] & (0b0010000000 >> (piece.x))) == 0);

	int cnt = 0;
	for (int i = 0; i < 4; i++) if (tspin_ind[i])cnt++;

	if (cnt >= 3) {
		if(srs_ind == 4) return TSpin;
		if(!tspin_ind[to_r] || !tspin_ind[(to_r + 1)%4]) return TSpinmini;
		return TSpin;
	}
	else return Normal;
}