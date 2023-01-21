#include "evaluation.h"

Solution Evaluation::find_optimal_solution(Board initial_board, short spawn_x, short spawn_y, short spawn_r, int *expand_max) {
	Solution opt_sol;
	//展開するノードの選択
	//展開 find_peice_moves(展開するノードの盤面、設置するミノ、上段操作モードか)
	// 	   ー＞vector<Piece_move_data>：　移動可能なミノ情報(移動方法、)
	//展開したノードに対して評価
	// 
	//評価の逆伝搬

	std::vector<MCT_node*> all_nodes;	//全ノード
	int debug_pushback_cnt = 0;

	std::vector<Piece_move_data*> expanded_pmds;
	MCT_node root_nodes;
	MCT_node* opt_leaf_node = NULL;

	//初期ノードの展開
	std::vector<Piece_move_data*> pmds;
	expanded_pmds = find_peice_moves(initial_board, spawn_x, spawn_y, spawn_r, false);
	pmds = find_peice_moves(initial_board, spawn_x, spawn_y, spawn_r, true);
	for (int i = 0; i < pmds.size(); i++) expanded_pmds.push_back(pmds[i]);
	if (expanded_pmds.size() == 0) {
		Comment(TEXT("dead"), 0, 0);
		return opt_sol;
	}

	//初期ノードの評価 todo 死亡する場合はEを0にする
	float best_E = -20000.0;
	float E = 0.0;
	for (int i = 0; i < expanded_pmds.size(); i++) {
		MCT_node *node = (MCT_node*) malloc(sizeof(MCT_node));
		all_nodes.push_back(node);
		node->parent_node = &root_nodes;
		node->pmd = expanded_pmds[i];
		node->eval_value = get_evaluation_value(node->pmd->board);
		node->reward_value = get_reward_value(node->pmd->board, node->pmd);
		node->children_size = 0;

		root_nodes.children_node[root_nodes.children_size] = node;
		root_nodes.children_size++;
		E = node->eval_value + node->reward_value;
		if (E > best_E) {
			best_E = E; 
			opt_leaf_node = node;
		}
	}

	//初期ノード評価の逆伝搬
	root_nodes.eval_value = best_E;
	opt_sol.depth = 1;

	std::ofstream ofs("./ev_debug.txt");
	ofs << "start" << std::endl;

	int exp_cnt = 0;
	while (true) {

		//展開するノードの選択
		MCT_node* select_node = &root_nodes;
		MCT_node* expand_node = NULL;
		int exp_depth = 0;
		while (true) {
			int children_size = select_node->children_size;
			if (children_size == 0) {
				expand_node = select_node;
				break;
			}
			std::vector<std::pair<float, int>> E_rank;
			int* p = (int*)malloc(sizeof(int) * children_size);

			//E(eval_value + reward_value)の計算
			for (int i = 0; i < children_size; i++) {
				E_rank.push_back({ select_node->children_node[i]->eval_value + select_node->children_node[i]->reward_value, i });
			}
			std::sort(E_rank.begin(), E_rank.end(), std::greater<std::pair<float, int>>());//降順ソート

			//選択される確率の計算
			float E_min = E_rank[children_size - 1].first;
			int sum = 0.0;
			for (int i = 0; i < children_size; i++) {
				p[i] = (((float)((E_rank[i].first - E_min)) + 10.0) / (float)((i)+1.0)) * 100;
				sum += p[i];
			}

			//選択
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
			exp_depth++;
		}
		if (opt_sol.depth < exp_depth) opt_sol.depth = exp_depth;

		/*展開*/
		Board* b_tmp = (*(*expand_node).pmd).board->copy_board();
		std::vector<Piece_move_data*> n_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//expanded_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//上記関数処理後、expand_node->pmdのデータがおかしくなる(戻り値もおかしい？) デバッガの問題？
		// 
		// Board *b_tmp = expand_node->pmd->board->copy_board();でexpand_nodeを使用すると起きる？
		// 
		//ポインタはNULLで初期化する、確保/解放でのエラーチェック、解放したらNULL
		//const?
		//
		//解決法
		//board.copy_boardの中身を一つずつコピーするように変更
		//expanded_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true); -> std::vector<Piece_move_data> n_pmds = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, true);
		//理由は不明
		std::vector<Piece_move_data*> pmds_2 = find_peice_moves(*b_tmp, spawn_x, spawn_y, spawn_r, false);
		for (int i = 0; i < pmds_2.size(); i++) n_pmds.push_back(pmds_2[i]);
		free(b_tmp);
		if (n_pmds.size() == 0) {
			continue;
			Comment(TEXT("moves size: 0"), 0, 0);
		}

		/*評価 & 追加*/
		best_E = -20000.0;
		MCT_node* best_E_node = NULL;
		for (int i = 0; i < n_pmds.size(); i++) {
			MCT_node* node = (MCT_node*)malloc(sizeof(MCT_node));
			all_nodes.push_back(node);
			node->parent_node = expand_node;
			node->pmd = n_pmds[i];
			node->eval_value = get_evaluation_value(node->pmd->board);
			node->reward_value = get_reward_value(node->pmd->board, node->pmd);
			node->children_size = 0;

			expand_node->children_node[expand_node->children_size] = node;
			expand_node->children_size++;
			E = node->eval_value + node->reward_value;
			if (E > best_E) {
				best_E = E;
				best_E_node = node;
			}
		}

		/*逆伝搬 (親ノードのEを更新しる葉を最適解とする)*/
		MCT_node* p_node = expand_node;
		while (true) {
			if (p_node->eval_value < best_E) {
				p_node->eval_value = best_E;
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
		if (*expand_max >= 0) {
			if (exp_cnt >= *expand_max) break;//300
		}
		else {
			if (exp_cnt >= 3000) break;//300
		}
	}
	
	/*最適解情報*/
	opt_sol.score = root_nodes.eval_value;
	MCT_node* opt_node_tmp = opt_leaf_node;
	opt_sol.moves_data_size = 0;
	while (true){
		if (opt_node_tmp->parent_node == NULL) break;
		Piece_move_data* pmd_tmp = (Piece_move_data*)malloc(sizeof(Piece_move_data));
		*pmd_tmp = *opt_node_tmp->pmd;
		pmd_tmp->board = (*(*opt_node_tmp).pmd).board->copy_board();
		opt_sol.moves_data[opt_sol.moves_data_size] = pmd_tmp;
		opt_sol.moves_data_size++;
		//opt_sol.moves_data.insert(opt_sol.moves_data.begin(), pmd_tmp);
		opt_node_tmp = opt_node_tmp->parent_node;
	}


	for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << std::bitset<10>(opt_leaf_node->pmd->board->board[y]) << std::endl; ofs << std::endl;

	//free
	int all_nodes_size = all_nodes.size();
	opt_sol.nodes = all_nodes_size;
	int free_cnt = 0;
	for (int i = 0; i < all_nodes_size; i++) {
		if (all_nodes[i]->pmd->board != NULL) {
			free(all_nodes[i]->pmd->board);
			free_cnt++;
		}
		if (all_nodes[i]->pmd != NULL){
			free(all_nodes[i]->pmd); 
			free_cnt++;
		}
		if(all_nodes[i] != NULL) {
			free(all_nodes[i]);
			free_cnt++;
		}
	}
	//Comment(TEXT("all: "), all_nodes_size, 1); 
	//Comment(TEXT("fc: "), free_cnt, 1);
	return opt_sol;
}

float Evaluation::get_evaluation_value(Board* board) {
	float result = 50.0;
	short heights[10];
	for (int i = 0; i < 10; i++) {
		heights[i] = get_heights(board, i);
	}
	result += get_holes(board, heights) * (-1);
	result += board->max_height * (-1);
		//地形評価
	//ネクスト評価
	return result;
}

float Evaluation::get_reward_value(Board* board, Piece_move_data *pmd) {
	float result = 50.0;
	/*attk_type*/
	Attk_type attk_type;
	if (board->board[0] == BOARD_EMPTY) attk_type = PerfectClear;
	else if (pmd->clear_lines == 0 && pmd->rot_type == Normal) attk_type = None;
	else if (pmd->clear_lines == 1 && pmd->rot_type == Normal) attk_type = LineClear1;
	else if (pmd->clear_lines == 2 && pmd->rot_type == Normal) attk_type = LineClear2;
	else if (pmd->clear_lines == 3 && pmd->rot_type == Normal) attk_type = LineClear3;
	else if (pmd->clear_lines == 4) attk_type = Tetris;
	else if (pmd->rot_type == TSpinmini) attk_type = TSpinMini_;
	else if (pmd->clear_lines == 1 && pmd->rot_type == TSpin) attk_type = TSpin1;
	else if (pmd->clear_lines == 2 && pmd->rot_type == TSpin) attk_type = TSpin2;
	else if (pmd->clear_lines == 3 && pmd->rot_type == TSpin) attk_type = TSpin3;

	//火力
	result += get_attk(board, pmd, attk_type) * 5;

	if (attk_type == LineClear1) result += -2;
	else if (attk_type == LineClear2) result += -2;
	else if (attk_type == LineClear3) result += -1;

	//置くのにかかる時間
	return result;
}

short Evaluation::get_attk(Board* board, Piece_move_data* pmd, Attk_type attk_type) {
	short attk = 0;
	short b_ren_bonus = 0;	//設置前の累計連ボーナス
	short c_ren_bonus = 0;	//設置による連ボーナス
	if(board->b_combo >= 0 && board->b_combo <= 10) b_ren_bonus = Defines::ren_bonus[board->b_combo];
	else {
		b_ren_bonus = Defines::ren_bonus[10];
		b_ren_bonus += (board->b_combo - 10) * 5;
	}
	if (board->combo >= 1 && board->combo <= 10) c_ren_bonus = Defines::ren_bonus[board->combo] - Defines::ren_bonus[board->combo - 1];
	else {
		c_ren_bonus = 5;
	}

	if (attk_type == PerfectClear) attk = 10;
	else if (attk_type == LineClear2) attk = 1;
	else if (attk_type == LineClear3) attk = 2;
	else {
		if (attk_type == Tetris) attk = 4;
		else if (attk_type == TSpinMini_) attk = 0;
		else if (attk_type == TSpin1) attk = 2;
		else if (attk_type == TSpin2) attk = 4;
		else if (attk_type == TSpin3) attk = 6;
		if (board->b_btb) attk++;
	}

	return attk + b_ren_bonus + c_ren_bonus;
}

short Evaluation::get_heights(Board* board, short x) {
	for (int y = board->max_height - 1; y >= 0; y--) {
		if ((board->board[y] & (0b1000000000 >> x)) != 0) return y + 1;
	}
	return 0;
}

short Evaluation::get_holes(Board* board, short heights[10]) {
	short holes = 0;
	for (int x = 0; x < 10; x++) {
		for (short y = heights[x] - 2; y >= 0; y--) {
			if ((board->board[y] & (0b1000000000 >> x)) == 0) holes++;
		}
	}
	return holes;
}

// todo 同じ置き方だが別の操作方法で置いている場合もある
std::vector<Piece_move_data*> Evaluation::find_peice_moves(Board board, short spawn_x, short spawn_y, short spawn_r, bool use_hold) {
	/*
	ハードドロップの場合
		Oミノは回転0のみ
		その他は4回転分
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
			pmd->use_hold = use_hold;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;

			piece_nomove = start_piece;
			if (r == 1) {
				if (piece_nomove.rotate(&board, Rotate_right) == false) continue;
			}
			else if (r == 2) {
				if (piece_nomove.rotate(&board, Rotate_right) == false) continue;
				if (piece_nomove.rotate(&board, Rotate_right) == false) continue;
			}
			else if (r == 3) {
				if (piece_nomove.rotate(&board, Rotate_left) == false) continue;
			}

			//初期位置に設置できるか。
			piece = piece_nomove;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) {
				if (r == 0) {//スポーン時、y=20に設置できない場合はy=21に設置される。
					if (start_piece.y == PIECE_SPAWN_Y) {
						start_piece.y++;
						piece_nomove.y++;
						piece = piece_nomove;
					}
					else {
						free(pmd);
						break;
					}
					free(pmd->board);
					pmd->board = board.copy_board();
					pmd->clear_lines = pmd->board->try_place(&piece, 0, 0, Rotate_null, true);
					if (pmd->clear_lines == -1) {
						free(pmd->board);
						free(pmd);
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
						else {
							free(pmd->board);
							free(pmd);
						}
					}
				}
				else {
					free(pmd);
					continue;
				}
			}
			else {
				pmd->last_x = piece.x;
				pmd->last_y = piece.y;
				pmd->last_r = piece.r;
				pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
				if (pmd->last_y < Defines::Gameover_y[piece.type][piece.r]) {
					result.push_back(pmd);
				}
				else {
					free(pmd->board);
					free(pmd);
				}
			}
		}

		//左に移動
		for (int dx = -1; dx > -10; dx--) {
			piece = piece_nomove;
			Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
			pmd->moves_size = 0;
			pmd->to_r = r;
			pmd->rot_type = Normal;
			pmd->use_hold = use_hold;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;
			pmd->to_x = piece.x + dx;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) {
				free(pmd->board);
				free(pmd);
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
				else {
					free(pmd->board);
					free(pmd);
				}
			}
		}

		//右に移動
		for (int dx = 1; dx < 10; dx++) {
			piece = piece_nomove;
			piece = piece_nomove;
			Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
			pmd->moves_size = 0;
			pmd->to_r = r;
			pmd->rot_type = Normal;
			pmd->use_hold = use_hold;
			pmd->moves[pmd->moves_size] = MOVE_HARDDROP;
			pmd->moves_size++;
			pmd->to_x = piece.x + dx;
			pmd->board = board.copy_board();
			pmd->clear_lines = pmd->board->try_place(&piece, dx, 0, Rotate_null, true);
			if (pmd->clear_lines == -1) {
				free(pmd->board);
				free(pmd);
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
				else {
					free(pmd->board);
					free(pmd);
				}
			}
		}
	}
	
	//ソフトドロップ後の追加移動
	//設置して死亡判定になる場合と意味のない移動のぞくis_upper_moveの場合は許可
	int Sind = 0, Eind = result.size();
	bool exit_loop = false;
	int add_move_cnt = 0;
	int result_ind_cnt = Eind;
	Piece_move_data pmd_i;
	while (true) {
		if (exit_loop) break;
		if (MOVES_SIZE_MAX <= add_move_cnt) break;

		Eind = result.size();
		if (Sind == Eind) break;
		for (int i = Sind; i < Eind; i++) {
			pmd_i = *result[i];
			if (add_move_cnt == 0) pmd_i.moves[pmd_i.moves_size - 1] = MOVE_SOFTDROP;
			pmd_i.rot_type = Normal;
			//rot_type(tミノ、回転のみ), moves, clear_line, lastx, lasty更新

			//左移動
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_MOVE_RIGHT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_LEFT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->use_hold = use_hold;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, -1, 0, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height>= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
						result_ind_cnt++;
						if (result_ind_cnt >= MOVES_BUFF_MAX) {
							exit_loop = true;
							break;
						}
					}
					else {
						free(pmd->board);
						free(pmd);
					}
				}
				else {
					free(pmd->board);
					free(pmd);
				}
			}

			//右移動
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_MOVE_LEFT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_RIGHT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->use_hold = use_hold;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 1, 0, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
						result_ind_cnt++;
						if (result_ind_cnt >= MOVES_BUFF_MAX) {
							exit_loop = true;
							break;
						}
					}
					else {
						free(pmd->board);
						free(pmd);
					}
				}
				else {
					free(pmd->board);
					free(pmd);
				}
			}

			//下移動
			{
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_MOVE_DOWN;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->use_hold = use_hold;
				pmd->board = board.copy_board();
				pmd->clear_lines = pmd->board->try_place(&piece, 0, -1, Rotate_null, false);
				if (pmd->clear_lines != -1) {
					pmd->last_x = piece.x;
					pmd->last_y = piece.y;
					pmd->last_r = piece.r;
					pmd->board->updata_board_status(pmd->clear_lines, pmd->rot_type, use_hold);
					if ((pmd->board->max_height >= UPPER_MOVE_Y_TH || is_meaningful_move(&board, piece)) && (pmd->last_y < Defines::Gameover_y[piece.type][piece.r])) {
						result.push_back(pmd);
						result_ind_cnt++;
						if (result_ind_cnt >= MOVES_BUFF_MAX) {
							exit_loop = true;
							break;
						}
					}
					else {
						free(pmd->board);
						free(pmd);
					}
				}
				else {
					free(pmd->board);
					free(pmd);
				}
			}

			//右回転
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_ROTATE_LEFT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_ROTATE_RIGHT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->use_hold = use_hold;
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
						result_ind_cnt++;
						if (result_ind_cnt >= MOVES_BUFF_MAX) {
							exit_loop = true;
							break;
						}
					}
					else {
						free(pmd->board);
						free(pmd);
					}
				}
				else {
					free(pmd->board);
					free(pmd);
				}
			}

			//左回転
			if (pmd_i.moves[pmd_i.moves_size - 1] != MOVE_ROTATE_RIGHT) {
				Piece_move_data* pmd = (Piece_move_data*)malloc(sizeof(Piece_move_data));
				*pmd = pmd_i;
				pmd->moves[pmd->moves_size] = MOVE_ROTATE_LEFT;
				pmd->moves_size++;
				piece.x = pmd->last_x;
				piece.y = pmd->last_y;
				piece.r = pmd->last_r;
				pmd->use_hold = use_hold;
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
						result_ind_cnt++;
						if (result_ind_cnt >= MOVES_BUFF_MAX) {
							exit_loop = true;
							break;
						}
					}
					else {
						free(pmd->board);
						free(pmd);
					}
				}
				else {
					free(pmd->board);
					free(pmd);
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