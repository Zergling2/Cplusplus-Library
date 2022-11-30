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
	WHEN_RETURNED_TO_POOL,
	WHEN_MEMORY_POOL_IS_DESTROYED
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
			CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (���!) dwMaximumSize�� 0�� �ƴ� ��� LFH ���� �Ұ� �� �ش� �������� ��ü �ִ� �Ҵ� ũ��� 0x7FFF8�� ���ѵ�.
			~CMemoryPool();
			template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
		private:
			void GetObjectErrorCrash();
			void ReturnObjectErrorCrash(CMemoryPool<ObjectType, opt>* address);
			Node* _Top;
			SRWLOCK stSRWLock;
			HANDLE hHeapHandle;
			DWORD dwHeapOptions;
		};

		template<typename ObjectType, DestructorCallOption opt>
		template<typename ...Types>
		inline ObjectType* CMemoryPool<ObjectType, opt>::GetObjectFromPool(Types ...args)
		{
			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* pTmp = nullptr;		// �ִ� ����ȭ (�ӵ� �켱) ��� �� nullptr ���� �ڵ� ���� ����.

			if (!(this->_Top))
			{
				ReleaseSRWLockExclusive(&this->stSRWLock);
				try
				{
					pTmp = reinterpret_cast<Node*>(HeapAlloc(this->hHeapHandle, this->dwHeapOptions, sizeof(CMemoryPool<ObjectType, opt>::Node)));
					new (pTmp) ObjectType(args...);		// placement new
					pTmp->pSource = this;
				}
				catch (std::bad_alloc& e)
				{
					UNREFERENCED_PARAMETER(e);
					CMemoryPool<ObjectType, opt>::GetObjectErrorCrash();
				}
				return reinterpret_cast<ObjectType*>(pTmp);
			}
			else
			{
				pTmp = this->_Top;
				this->_Top = this->_Top->below;
				ReleaseSRWLockExclusive(&this->stSRWLock);
				new (pTmp) ObjectType(args...);		// placement new
				return reinterpret_cast<ObjectType*>(pTmp);
			}
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::ReturnObjectToPool(ObjectType* _Ret)
		{
			if (reinterpret_cast<Node*>(_Ret)->pSource != this)
				CMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(_Ret)->pSource);

			if constexpr (opt == DestructorCallOption::WHEN_RETURNED_TO_POOL)
				_Ret->~ObjectType();

			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(_Ret);
			this->_Top->below = prevTop;
			ReleaseSRWLockExclusive(&this->stSRWLock);
		}

		template<typename ObjectType, DestructorCallOption opt>
		CMemoryPool<ObjectType, opt>::CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
		{
			InitializeSRWLock(&this->stSRWLock);
			this->hHeapHandle = HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
			this->dwHeapOptions = flOptions;
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline CMemoryPool<ObjectType, opt>::~CMemoryPool()
		{
			if constexpr (opt == DestructorCallOption::WHEN_MEMORY_POOL_IS_DESTROYED)
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
