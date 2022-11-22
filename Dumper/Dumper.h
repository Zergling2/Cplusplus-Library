#pragma once

#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_

class CDumper
{
public:
	static void Crash();
private:
	static LONG CALLBACK CustomExceptionFilter(_In_ PEXCEPTION_POINTERS pExceptionPointer);
	static void CustomInvalidParameterHandler(const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile, unsigned int line, uintptr_t pReserved);
	static int CustomReportHook(int iRepostType, char* szMessage, int* pReturnValue);
	static void CustomPurecallHandler();
	static CDumper _Instance;
	static long lDumpCount;
	CDumper();
};
