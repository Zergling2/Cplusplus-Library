// Recommend using C++ 17 or later.
#pragma once

#pragma warning(disable:26495)

#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_
#include <strsafe.h>
#include "Logger.h"
#include "ForceCrash.h"

// ---------------- Option -------------------
// constexpr DWORD flOptions = 0;
// constexpr SIZE_T dwInitialSize = 4096 * 16;
// constexpr SIZE_T dwMaximumSize = 0;
// -------------------------------------------

class CSession;

enum class DestructorCallOption
{
	RETURNED_TO_POOL,
	FROM_POOL_DESTRUCTOR
};

namespace SJNET
{
	namespace LIB
	{
		template<typename ObjectType, DestructorCallOption opt>
		class CMemoryPool
		{
		private:
			struct Node
			{
			public:
				template<typename... Types> Node(Types ...args);
				// void* operator new(size_t size);			// deprecated
				// void operator delete(void* _Block);		// deprecated
				ObjectType object;
				CMemoryPool<ObjectType, opt>* pSource;
				Node* below;
			};
		public:
			CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (경고!) dwMaximumSize가 0이 아닌 경우 LFH 적용 불가 및 해당 힙에서의 객체 최대 할당 크기는 0x7FFF8로 제한됨.
			~CMemoryPool();
			template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
			inline size_t GetAllocCount() { return this->_AllocationCount; }
			inline size_t GetFreeObjectCount() { return this->_FreeObjectCount; }
		private:
			void GetObjectErrorCrash();
			void ReturnObjectErrorCrash(CMemoryPool<ObjectType, opt>* address);
			Node* _Top;
			SRWLOCK stSRWLock;
			HANDLE hHeapHandle;
			DWORD dwHeapOptions;
			size_t _AllocationCount;
			size_t _FreeObjectCount;
		};

		template<typename ObjectType, DestructorCallOption opt>
		CMemoryPool<ObjectType, opt>::CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
			, hHeapHandle(HeapCreate(flOptions, dwInitialSize, dwMaximumSize))
			, dwHeapOptions(flOptions)
			, _AllocationCount(0)
			, _FreeObjectCount(0)
		{
			InitializeSRWLock(&this->stSRWLock);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline CMemoryPool<ObjectType, opt>::~CMemoryPool()
		{
			if constexpr (opt == DestructorCallOption::FROM_POOL_DESTRUCTOR)
			{
				AcquireSRWLockExclusive(&this->stSRWLock);

				Node* pCursor = this->_Top;
				Node* pTemp;
				while (pCursor != nullptr)
				{
					pTemp = pCursor->below;
					HeapFree(this->hHeapHandle, this->dwHeapOptions, pCursor);
					pCursor = pTemp;
				}

				ReleaseSRWLockExclusive(&this->stSRWLock);
			}

			HeapDestroy(this->hHeapHandle);
		}

		template<typename ObjectType, DestructorCallOption opt>
		template<typename ...Types>
		inline ObjectType* CMemoryPool<ObjectType, opt>::GetObjectFromPool(Types ...args)
		{
			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* pNode = nullptr;		// 최대 최적화 (속도 우선) 사용 시 nullptr 대입 코드 삭제 가능.

			if (!(this->_Top))
			{
				_AllocationCount++;
				ReleaseSRWLockExclusive(&this->stSRWLock);
				try
				{
					pNode = reinterpret_cast<Node*>(HeapAlloc(this->hHeapHandle, this->dwHeapOptions, sizeof(CMemoryPool<ObjectType, opt>::Node)));
					new (pNode) ObjectType(args...);		// placement new
					pNode->pSource = this;
				}
				catch (std::bad_alloc& e)
				{
					UNREFERENCED_PARAMETER(e);
					CMemoryPool<ObjectType, opt>::GetObjectErrorCrash();
				}
			}
			else
			{
				_FreeObjectCount--;
				pNode = this->_Top;
				this->_Top = this->_Top->below;
				ReleaseSRWLockExclusive(&this->stSRWLock);
				new (pNode) ObjectType(args...);		// placement new
			}

			return reinterpret_cast<ObjectType*>(pNode);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::ReturnObjectToPool(ObjectType* pObj)
		{
			if (reinterpret_cast<Node*>(pObj)->pSource != this)
				CMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(pObj)->pSource);

			if constexpr (opt == DestructorCallOption::RETURNED_TO_POOL)
				pObj->~ObjectType();

			AcquireSRWLockExclusive(&this->stSRWLock);
			_FreeObjectCount++;
			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(pObj);
			this->_Top->below = prevTop;
			ReleaseSRWLockExclusive(&this->stSRWLock);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::GetObjectErrorCrash()
		{
			SJNET::API::CFileLogger fLogger(L"CMemoryPool_MT Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] std::bad_alloc exception occurred. Pool address is %p", this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(CMemoryPool<ObjectType, opt>* address)
		{
			SJNET::API::CFileLogger fLogger(L"CMemoryPool_MT Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] An object created from another pool(%p) has been returned to the pool located at address %p.", address, this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType, DestructorCallOption opt>
		template<typename ...Types>
		inline CMemoryPool<ObjectType, opt>::Node::Node(Types ...args)
			: object(args...)
		{
		}
	}
}

#pragma warning(default:26495)
