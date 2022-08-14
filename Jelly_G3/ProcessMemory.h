#pragma once

static class ProcessMemory {
private:
	static DWORD pID;
	static HWND hGameWindow;
	static HANDLE pHandle;
public:
	static void GetHandle();
	static intptr_t ReadMemorys(LPCVOID addr);
	static int ReadMemory(LPCVOID addr);
	static intptr_t ReadBytes(LPCVOID addr, int bytes);
	static int GetHoldmino(int player);
	static int GetCurrentmino(int player);
	static int GetNextmino(int, int);
	static int GetBoard(int, int, int);
	static bool IsGame();
	static int GetFrames();
	static int GetScore();
	static int GetLinesCleared();
	static int GetY(int player);
	static int GetX(int player);
	static int GetRotation(int player);
	static bool IsCharacterSelect();
	static int GetCharacter(int player);
	static int GameState(int player);
	static bool IsPause(int player);
	static int GetGarbageLines(int player);
	static bool IsHoldEmpty(int player);
	static int test();
};

