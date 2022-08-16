#include "include.h"

unsigned short Defines::piece[7][4][4] = { {	{ 0b0110, 0b1100, 0b0000, 0b0000 },
											{ 0b0100, 0b0110, 0b0010, 0b0000 },
											{ 0b0000, 0b0110, 0b1100, 0b0000 },
											{ 0b1000, 0b1100, 0b0100, 0b0000 }},

										{	{ 0b1100, 0b0110, 0b0000, 0b0000 },
											{ 0b0010, 0b0110, 0b0100, 0b0000 },
											{ 0b0000, 0b1100, 0b0110, 0b0000 },
											{ 0b0100, 0b1100, 0b1000, 0b0000 }},

										{	{ 0b1000, 0b1110, 0b0000, 0b0000 },
											{ 0b0110, 0b0100, 0b0100, 0b0000 },
											{ 0b0000, 0b1110, 0b0010, 0b0000 },
											{ 0b0100, 0b0100, 0b1100, 0b0000 }},

										{	{ 0b0010, 0b1110, 0b0000, 0b0000 },
											{ 0b0100, 0b0100, 0b0110, 0b0000 },
											{ 0b0000, 0b1110, 0b1000, 0b0000 },
											{ 0b1100, 0b0100, 0b0100, 0b0000 }},

										{	{ 0b0100, 0b1110, 0b0000, 0b0000 },
											{ 0b0100, 0b0110, 0b0100, 0b0000 },
											{ 0b0000, 0b1110, 0b0100, 0b0000 },
											{ 0b0100, 0b1100, 0b0100, 0b0000 }},

										{	{ 0b0110, 0b0110, 0b0000, 0b0000 },
											{ 0b0110, 0b0110, 0b0000, 0b0000 },
											{ 0b0110, 0b0110, 0b0000, 0b0000 },
											{ 0b0110, 0b0110, 0b0000, 0b0000 }},

										{	{ 0b0000, 0b1111, 0b0000, 0b0000 },
											{ 0b0010, 0b0010, 0b0010, 0b0010 },
											{ 0b0000, 0b0000, 0b1111, 0b0000 },
											{ 0b0100, 0b0100, 0b0100, 0b0100 }}};

short Defines::piece_place_range_X[7][4][2] = {{{0,7}, {-1,7}, {0,7}, {0,8}},
												{{0,7}, {-1,7}, {0,7}, {0,8}}, 
												{{0,7}, {-1,7}, {0,7}, {0,8}}, 
												{{0,7}, {-1,7}, {0,7}, {0,8}}, 
												{{0,7}, {-1,7}, {0,7}, {0,8}}, 
												{{-1,7}, {-1,7}, {-1,7}, {-1,7}}, 
												{{0,6}, {-2,7}, {0,6}, {-1,8}}};

short Defines::I_LeftRotation_dxy[4][5][2] = {  {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},
												{{0,0},{2,0},{-1,0},{2,1},{-1,-2}},
												{{0,0},{1,0},{-2,0},{1,-2},{-2,1}},
												{{0,0},{1,0},{-2,0},{-2,-1},{1,2}} };

short Defines::SZJLT_LeftRotation_dxy[4][5][2] = {  {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
													{{0,0},{1,0},{1,-1},{0,2},{1,2}},
													{{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
													{{0,0},{-1,0},{-1,-1},{0,2},{-1,2}} };


short Defines::Gameover_y[7][4] = { {21,22,22,22},
									{21,22,22,22},
									{21,22,22,22},
									{21,22,22,22},
									{21,22,22,22},
									{21,21,21,21},
									{21,23,22,23} };
