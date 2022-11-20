#include "RingBuffer.h"
#include "Logger.h"
#include "ForceCrash.h"

using namespace SJNET::LIB;

CRingBuffer::CRingBuffer(size_t size)
	: bufferSize(size)
{
	bufferBeginAddress = new(std::nothrow) char[size + 1];

	if (!bufferBeginAddress)
	{
		SJAPI::CFileLogger fLogger(L"CRingBuffer_Error_", true);
		fLogger.WriteLog(L"std::bad_alloc exception!", LogType::LT_CRITICAL);

		ForceCrash(0x78563412);
	}

	bufferEndAddress = bufferBeginAddress + size + 1;
	readPointer = bufferBeginAddress;
	writePointer = bufferBeginAddress;
}

CRingBuffer::~CRingBuffer()
{
	delete[] bufferBeginAddress;
}

RB_RESULT CRingBuffer::Enqueue(const void* _Src, size_t size)
{
	size_t des;
	size_t freeSize;

	ptrdiff_t diff = readPointer - writePointer;
	if (diff <= 0)
	{
		if (readPointer == bufferBeginAddress)
			des = bufferEndAddress - writePointer - 1;
		else
			des = bufferEndAddress - writePointer;
		freeSize = bufferSize + diff;
	}
	else
	{
		freeSize = des = diff - 1;
	}

	if (freeSize < size)
		return INSUFFICIENT_FREE_SPACE;

	if (des >= size)
	{
		memmove(writePointer, _Src, size);
	}
	else
	{
		memmove(writePointer, _Src, des);
		memmove(bufferBeginAddress, reinterpret_cast<const char*>(_Src) + des, size - des);
	}

	MoveWritePointer(size);
	return size;
}

RB_RESULT CRingBuffer::Dequeue(void* _Dst, size_t size)
{
	size_t dds;
	size_t usingSize;

	ptrdiff_t diff = writePointer - readPointer;
	if (diff >= 0)
	{
		dds = usingSize = diff;
	}
	else
	{
		dds = bufferEndAddress - readPointer;
		usingSize = bufferSize + diff + 1;
	}

	if (usingSize < size)
		return INSUFFICIENT_DATA_IN_BUFFER;

	if (dds >= size)
	{
		memmove(_Dst, readPointer, size);
	}
	else
	{
		memmove(_Dst, readPointer, dds);
		memmove(reinterpret_cast<char*>(_Dst) + dds, bufferBeginAddress, size - dds);
	}

	MoveReadPointer(size);
	return size;
}

RB_RESULT CRingBuffer::Peek(void* _Dst, size_t size)
{
	size_t dds;
	size_t usingSize;

	ptrdiff_t diff = writePointer - readPointer;
	if (diff >= 0)
	{
		dds = usingSize = diff;
	}
	else
	{
		dds = bufferEndAddress - readPointer;
		usingSize = bufferSize + diff + 1;
	}

	if (usingSize < size)
		return INSUFFICIENT_DATA_IN_BUFFER;

	if (dds >= size)
	{
		memmove(_Dst, readPointer, size);
	}
	else
	{
		memmove(_Dst, readPointer, dds);
		memmove(reinterpret_cast<char*>(_Dst) + dds, bufferBeginAddress, size - dds);
	}

	return size;
}