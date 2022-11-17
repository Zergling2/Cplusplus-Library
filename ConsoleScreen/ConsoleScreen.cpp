#include "ConsoleScreen.h"

void ConsoleScreen::ConsoleInitialize(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	//-------------------------------------------------------------
	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
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

//-------------------------------------------------------------
// 콘솔 화면을 초기화 한다.
//
//-------------------------------------------------------------
void ConsoleScreen::ClearScreen(void)
{
	COORD c = { 0, 0 };
	DWORD dw;

	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 100 * 100, c, &dw);
}

ConsoleScreen::ConsoleScreen(unsigned int dfScreenWidth, unsigned int dfScreenHeight) :
	dfScreenWidth(dfScreenWidth), dfScreenHeight(dfScreenHeight)
{
	screen = new char[dfScreenWidth * dfScreenHeight];
	ConsoleInitialize();
}

ConsoleScreen::~ConsoleScreen()
{
	delete[] screen;
}

void ConsoleScreen::BufferClear()
{
	memset(screen, ' ', dfScreenWidth * dfScreenHeight);
}

void ConsoleScreen::PrintScreen()
{
	char* p = this->screen + dfScreenWidth - 1;
	for (int i = 0; i < dfScreenHeight; i++)
	{
		*p = '\0';
		p += dfScreenWidth;
	}

	p = screen;
	for (int i = 0; i < dfScreenHeight; i++)
	{
		MoveCursor(0, i);
		printf(p);
		p += dfScreenWidth;
	}
}

void ConsoleScreen::DrawSprite(char chSprite, int xPos, int yPos)
{
	if (xPos < 0 || yPos < 0 || xPos >= dfScreenWidth - 1 || yPos >= dfScreenHeight)
		return;

	char* p = screen + (yPos * dfScreenWidth) + xPos;
	*p = chSprite;
}
