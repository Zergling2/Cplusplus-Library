#include "Time.h"

CTime::CTime()
	: dwDeltaTime(0)
{
	timeBeginPeriod(1);
	dwOldTime = timeGetTime();
}

void CTime::Update()
{
	DWORD dwCurrentTime = timeGetTime();
	dwDeltaTime = dwCurrentTime - dwOldTime;
	dwOldTime = dwCurrentTime;
}