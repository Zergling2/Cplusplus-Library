#pragma once

#define _WINSOCKAPI_
#include <Windows.h>

class CDumper
{
public:
	static void Crash();
	static LONG CALLBACK CustomExceptionFilter(_In_ PEXCEPTION_POINTERS pExceptionPointer);
	static void CustomInvalidParameterHandler(const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile, unsigned int line, uintptr_t pReserved);
	static int CustomReportHook(int iRepostType, char* szMessage, int* pReturnValue);
	static void CustomPurecallHandler();
private:
	static CDumper _Instance;
	static long lDumpCount;
	CDumper();
};

#undef _WINSOCKAPI_