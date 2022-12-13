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
// �ܼ� ȭ���� �ʱ�ȭ �Ѵ�.
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
// �ܼ� ȭ���� Ŀ���� X, Y ��ǥ�� �̵���Ų��.
//
//-------------------------------------------------------------
void ConsoleScreen::MoveCursor(short xPos, short yPos)
{
	COORD stCoord = { xPos, yPos };
	//-------------------------------------------------------------
	// ���ϴ� ��ġ�� Ŀ���� �̵���Ų��.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(hConsole, stCoord);
}

void ConsoleScreen::Initialize(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// ȭ���� Ŀ���� �Ⱥ��̰Բ� �����Ѵ�.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	//-------------------------------------------------------------
	// �ܼ�ȭ�� (���Ĵٵ� �ƿ�ǲ) �ڵ��� ���Ѵ�.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
	iScreenByteSize = iScreenWidth * iScreenHeight;
}