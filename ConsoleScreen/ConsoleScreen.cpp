#include "ConsoleScreen.h"

void ConsoleScreen::ConsoleInitialize(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// ȭ���� Ŀ���� �Ⱥ��̰Բ� �����Ѵ�.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	//-------------------------------------------------------------
	// �ܼ�ȭ�� (���ٴٵ� �ƿ�ǲ) �ڵ��� ���Ѵ�.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
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

//-------------------------------------------------------------
// �ܼ� ȭ���� �ʱ�ȭ �Ѵ�.
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
