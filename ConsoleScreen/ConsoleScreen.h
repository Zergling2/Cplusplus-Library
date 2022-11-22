#pragma once

#include <Windows.h>

class ConsoleScreen
{
public:
	ConsoleScreen(unsigned int dfScreenWidth, unsigned int dfScreenHeight);
	~ConsoleScreen();
	void ClearScreen();
	void BufferClear();
	void PrintScreen();
	void DrawSprite(char chSprite, int xPos, int yPos);
	void MoveCursor(short xPos, short yPos);
private:
	void ConsoleInitialize(void);
	char* screen;
	int dfScreenWidth;
	int dfScreenHeight;
	HANDLE hConsole;
};
