#include "Racoon.h"

#include <iostream>
#include <stdlib.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>

#undef new	// Remove the define statement of 'new' created in 'Racoon.h' file.

const TCHAR noAllocMsg[] = _T("NOALLOC");
const TCHAR arrayMsg[] = _T("ARRAY");
const TCHAR leakMsg[] = _T("LEAK");

Racoon Racoon::instance;

Racoon::Racoon()
{
	_tsetlocale(LC_ALL, _T("ko-kr"));
	time_t base_time = time(NULL);
	tm base_date_local;
	errno_t e = localtime_s(&base_date_local, &base_time);

	// 더미노드 생성
	this->front = reinterpret_cast<AllocInfo*>(malloc(sizeof(AllocInfo)));
	this->rear = reinterpret_cast<AllocInfo*>(malloc(sizeof(AllocInfo)));

	this->front->next = this->rear;
	this->front->prev = nullptr;
	this->rear->prev = this->front;
	this->rear->next = nullptr;

	StringCbPrintf(logFileName, sizeof(logFileName), _T("Alloc_%04d%02d%02d_%02d%02d%02d.txt"),
		base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
		base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
}

Racoon::~Racoon()
{
	AllocInfo* cursor = front->next;
	AllocInfo* temp;

	// 앞쪽의 더미 노드 해제
	free(front);
	while (cursor != rear)
	{
		temp = cursor->next;
		Record(cursor, LogType::LEAK);
		free(cursor);
		cursor = temp;
	}

	// 뒤쪽 더미 노드 해제
	free(rear);
}

inline Racoon& Racoon::GetInstance()
{
	return Racoon::instance;
}

bool Racoon::Add(AllocInfo* ai)
{
	ai->prev = rear->prev;
	ai->prev->next = ai;
	ai->next = rear;
	rear->prev = ai;

	return true;
}

bool Racoon::Delete(void* ptr, bool deleteArray)
{
	// 사용자가 올바르게 해제했을 시.
	// 리스트에서 ptr 또는 ptr - 4(8)를 갖는 구조체를 찾은 후 제거한다.
	int offset;
#if defined(_WIN64)
	offset = 8;
#else
	offset = 4;
#endif

	char* p1 = reinterpret_cast<char*>(ptr);	// 소멸자가 없는 클래스의 경우 이 값으로 찾아질 것이다.
	char* p2 = p1 - offset;				// 소멸자가 있는 클래스가 new[]로 할당되었을 경우 이 값으로 찾아질 것이다.
	void* msp;
	AllocInfo* cursor = this->front->next;
	bool find = false;

	while (cursor != this->rear)
	{
		msp = cursor->mallocStartPtr;
		if (msp == reinterpret_cast<void*>(p1) || msp == reinterpret_cast<void*>(p2))
		{
			if (((deleteArray == false) && (cursor->isArray == true)) || ((deleteArray == true) && (cursor->isArray == false)))
			{
				// delete 함수로 요청했는데 사실 new[]로 할당되었던 경우
				// 또는
				// delete[] 함수로 요청받았는데 사실 new로 할당되었던 경우
				// 로그 기록
				Record(cursor, LogType::ARRAY);
			}

			free(cursor->mallocStartPtr);

			// 해당 노드 삭제
			cursor->prev->next = cursor->next;
			cursor->next->prev = cursor->prev;
			free(cursor);

			find = true;
			break;
		}
		cursor = cursor->next;
	}

	if (!find)
	{
		// 찾지 못한 경우
		// NOALLOC 로그 남기기
		Record(ptr, LogType::NOALLOC);
	}

	return true; // 로그 기록했음을 알림
}

void Racoon::Record(void* arg, LogType logType)
{
	FILE* fp;
	TCHAR logMsg[LOG_LENGTH];
	AllocInfo* ai = reinterpret_cast<AllocInfo*>(arg);

	switch (logType)
	{
	case LogType::NOALLOC:
		StringCbPrintf(logMsg, sizeof(logMsg), _T("%s\t[0x%p]\n"), noAllocMsg, arg);
		break;
	case LogType::ARRAY:
		StringCbPrintf(logMsg, sizeof(logMsg), _T("%s\t[0x%p] [%d] %s : %d\n"), arrayMsg, ai->mallocStartPtr, ai->size, ai->fileName, ai->lineNumber);
		break;
	case LogType::LEAK:
		StringCbPrintf(logMsg, sizeof(logMsg), _T("%s\t[0x%p] [%d] %s : %d\n"), leakMsg, ai->mallocStartPtr, ai->size, ai->fileName, ai->lineNumber);
		break;
	}

	errno_t e = _tfopen_s(&fp, logFileName, _T("at"));
	if (e == 0)
	{
#if defined _UNICODE
		fputws(logMsg, fp);
#else
		fputs(logMsg, fp);
#endif
		fclose(fp);
	}
	else
	{
		std::cout << "로그 파일 생성 실패!" << std::endl;
	}
}

void* operator new(size_t size, const TCHAR* fileName, int lineNumber)
{
	void* p = malloc(size);
	if (p == nullptr)
		return nullptr;

	AllocInfo* newAllocInfo = reinterpret_cast<AllocInfo*>(malloc(sizeof(AllocInfo)));
	if (newAllocInfo == nullptr)
	{
		free(p);
		int* p_Critical = reinterpret_cast<int*>(0x00000443);
		*p_Critical = 0x00000443;
		return nullptr;
	}

	newAllocInfo->mallocStartPtr = p;
	newAllocInfo->size = static_cast<int>(size);
	newAllocInfo->lineNumber = lineNumber;
	newAllocInfo->isArray = false;
	_tcscpy_s(newAllocInfo->fileName, sizeof(newAllocInfo->fileName) / sizeof(TCHAR), fileName);

	Racoon::GetInstance().Add(newAllocInfo);

	return p;
}

void* operator new[](size_t size, const TCHAR* fileName, int lineNumber)
{
	void* p = malloc(size);
	if (p == nullptr)
		return nullptr;

	AllocInfo* newAllocInfo = reinterpret_cast<AllocInfo*>(malloc(sizeof(AllocInfo)));
	if (newAllocInfo == nullptr)
	{
		free(p);
		int* p_Critical = reinterpret_cast<int*>(0x00000443);
		*p_Critical = 0x00000443;
		return nullptr;
	}

	newAllocInfo->mallocStartPtr = p;
	newAllocInfo->size = static_cast<int>(size);
	newAllocInfo->lineNumber = lineNumber;
	newAllocInfo->isArray = true;
	_tcscpy_s(newAllocInfo->fileName, sizeof(newAllocInfo->fileName) / sizeof(TCHAR), fileName);

	Racoon::GetInstance().Add(newAllocInfo);

	return p;
}

void operator delete(void* p)
{
	Racoon::GetInstance().Delete(p, false);
}

void operator delete[](void* p)
{
	Racoon::GetInstance().Delete(p, true);
}

void operator delete(void* p, const TCHAR* fileName, int lineNumber)
{

}

void operator delete[](void* p, const TCHAR* fileName, int lineNumber)
{

}