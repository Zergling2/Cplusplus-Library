#pragma once

#include <string>

enum class LogType
{
	LT_SYSTEM,
	LT_INFO,
	LT_CRITICAL,
	LT_ERROR,
	LT_WARNING,
	LT_FAIL,
	LT_LAST_ERROR,
	LT_NETWORK,
	LT_NO_PREFIX
};

namespace SJAPI
{
	enum class ColorType
	{
		BLACK,  		//  0
		DARK_BLUE,		//  1
		DARK_GREEN,		//  2
		DARK_SKY_BLUE,  //  3
		DARK_RED,  		//  4
		DARK_PURPLE,	//  5
		DARK_YELLOW,	//  6
		GRAY,			//  7
		DARK_GRAY,		//  8
		BLUE,			//  9
		GREEN,			// 10
		SKY_BLUE,		// 11
		RED,			// 12
		PURPLE,			// 13
		YELLOW,			// 14
		WHITE			// 15
	};

	class CFileLogger
	{
	public:
		CFileLogger(const std::wstring& logFileName, bool bAppendTimeToFileName);
		CFileLogger(const wchar_t* logFileName, bool bAppendTimeToFileName);
		bool WriteLog(const std::wstring& logString, LogType type);
		bool WriteLog(const wchar_t* logString, LogType type);
	private:
		void AppendTimeToFileName();
		std::wstring logFileName;
	};

	class CConsoleLogger
	{
	public:
		static void WriteLog(const std::wstring& logString, LogType type);
		static void WriteLog(const wchar_t* logString, LogType type);
	private:
		static ColorType currentColor;
	};
}
