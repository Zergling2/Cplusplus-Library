#include "ConsoleScreen.h"
#include <Windows.h>
#include <stdio.h>

ConsoleScreen::ConsoleScreen(int _ScreenWidth, int _ScreenHeight)
	: iScreenWidth(_ScreenWidth)
	, iScreenHeight(_ScreenHeight)
{
	if (iScreenWidth < 0 || iScreenHeight < 0)
	{
		printf("The screen width or height must be positive integer!\n");
		exit(999);
	}

	screen = new BYTE[iScreenWidth * iScreenHeight];
	Initialize();
}

ConsoleScreen::~ConsoleScreen()
{
	delete[] screen;
}

//-------------------------------------------------------------
// 콘솔 화면을 초기화 한다.
//
//-------------------------------------------------------------
void ConsoleScreen::ClearScreen(void)
{
	COORD c = { 0, 0 };
	DWORD dw;

	FillConsoleOutputCharacter(hConsole, ' ', iScreenByteSize, c, &dw);
}

void ConsoleScreen::BufferClear()
{
	memset(screen, ' ', iScreenByteSize);
}

void ConsoleScreen::PrintScreen()
{
	char* p = this->screen + iScreenWidth - 1;
	for (int i = 0; i < iScreenHeight; i++)
	{
		*p = '\0';
		p += iScreenWidth;
	}

	p = screen;
	for (int i = 0; i < iScreenHeight; i++)
	{
		MoveCursor(0, i);
		printf(p);
		p += iScreenWidth;
	}
}

void ConsoleScreen::DrawSprite(char chSprite, int xPos, int yPos)
{
	if (xPos < 0 || yPos < 0 || xPos >= iScreenWidth - 1 || yPos >= iScreenHeight)
		return;

	char* pSpritePos = screen + (yPos * iScreenWidth) + xPos;
	*pSpritePos = chSprite;
}

//-------------------------------------------------------------
// 콘솔 화면의 커서를 X, Y 좌표로 이동시킨다.
//
//-------------------------------------------------------------
void ConsoleScreen::MoveCursor(short xPos, short yPos)
{
	COORD stCoord = { xPos, yPos };
	//-------------------------------------------------------------
	// 원하는 위치로 커서를 이동시킨다.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(hConsole, stCoord);
}

void ConsoleScreen::Initialize(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	//-------------------------------------------------------------
	// 콘솔화면 (스탠다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
	iScreenByteSize = iScreenWidth * iScreenHeight;
}