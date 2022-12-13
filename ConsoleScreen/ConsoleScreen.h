#pragma once

typedef void* HANDLE;

class ConsoleScreen
{
public:
	ConsoleScreen(int _ScreenWidth, int _ScreenHeight);
	~ConsoleScreen();
	void ClearScreen();
	void BufferClear();
	void PrintScreen();
	void DrawSprite(char chSprite, int xPos, int yPos);
	void MoveCursor(short xPos, short yPos);
private:
	void Initialize(void);
	char* screen;
	int iScreenWidth;
	int iScreenHeight;
	int iScreenByteSize;
	HANDLE hConsole;
};
