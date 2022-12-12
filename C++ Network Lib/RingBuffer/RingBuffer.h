#pragma once

#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_

#ifdef _WIN64
typedef long long RB_RESULT;
#else
typedef int RB_RESULT;
#endif

#define INSUFFICIENT_FREE_SPACE ((RB_RESULT)(-1))

namespace SJNET
{
	namespace LIB
	{
		class CRingBuffer
		{
		public:
			CRingBuffer(size_t size);
			~CRingBuffer();

			// Write 포인터가 Read 바로 뒤에 있으면 꽉 찬 상태
			// Read 포인터랑 Write 포인터 위치가 동일하면 빈 상태
			inline size_t GetBufferSize() const;
			inline size_t GetUsingSize() const;
			inline size_t GetFreeSize() const;

			/////////////////////////////////////////////////////////////////////////
			// 버퍼 포인터로 외부에서 한번에 읽거나 쓸 수 있는 길이.
			// (끊기지 않은 길이)
			//
			// Parameters: 없음.
			// Return: (size_t)사용가능 용량.
			////////////////////////////////////////////////////////////////////////
			inline size_t DirectEnqueueSize() const;
			inline size_t DirectDequeueSize() const;

			/////////////////////////////////////////////////////////////////////////
			// WritePos 에 Data 넣음.
			//
			// Parameters: (void*)Data 포인터. (size_t)크기. 
			// Return: (size_t)넣은 크기.
			/////////////////////////////////////////////////////////////////////////
			RB_RESULT Enqueue(const void* _Src, size_t size);

			/////////////////////////////////////////////////////////////////////////
			// ReadPos 에서 Data 가져옴. ReadPos 이동.
			//
			// Parameters: (void*)Data 포인터. (size_t)크기.
			// Return: (size_t)가져온 크기.
			/////////////////////////////////////////////////////////////////////////
			RB_RESULT Dequeue(void* _Dst, size_t size);

			/////////////////////////////////////////////////////////////////////////
			// ReadPos 에서 Data 읽어옴. ReadPos 고정.
			//
			// Parameters: (void*)Data 포인터. (size_t)크기.
			// Return: (size_t)가져온 크기.
			/////////////////////////////////////////////////////////////////////////
			RB_RESULT Peek(void* _Dst, size_t size);

			inline void MoveReadPointer(size_t offset);
			inline void MoveWritePointer(size_t offset);
			inline char* GetReadPointer() const { return readPointer; }
			inline char* GetWritePointer() const { return writePointer; }
			inline char* GetBufferBeginAddress() const { return bufferBeginAddress; }
			inline void ClearBuffer() { readPointer = writePointer; }
		private:
			size_t bufferSize;
			char* bufferBeginAddress;
			char* bufferEndAddress;
			alignas(64) char* readPointer;
			char* writePointer;
		};

		inline size_t CRingBuffer::GetBufferSize() const
		{
			return bufferSize;
		}

		inline size_t CRingBuffer::GetUsingSize() const
		{
			ptrdiff_t diff = writePointer - readPointer;
			if (diff >= 0)
				return diff;
			else
				return bufferSize + diff + 1;
		}

		inline size_t CRingBuffer::GetFreeSize() const
		{
			ptrdiff_t diff = readPointer - writePointer;
			if (diff > 0)
				return diff - 1;
			else
				return bufferSize + diff;
		}


		inline size_t CRingBuffer::DirectEnqueueSize() const
		{
			ptrdiff_t diff = readPointer - writePointer;
			if (diff <= 0)
			{
				if (readPointer == bufferBeginAddress)
					return bufferEndAddress - writePointer - 1;
				else
					return bufferEndAddress - writePointer;
			}
			else
			{
				return diff - 1;
			}
		}

		inline size_t CRingBuffer::DirectDequeueSize() const
		{
			ptrdiff_t diff = writePointer - readPointer;
			if (diff < 0)
				return bufferEndAddress - readPointer;
			else
				return diff;
		}


		inline void CRingBuffer::MoveReadPointer(size_t offset)
		{
			char* tempPos = readPointer + offset;
			ptrdiff_t diff = tempPos - bufferEndAddress;

			if (diff >= 0)
				readPointer = bufferBeginAddress + diff;
			else
				readPointer = tempPos;
		}

		inline void CRingBuffer::MoveWritePointer(size_t offset)
		{
			char* tempPos = writePointer + offset;
			ptrdiff_t diff = tempPos - bufferEndAddress;

			if (diff >= 0)
				writePointer = bufferBeginAddress + diff;
			else
				writePointer = tempPos;
		}
	}
}
