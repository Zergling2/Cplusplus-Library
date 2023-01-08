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

enum class SRWLMPOption
{
	RETURNED_TO_POOL,
	FROM_POOL_DESTRUCTOR
};

namespace SJNET
{
	namespace LIB
	{
		template<typename ObjectType, SRWLMPOption opt>
		class CSRWLMemoryPool
		{
		private:
			struct Node
			{
			public:
				template<typename... Types> Node(Types ...args);
				// void* operator new(size_t size);			// deprecated
				// void operator delete(void* _Block);		// deprecated
				ObjectType object;
				CSRWLMemoryPool<ObjectType, opt>* pSource;
				Node* below;
			};
		public:
			CSRWLMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (경고!) dwMaximumSize가 0이 아닌 경우 LFH 적용 불가 및 해당 힙에서의 객체 최대 할당 크기는 0x7FFF8로 제한됨.
			~CSRWLMemoryPool();
			template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
		private:
			void GetObjectErrorCrash();
			void ReturnObjectErrorCrash(CSRWLMemoryPool<ObjectType, opt>* address);
			Node* _Top;
			SRWLOCK stSRWLock;
			HANDLE hHeapHandle;
			DWORD dwHeapOptions;
		};

		template<typename ObjectType, SRWLMPOption opt>
		CSRWLMemoryPool<ObjectType, opt>::CSRWLMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
			, hHeapHandle(HeapCreate(flOptions, dwInitialSize, dwMaximumSize))
			, dwHeapOptions(flOptions)
		{
			InitializeSRWLock(&this->stSRWLock);
		}

		template<typename ObjectType, SRWLMPOption opt>
		inline CSRWLMemoryPool<ObjectType, opt>::~CSRWLMemoryPool()
		{
			if constexpr (opt == SRWLMPOption::FROM_POOL_DESTRUCTOR)
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

		template<typename ObjectType, SRWLMPOption opt>
		template<typename ...Types>
		inline ObjectType* CSRWLMemoryPool<ObjectType, opt>::GetObjectFromPool(Types ...args)
		{
			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* pNode;

			if (!(this->_Top))
			{
				ReleaseSRWLockExclusive(&this->stSRWLock);
				pNode = reinterpret_cast<Node*>(HeapAlloc(this->hHeapHandle, this->dwHeapOptions, sizeof(CSRWLMemoryPool<ObjectType, opt>::Node)));
				if (pNode == NULL)
					CSRWLMemoryPool<ObjectType, opt>::GetObjectErrorCrash();
				new (pNode) ObjectType(args...);		// placement new
				pNode->pSource = this;
			}
			else
			{
				pNode = this->_Top;
				this->_Top = this->_Top->below;
				ReleaseSRWLockExclusive(&this->stSRWLock);
				new (pNode) ObjectType(args...);		// placement new
			}

			return reinterpret_cast<ObjectType*>(pNode);
		}

		template<typename ObjectType, SRWLMPOption opt>
		inline void CSRWLMemoryPool<ObjectType, opt>::ReturnObjectToPool(ObjectType* pObj)
		{
			if (reinterpret_cast<Node*>(pObj)->pSource != this)
				CSRWLMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(pObj)->pSource);

			if constexpr (opt == SRWLMPOption::RETURNED_TO_POOL)
				pObj->~ObjectType();

			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(pObj);
			this->_Top->below = prevTop;
			ReleaseSRWLockExclusive(&this->stSRWLock);
		}

		template<typename ObjectType, SRWLMPOption opt>
		inline void CSRWLMemoryPool<ObjectType, opt>::GetObjectErrorCrash()
		{
			SJNET::API::CFileLogger fLogger(L"CSRWLMemoryPool Error_", true);
			wchar_t logBuffer[256];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[SRWL MemPool Error] std::bad_alloc exception occurred. Pool address is 0x%p", this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xDEADBEEF);
		}

		template<typename ObjectType, SRWLMPOption opt>
		inline void CSRWLMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(CSRWLMemoryPool<ObjectType, opt>* address)
		{
			SJNET::API::CFileLogger fLogger(L"CSRWLMemoryPool Error_", true);
			wchar_t logBuffer[256];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[SRWL MemPool Error] An object created from another pool(0x%p) has been returned to the pool located at address 0x%p.", address, this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xDEADBEEF);
		}

		template<typename ObjectType, SRWLMPOption opt>
		template<typename ...Types>
		inline CSRWLMemoryPool<ObjectType, opt>::Node::Node(Types ...args)
			: object(args...)
		{
		}
	}
}

#pragma warning(default:26495)
