#include "CLogger.h"

#include <Windows.h>
#include <time.h>
#include <strsafe.h>
#pragma warning(disable : 6387)

SJNET::LIB::ColorType SJNET::LIB::CConsoleLogger::currentColor = SJNET::LIB::ColorType::DARK_GRAY;

SJNET::LIB::CFileLogger::CFileLogger(const std::wstring& logFileName, bool bAppendTimeToFileName) :
	logFileName(logFileName)
{
	if (bAppendTimeToFileName)
		AppendTimeToFileName();
}

SJNET::LIB::CFileLogger::CFileLogger(const wchar_t* logFileName, bool bAppendTimeToFileName) :
	logFileName(logFileName)
{
	if (bAppendTimeToFileName)
		AppendTimeToFileName();
}

bool SJNET::LIB::CFileLogger::WriteLog(const std::wstring& logString, LogType type)
{
	return WriteLog(logString.c_str(), type);
}

bool SJNET::LIB::CFileLogger::WriteLog(const wchar_t* logString, LogType type)
{
	FILE* logFileStream;
	errno_t e = _wfopen_s(&logFileStream, logFileName.c_str(), L"at");

	if (e != 0)
		return false;

	switch (type)
	{
	case LogType::LT_NONE:
		fwprintf(logFileStream, L"[NONE] %s\n", logString);
		break;
	case LogType::LT_INFO:
		fwprintf(logFileStream, L"[INFO] %s\n", logString);
		break;
	case LogType::LT_CRITICAL:
		fwprintf(logFileStream, L"[CRITICAL] %s\n", logString);
		break;
	case LogType::LT_ERROR:
		fwprintf(logFileStream, L"[ERROR] %s\n", logString);
		break;
	case LogType::LT_WARNING:
		fwprintf(logFileStream, L"[WARNING] %s\n", logString);
		break;
	case LogType::LT_FAIL:
		fwprintf(logFileStream, L"[FAIL] %s\n", logString);
		break;
	case LogType::LT_LAST_ERROR:
		fwprintf(logFileStream, L"[LAST_ERROR] %s\n", logString);
		break;
	case LogType::LT_NETWORK:
		fwprintf(logFileStream, L"[NETWORK I/O] %s\n", logString);
		break;
	default:
		fwprintf(logFileStream, L"[UNDEFINED] %s\n", logString);
		break;
	}

	fclose(logFileStream);

	return true;
}

void SJNET::LIB::CFileLogger::AppendTimeToFileName()
{
	time_t base_time = time(NULL);
	tm base_date_local;
	errno_t e = localtime_s(&base_date_local, &base_time);
	wchar_t buffer[50];

	StringCbPrintfW(buffer, sizeof(buffer), L"%04d%02d%02d_%02d%02d%02d.txt",
		base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
		base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);

	logFileName.append(buffer);
}

void SJNET::LIB::CConsoleLogger::WriteLog(const std::wstring& logString, LogType type)
{
	SJNET::LIB::CConsoleLogger::WriteLog(logString.c_str(), type);
}

void SJNET::LIB::CConsoleLogger::WriteLog(const wchar_t* logString, LogType type)
{
	switch (type)
	{
	case LogType::LT_NONE:
		if (currentColor != ColorType::GRAY)
		{
			currentColor = ColorType::GRAY;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[NONE] %s\n", logString);
		break;
	case LogType::LT_INFO:
		if (currentColor != ColorType::GREEN)
		{
			currentColor = ColorType::GREEN;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[INFO] %s\n", logString);
		break;
	case LogType::LT_CRITICAL:
		if (currentColor != ColorType::DARK_RED)
		{
			currentColor = ColorType::DARK_RED;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[CRITICAL] %s\n", logString);
		break;
	case LogType::LT_ERROR:
		if (currentColor != ColorType::DARK_YELLOW)
		{
			currentColor = ColorType::DARK_YELLOW;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[ERROR] %s\n", logString);
		break;
	case LogType::LT_WARNING:
		if (currentColor != ColorType::RED)
		{
			currentColor = ColorType::RED;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[WARNING] %s\n", logString);
		break;
	case LogType::LT_FAIL:
		if (currentColor != ColorType::PURPLE)
		{
			currentColor = ColorType::PURPLE;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[FAIL] %s\n", logString);
		break;
	case LogType::LT_LAST_ERROR:
		if (currentColor != ColorType::DARK_BLUE)
		{
			currentColor = ColorType::DARK_BLUE;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[LAST_ERROR] %s\n", logString);
		break;
	case LogType::LT_NETWORK:
		if (currentColor != ColorType::SKY_BLUE)
		{
			currentColor = ColorType::SKY_BLUE;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[NETWORK I/O] %s\n", logString);
		break;
	default:
		if (currentColor != ColorType::WHITE)
		{
			currentColor = ColorType::WHITE;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(currentColor));
		}
		wprintf(L"[UNDEFINED] %s\n", logString);
		break;
	}
}

#pragma warning(default : 6387)