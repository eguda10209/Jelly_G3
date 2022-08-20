#include "include.h"

using namespace std;
void set_debug_board(Board *board) {
	board->reset_board();
	board->board[2] = 0b1011001111;
	board->board[1] = 0b1111000110;
	board->board[0] = 0b1111101110;

}

void debug() {
	Board board(0);
	Evaluation ev;

	std::ofstream ofs("./debug.txt");
	/*board.board[0] = BOARD_FILL;
	board.board[1] = BOARD_FILL;
	board.board[2] = 0b1000000000;
	board.board[3] = BOARD_FILL;
	board.board[4] = 0b0100000000;
	board.board[5] = BOARD_FILL;
	board.board[6] = 0b0010000000;
	board.board[38] = BOARD_FILL;
	board.board[39] = 0b0011000000;
	board.output_board();*/


	/*piece defines check
	for (int p = 0; p < 7; p++) {
		for (int r = 0; r < 4; r++) {
			for (int i = 0; i < 4; i++) {
				ofs << bitset<4>(Defines::piece[p][r][i]) << endl;
			}
			ofs << endl;
		}
	}*/

	/*ミノ設置テスト
	for (int m = 0; m < 7; m++) {
		if (m >= 1 && m <= 4) continue;
		for (int r = 0; r < 4; r++) {
			for (int x = -2; x < 9; x++) {
				if (board.can_place(Piece(m, x, 4, r), 0, 0)) {
					board.place_piece(Piece(m, x, 4, r));

					for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl;
					ofs << endl;
					board.reset_board();
				}
			}
		}
	}
	board.output_board();*/

	/*Piece piece(4,-1,7,1);
	piece.place(&board);
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	board.reset_board();

	//piece.fall(&board);

	board.board[4] = 0b0011111111;
	board.board[3] = 0b0001111111;
	board.board[2] = 0b1101111111;
	board.board[1] = 0b1001111111;
	board.board[0] = 0b1101111111;
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;

	piece.rotate(&board, Rotate_left);
	piece.rotate(&board, Rotate_left);
	piece.place(&board);
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	board.reset_board();*/

	/*Piece piece2(6, 0, 9, 0);
	piece2.place(&board);
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	board.reset_board();
	board.board[3] = 0b1101111111;
	board.board[2] = 0b1001111111;
	board.board[1] = 0b1001111111;
	board.board[0] = 0b1101111111;
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	ofs << "cline: " << board.try_place(piece2, 0, 0, Rotate_right) << endl;
	for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;*/

	/*board.hold_piece = 3;
	board.next_piece[0] = 4;
	board.next_piece[1] = 5;
	set_debug_board(&board);
	for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	std::vector<Piece_move_data> moves = ev.find_peice_moves(&board, PIECE_SPAWN_X, PIECE_SPAWN_Y, 0, false);
	for (int i = 0; i < moves.size(); i++) {
		//ofs << moves[i].to_x << " " << moves[i].to_r << " " << moves[i].last_r << endl;
		//for (int j = 0; j < moves[i].moves.size(); j++) ofs << moves[i].moves[j] <<" ";
		//ofs << endl;
		if (moves[i].rot_type == Normal) ofs << "tspin: normal" << endl;
		else if (moves[i].rot_type == TSpinmini) ofs << "tspin: TSpinmini" << endl;
		else if (moves[i].rot_type == TSpin) ofs << "tspin: TSpin" << endl;
		ofs << moves[i].clear_lines << endl;
		ofs << "btb: " << moves[i].board->btb << endl;
		ofs << "combo: " << moves[i].board->combo << endl;
		ofs << (int)moves[i].board->hold_piece << " ";
		for (int j = 0; j < 5; j++) ofs << (int)moves[i].board->next_piece[j] << " "; ofs << endl;
		for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(moves[i].board->board[y]) << endl; ofs << endl;
	}

	for (int i = 0; i < moves.size(); i++) free(moves[i].board);


	for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;*/
	
	/*board.hold_piece = 0;
	board.next_piece[0] = 1;
	board.next_piece[1] = 2;
	board.next_piece[2] = 3;
	board.next_piece[3] = 4;
	board.next_piece[4] = 5;
	board.next_piece[5] = 6;

	Solution opt_sol = ev.find_optimal_solution(board, PIECE_SPAWN_X, PIECE_SPAWN_Y, 0);
	Comment(TEXT("depth: "), opt_sol.depth, 1);
	Comment(TEXT("nodes: "), opt_sol.nodes, 1);
	Comment(TEXT("score: "), opt_sol.score, 1);

	for (int i = 0; i < opt_sol.moves_data.size(); i++) {
		for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(opt_sol.moves_data[i]->board->board[y]) << endl; ofs << endl; 
	}

	for (int i = 0; i < opt_sol.moves_data.size(); i++) {
		free(opt_sol.moves_data[i]->board);
		free(opt_sol.moves_data[i]);
	}*/
/*
set_debug_board(&board);
for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	short heights[10];
	for (int i = 0; i < 10; i++) {
		heights[i] = ev.get_heights(&board, i);
	}
	 ofs << ev.get_holes(&board, heights);*/
}

