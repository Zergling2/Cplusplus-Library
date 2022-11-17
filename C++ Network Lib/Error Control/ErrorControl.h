#pragma once

#include <tchar.h>

void PrintWSADetailErrorAndExit(int _exitCode);

void PrintWSADetailError();

void GetWSADetailError(TCHAR* buf, size_t cbDest);

void GetDetailError(TCHAR* buf, size_t cbDest);

void ExitWithWSAErrorMessageBox(const TCHAR* szMessage, int _exitCode);

void WSAErrorMessageBox(const TCHAR* szMessage);