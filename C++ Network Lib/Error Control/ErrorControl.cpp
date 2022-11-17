#include "ErrorControl.h"

#include <WinSock2.h>
#include <stdio.h>
#include <strsafe.h>

void PrintWSADetailErrorAndExit(int _exitCode)
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	_tprintf(_T("[%d] %s"), WSAGetLastError(), (LPCTSTR)lpMsgBuffer);
	LocalFree(lpMsgBuffer);
	exit(_exitCode);
}

void PrintWSADetailError()
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	_tprintf(_T("[%d] %s"), WSAGetLastError(), (LPCTSTR)lpMsgBuffer);
	LocalFree(lpMsgBuffer);
}

void GetWSADetailError(TCHAR* buf, size_t cbDest)
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	StringCbPrintf(buf, cbDest, _T("Error Code: %d, %s"), WSAGetLastError(), (LPCTSTR)lpMsgBuffer);
	buf[_tcslen(buf) - 2] = _T('\0');
	LocalFree(lpMsgBuffer);
}

void GetDetailError(TCHAR* buf, size_t cbDest)
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	StringCbPrintf(buf, cbDest, _T("Error Code: %d, %s"), GetLastError(), (LPCTSTR)lpMsgBuffer);
	buf[_tcslen(buf) - 2] = _T('\0');
	LocalFree(lpMsgBuffer);
}

void ExitWithWSAErrorMessageBox(const TCHAR* szMessage, int _exitCode)
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuffer, szMessage, MB_ICONERROR);
	LocalFree(lpMsgBuffer);
	exit(_exitCode);
}

void WSAErrorMessageBox(const TCHAR* szMessage)
{
	LPVOID lpMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuffer, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuffer, szMessage, MB_ICONERROR);
	LocalFree(lpMsgBuffer);
}