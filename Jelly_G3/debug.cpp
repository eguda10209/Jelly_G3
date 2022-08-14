#include "include.h"

using namespace std;
void set_debug_board(Board *board) {
	board->reset_board();
	board->board[2] = 0b1011001111;
	board->board[1] = 0b1011000111;
	board->board[0] = 0b1011101111;

}

void debug() {
	Board board;
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

	Piece piece3(4, PIECE_SPAWN_X, PIECE_SPAWN_Y, 0);
	set_debug_board(&board);
	for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
	std::vector<Piece_move_data> moves = ev.find_peice_moves(&board, piece3, false);
	for (int i = 0; i < moves.size(); i++) {
		Piece piece_tmp = piece3;
		piece_tmp.x = moves[i].last_x;
		piece_tmp.y = moves[i].last_y;
		piece_tmp.r = moves[i].last_r;
		piece_tmp.place(&board);
		ofs << moves[i].to_x << " " << moves[i].to_r << " " << moves[i].last_r << endl;
		for (int j = 0; j < moves[i].moves.size(); j++) ofs << moves[i].moves[j] <<" ";
		ofs << endl;
		if (moves[i].rot_type == Normal) ofs << "tspin: normal" << endl;
		else if (moves[i].rot_type == TSpinmini) ofs << "tspin: TSpinmini" << endl;
		else if (moves[i].rot_type == TSpin) ofs << "tspin: TSpin" << endl;
		for (int y = BOARD_HEIGHT - 1 - 15; y >= 0; y--) ofs << bitset<10>(board.board[y]) << endl; ofs << endl;
		set_debug_board(&board);
	}
}

