#include "piece.h"

void Piece::move_left() {
	this->x--;
}

void Piece::move_right() {
	this->x++;
}

void Piece::move_down() {
	this->y--;
}

void Piece::rotate_left() {
	this->r = (this->r + 3) % 4;
}
void Piece::rotate_right() {
	this->r = (this->r + 1) % 4;
}

bool Piece::move(Board *board, short dx, short dy) {
	if (board->can_place(*this, dx, dy)) {
		this->x += dx;
		this->y += dy;
		return true;
	}
	return false;
}

bool Piece::rotate(Board *board, Rotation rotation) {
	SRS_data ret = board->can_rotate(*this, rotation);
	if (ret.index == -1) return false;
	else {
		this->x += ret.dx;
		this->y += ret.dy;
		this->r = ret.to_r;
		return true;
	}
	return false;
}

short Piece::place(Board* board) {
	board->place_piece(*this);
	return board->clear_line();
}

void Piece::fall(class Board* board) {
	for (int dy = 1; dy < BOARD_HEIGHT; dy++) {
		if (!board->can_place(*this, 0, -dy)) {
			this->y += -dy + 1;
			return;
		}
	}
}