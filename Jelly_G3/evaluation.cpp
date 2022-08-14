#include "evaluation.h"

Solution Evaluation::find_optimal_solution(short next_num, std::vector<char> pieces) {
	Solution opt_sol;

	return opt_sol;
}

int Evaluation::get_evaluation_value(Board* board) {
	int result = 0;

	return result;
}

int Evaluation::get_reward_value(Board* board, Piece_move_data pmd) {
	int result = 0;

	return result;
}

std::vector<Piece_move_data> Evaluation::find_peice_moves(Board *board, Piece start_piece, bool is_upper_move) {
	/*
	�n�[�h�h���b�v�̏ꍇ
		O�~�m�͉�]0�̂�
		���̑���4��]��
	*/
	std::vector<Piece_move_data> result;
	Piece piece_nomove = start_piece;
	Piece piece = start_piece;

	for (int r = 0; r < 4; r++) {
		if (start_piece.type == PIECE_TYPE_O && r == 1) return result;
		Piece_move_data pmd;
		pmd.to_r = r;
		pmd.to_x = piece.x;
		pmd.rot_type = Normal;
		pmd.moves.push_back(MOVE_HARDDROP);

		piece_nomove = start_piece;
		if (r == 1) {
			if (piece_nomove.rotate(board, Rotate_right) == -1) continue;
		}
		else if (r == 2) {
			if (piece_nomove.rotate(board, Rotate_right) == -1) continue;
			if (piece_nomove.rotate(board, Rotate_right) == -1) continue;
		}
		else if (r == 3) {
			if (piece_nomove.rotate(board, Rotate_left) == -1) continue;
		}
		 
		//�����ʒu�ɐݒu�ł��邩�B
		piece = piece_nomove;
		pmd.clear_lines = board->try_place(&piece, 0, 0, Rotate_null, true);
		if (pmd.clear_lines == -1) {
			if (r == 0) {//�X�|�[�����Ay=20�ɐݒu�ł��Ȃ��ꍇ��y=21�ɐݒu�����B
				if (start_piece.y == PIECE_SPAWN_Y) {
					start_piece.y++;
					piece_nomove.y++;
					piece = piece_nomove;
				}else break;
				pmd.clear_lines = board->try_place(&piece, 0, 0, Rotate_null, true);
				if (pmd.clear_lines == -1) {
					break;
				}
				else {
					pmd.last_x = piece.x;
					pmd.last_y = piece.y;
					pmd.last_r = piece.r;
					if (pmd.last_y < Defines::Gameover_y[piece.type][piece.r]) result.push_back(pmd);
				}
			}
			else continue;
		}
		else {
			pmd.last_x = piece.x;
			pmd.last_y = piece.y;
			pmd.last_r = piece.r;
			if (pmd.last_y < Defines::Gameover_y[piece.type][piece.r]) result.push_back(pmd);
		}

		//���Ɉړ�
		for (int dx = -1; dx > -10; dx--) {
			piece = piece_nomove;
			pmd.to_x = piece.x + dx;
			pmd.clear_lines = board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd.clear_lines == -1) break;
			else {
				pmd.last_x = piece.x;
				pmd.last_y = piece.y;
				pmd.last_r = piece.r;
				if (pmd.last_y < Defines::Gameover_y[piece.type][piece.r]) result.push_back(pmd);
			}
		}

		//�E�Ɉړ�
		for (int dx = 1; dx < 10; dx++) {
			piece = piece_nomove;
			pmd.to_x = piece.x + dx;
			pmd.clear_lines = board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd.clear_lines == -1) break;
			else {
				pmd.last_x = piece.x;
				pmd.last_y = piece.y;
				pmd.last_r = piece.r;
				if (pmd.last_y < Defines::Gameover_y[piece.type][piece.r]) result.push_back(pmd);
			}
		}
	}

	//�\�t�g�h���b�v��̒ǉ��ړ�
	//�ݒu���Ď��S����ɂȂ�ꍇ�ƈӖ��̂Ȃ��ړ��̂���is_upper_move�̏ꍇ�͋���
	int Sind = 0, Eind = result.size();
	bool exit_loop = false;
	int add_move_cnt = 0;
	Piece_move_data pmd_i, pmd;
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
			
			pmd_i = result[i];
			if (add_move_cnt == 0) pmd_i.moves[pmd_i.moves.size() - 1] = MOVE_SOFTDROP;
			pmd_i.rot_type = Normal;
			//rot_type(t�~�m�A��]�̂�), moves, clear_line, lastx, lasty�X�V

			//���ړ�
			if (pmd_i.moves[pmd_i.moves.size() - 1] != MOVE_MOVE_RIGHT) {
				pmd = pmd_i;
				pmd.moves.push_back(MOVE_MOVE_LEFT);
				piece.x = pmd.last_x;
				piece.y = pmd.last_y;
				piece.r = pmd.last_r;
				pmd.clear_lines = board->try_place(&piece, -1, 0, Rotate_null, false);
				if (pmd.clear_lines != -1) {
					pmd.last_x = piece.x;
					pmd.last_y = piece.y;
					pmd.last_r = piece.r;
					if ((is_upper_move || is_meaningful_move(board, piece)) && (pmd.last_y < Defines::Gameover_y[piece.type][piece.r])) result.push_back(pmd);
				}
			}

			//�E�ړ�
			if (pmd_i.moves[pmd_i.moves.size() - 1] != MOVE_MOVE_LEFT) {
				pmd = pmd_i;
				pmd.moves.push_back(MOVE_MOVE_RIGHT);
				piece.x = pmd.last_x;
				piece.y = pmd.last_y;
				piece.r = pmd.last_r;
				pmd.clear_lines = board->try_place(&piece, 1, 0, Rotate_null, false);
				if (pmd.clear_lines != -1) {
					pmd.last_x = piece.x;
					pmd.last_y = piece.y;
					pmd.last_r = piece.r;
					if ((is_upper_move || is_meaningful_move(board, piece)) && (pmd.last_y < Defines::Gameover_y[piece.type][piece.r])) result.push_back(pmd);
				}
			}

			//���ړ�
			pmd = pmd_i;
			pmd.moves.push_back(MOVE_MOVE_DOWN);
			piece.x = pmd.last_x;
			piece.y = pmd.last_y;
			piece.r = pmd.last_r;
			pmd.clear_lines = board->try_place(&piece, 0, -1, Rotate_null, false);
			if (pmd.clear_lines != -1) {
				pmd.last_x = piece.x;
				pmd.last_y = piece.y;
				pmd.last_r = piece.r;
				if ((is_upper_move || is_meaningful_move(board, piece)) && (pmd.last_y < Defines::Gameover_y[piece.type][piece.r])) result.push_back(pmd);
			}

			//�E��]
			if (pmd_i.moves[pmd_i.moves.size() - 1] != MOVE_ROTATE_LEFT) {
				pmd = pmd_i;
				pmd.moves.push_back(MOVE_ROTATE_RIGHT);
				piece.x = pmd.last_x;
				piece.y = pmd.last_y;
				piece.r = pmd.last_r;
				if (piece.type == PIECE_TYPE_T) {
					pmd.rot_type = get_rotation_type(board, piece, Rotate_right);
				}
				pmd.clear_lines = board->try_place(&piece, 0, 0, Rotate_right, false);
				if (pmd.clear_lines != -1) {
					pmd.last_x = piece.x;
					pmd.last_y = piece.y;
					pmd.last_r = piece.r;
					if ((is_upper_move || pmd.rot_type != Normal || is_meaningful_move(board, piece)) && (pmd.last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
				}
			}

			//����]
			if (pmd_i.moves[pmd_i.moves.size() - 1] != MOVE_ROTATE_RIGHT) {
				pmd = pmd_i;
				pmd.moves.push_back(MOVE_ROTATE_LEFT);
				piece.x = pmd.last_x;
				piece.y = pmd.last_y;
				piece.r = pmd.last_r;
				if (piece.type == PIECE_TYPE_T) {
					pmd.rot_type = get_rotation_type(board, piece, Rotate_left);
				}
				pmd.clear_lines = board->try_place(&piece, 0, 0, Rotate_left, false);
				if (pmd.clear_lines != -1) {
					pmd.last_x = piece.x;
					pmd.last_y = piece.y;
					pmd.last_r = piece.r;
					if ((is_upper_move || pmd.rot_type != Normal || is_meaningful_move(board, piece)) && (pmd.last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
					}
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