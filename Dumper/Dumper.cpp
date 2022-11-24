#pragma comment(lib, "Dbghelp.lib")
#include "Dumper.h"
#include <crtdbg.h>
#include <DbgHelp.h>
#include "Logger.h"

CDumper CDumper::_Instance;
long CDumper::lDumpCount = 0;

CDumper::CDumper()
{
	lDumpCount = 0;

	_invalid_parameter_handler hdlOldHandler;
	_invalid_parameter_handler hdlNewHandler;

	hdlNewHandler = CustomInvalidParameterHandler;
	hdlOldHandler = _set_invalid_parameter_handler(hdlNewHandler);
	_CrtSetReportMode(_CRT_WARN, 0);
	_CrtSetReportMode(_CRT_ASSERT, 0);
	_CrtSetReportMode(_CRT_ERROR, 0);

	_CrtSetReportHook(CustomReportHook);

	_set_purecall_handler(CustomPurecallHandler);		// purecall 핸들러를 사용자 정의 함수로 대체한다.

	SetUnhandledExceptionFilter(CustomExceptionFilter);	// 처리되지 않은 예외 필터를 사용자 정의 함수로 대체한다.
}

void CDumper::Crash()
{
	*reinterpret_cast<int*>(0x00000000) = 0;
}

LONG CALLBACK CDumper::CustomExceptionFilter(_In_ PEXCEPTION_POINTERS pExceptionPointer)
{
	int iWorkingMemory = 0;
	SYSTEMTIME stCurrentTime;

	long lDumpCount = InterlockedIncrement(&CDumper::lDumpCount);

	wchar_t szFileName[MAX_PATH];

	GetLocalTime(&stCurrentTime);
	wsprintfW(szFileName, L"Dump_%d%02d%02d_%02dh%02dm%02ds_%d.dmp",
		stCurrentTime.wYear, stCurrentTime.wMonth, stCurrentTime.wDay, stCurrentTime.wHour, stCurrentTime.wMinute, stCurrentTime.wSecond, lDumpCount);

	SJNET::API::CConsoleLogger::WriteLog(L"Crash Error!!!", LogType::LT_CRITICAL);
	SJNET::API::CConsoleLogger::WriteLog(L"Saving dump file...", LogType::LT_SYSTEM);

	HANDLE hDumpFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION stMinidumpExceptionInformation;

		stMinidumpExceptionInformation.ThreadId = GetCurrentThreadId();
		stMinidumpExceptionInformation.ExceptionPointers = pExceptionPointer;
		stMinidumpExceptionInformation.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithFullMemory,
			&stMinidumpExceptionInformation, NULL, NULL);
		CloseHandle(hDumpFile);
		SJNET::API::CConsoleLogger::WriteLog(L"Dump file saved successfully.", LogType::LT_SYSTEM);
	}
	else
	{
		SJNET::API::CConsoleLogger::WriteLog(L"Failed to save dump file!", LogType::LT_FAIL);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void CDumper::CustomInvalidParameterHandler(const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile, unsigned int line, uintptr_t pReserved)
{
	Crash();
}

int CDumper::CustomReportHook(int iRepostType, char* szMessage, int* pReturnValue)
{
	Crash();
	return -1;
}

void CDumper::CustomPurecallHandler()
{
	Crash();
}
