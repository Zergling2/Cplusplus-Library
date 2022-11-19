#pragma once

#pragma warning(disable:26495)

#define _WINSOCKAPI_
#include <Windows.h>
#include <strsafe.h>
#include "Logger.h"
#include "ForceCrash.h"

// ---------------- Option -------------------
constexpr DWORD flOptions = 0;
constexpr SIZE_T dwInitialSize = 4096 * 16;
constexpr SIZE_T dwMaximumSize = 0;
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
			struct Node;
		public:
			static CMemoryPool* GetInstance();
			static void Destroy();
			template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
		private:
			CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (경고!) dwMaximumSize가 0이 아닌 경우 LFH 적용 불가 및 해당 힙에서의 객체 최대 할당 크기는 0x7FFF8로 제한됨.
			~CMemoryPool();
			static void GetObjectErrorCrash();
			static void ReturnObjectErrorCrash(ULONG_PTR llWrongVerificationCode);
			Node* _Top;
			static CMemoryPool<ObjectType, opt>* pInstance;
			static ULONG_PTR _VerificationCode;
			static HANDLE _HeapHandle;
			static DWORD _HeapOptions;
		private:
			struct Node
			{
			public:
				template<typename... Types> Node(Types ...args);
				~Node();
				void* operator new(size_t size);
				void operator delete(void* _Block);
				ObjectType object;
				ULONG_PTR verificationCode;
				Node* below;
			};
		};


		template<typename ObjectType, DestructorCallOption opt>
		inline CMemoryPool<ObjectType, opt>* CMemoryPool<ObjectType, opt>::GetInstance()
		{
			if (CMemoryPool<ObjectType, opt>::pInstance == nullptr)
			{
				CMemoryPool<ObjectType, opt>::pInstance = new CMemoryPool<ObjectType, opt>(flOptions, dwInitialSize, dwMaximumSize);
				atexit(CMemoryPool<ObjectType, opt>::Destroy);
			}

			return CMemoryPool<ObjectType, opt>::pInstance;
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::Destroy()
		{
			delete CMemoryPool<ObjectType, opt>::pInstance;
			CMemoryPool<ObjectType, opt>::pInstance = nullptr;
		}

		template<typename ObjectType, DestructorCallOption opt>
		template<typename ...Types>
		inline ObjectType* CMemoryPool<ObjectType, opt>::GetObjectFromPool(Types ...args)
		{
			Node* pTmp = nullptr;		// 최대 최적화 (속도 우선) 사용 시 nullptr 대입 코드 삭제 가능.

			if (!(this->_Top))
			{
				try
				{
					pTmp = new Node(args...);
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
				new (pTmp) ObjectType(args...);		// placement new
				return reinterpret_cast<ObjectType*>(pTmp);
			}
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::ReturnObjectToPool(ObjectType* _Ret)
		{
			if (reinterpret_cast<Node*>(_Ret)->verificationCode != this->_VerificationCode)
				CMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(_Ret)->verificationCode);

			if constexpr (opt == DestructorCallOption::WHEN_RETURNED_TO_POOL)
				_Ret->~ObjectType();

			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(_Ret);
			this->_Top->below = prevTop;
		}

		template<typename ObjectType, DestructorCallOption opt>
		CMemoryPool<ObjectType, opt>::CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
		{
			CMemoryPool<ObjectType, opt>::_VerificationCode = reinterpret_cast<ULONG_PTR>(this);
			if (CMemoryPool<ObjectType, opt>::_HeapHandle == NULL)
			{
				CMemoryPool<ObjectType, opt>::_HeapHandle = HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
				CMemoryPool<ObjectType, opt>::_HeapOptions = flOptions;
			}
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline CMemoryPool<ObjectType, opt>::~CMemoryPool()
		{
			if constexpr (opt == DestructorCallOption::WHEN_MEMORY_POOL_IS_DESTROYED)
			{
				Node* pCursor = this->_Top;
				Node* pTemp;
				while (pCursor != nullptr)
				{
					pTemp = pCursor->below;
					delete pCursor;
					pCursor = pTemp;
				}
			}

			HeapDestroy(CMemoryPool<ObjectType, opt>::_HeapHandle);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::GetObjectErrorCrash()
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool_MT Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] std::bad_alloc exception occurred. Pool code is %llu", _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::ReturnObjectErrorCrash(ULONG_PTR llWrongVerificationCode)
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] An object with wrong verification code %llu was returned to the memory pool of code %llu.", llWrongVerificationCode, _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType, DestructorCallOption opt>
		template<typename ...Types>
		inline CMemoryPool<ObjectType, opt>::Node::Node(Types ...args)
			: verificationCode(CMemoryPool<ObjectType, opt>::_VerificationCode)
			, object(args...)
		{
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline CMemoryPool<ObjectType, opt>::Node::~Node()
		{
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void* CMemoryPool<ObjectType, opt>::Node::operator new(size_t size)
		{
			return HeapAlloc(CMemoryPool<ObjectType, opt>::_HeapHandle, CMemoryPool<ObjectType, opt>::_HeapOptions, size);
		}

		template<typename ObjectType, DestructorCallOption opt>
		inline void CMemoryPool<ObjectType, opt>::Node::operator delete(void* _Block)
		{
			HeapFree(CMemoryPool<ObjectType, opt>::_HeapHandle, 0, _Block);
		}

		template<typename ObjectType, DestructorCallOption opt>
		CMemoryPool<ObjectType, opt>* CMemoryPool<ObjectType, opt>::pInstance = nullptr;

		template<typename ObjectType, DestructorCallOption opt>
		ULONG_PTR CMemoryPool<ObjectType, opt>::_VerificationCode = -1;

		template<typename ObjectType, DestructorCallOption opt>
		HANDLE CMemoryPool<ObjectType, opt>::_HeapHandle = NULL;

		template<typename ObjectType, DestructorCallOption opt>
		DWORD CMemoryPool<ObjectType, opt>::_HeapOptions = flOptions;
	}
}

#pragma warning(default:26495)

#undef _WINSOCKAPI_