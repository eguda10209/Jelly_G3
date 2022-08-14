#pragma once

#define THINKBOARD_H 24
#define THINKBOARD_W 10

void Comment(LPCTSTR text, int num, int usevar);
bool GetIsAION();
int Getnextthink();
int GetAIType();
int GetPlayerNumber();
void PrintColorBoard(int colorboard[24][10]);
void CommentStr(std::string str);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wc, LPARAM lp);