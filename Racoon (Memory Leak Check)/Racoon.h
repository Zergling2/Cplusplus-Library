#ifndef __RACOON_MEMORY_LEAK_DETECTION_H__
#define __RACOON_MEMORY_LEAK_DETECTION_H__

#if defined _UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif

#define MAX_PATH 260
#define LOG_LENGTH 400

enum class LogType
{
	NOALLOC = 0,
	ARRAY,
	LEAK
};

struct AllocInfo
{
public:
	void* mallocStartPtr;
	int size;
	TCHAR fileName[MAX_PATH];
	int lineNumber;
	bool isArray;
	AllocInfo* next;
	AllocInfo* prev;
};

class Racoon
{
public:
	inline static Racoon& GetInstance();
	bool Add(AllocInfo* ai);					// Do Not use this function
	bool Delete(void* p, bool deleteArray);		// Do Not use this function
	void Record(void* arg, LogType logType);
private:
	Racoon();
	~Racoon();
	static Racoon instance;
	AllocInfo* front;
	AllocInfo* rear;
	TCHAR logFileName[30];
};

void* operator new(size_t size, const TCHAR* fileName, int lineNumber);

void* operator new[](size_t size, const TCHAR* fileName, int lineNumber);

void operator delete(void* p);

void operator delete[](void* p);

void operator delete(void* p, const TCHAR* fileName, int lineNumber);

void operator delete[](void* p, const TCHAR* fileName, int lineNumber);

#if defined _UNICODE
	#define new new(__FILEW__, __LINE__)
#else
	#define new new(__FILE__, __LINE__)
#endif

#endif // !__RACOON_MEMORY_LEAK_DETECTION_H__
