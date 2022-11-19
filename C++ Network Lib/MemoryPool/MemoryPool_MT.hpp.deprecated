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

namespace SJNET
{
	namespace LIB
	{
		template<typename ObjectType>
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
			SRWLOCK stSRWLock;
			static CMemoryPool<ObjectType>* pInstance;
			static ULONG_PTR _VerificationCode;
			static HANDLE _HeapHandle;
			static DWORD _HeapOptions;
		private:
			struct Node
			{
			public:
				template<typename... Types> Node(Types ...args);
				void* operator new(size_t size);
				void operator delete(void* _Block);
				ObjectType object;
				ULONG_PTR verificationCode;
				Node* below;
			};
		};

		template<typename ObjectType>
		inline CMemoryPool<ObjectType>* CMemoryPool<ObjectType>::GetInstance()
		{
			if (CMemoryPool<ObjectType>::pInstance == nullptr)
			{
				CMemoryPool<ObjectType>::pInstance = new CMemoryPool<ObjectType>(flOptions, dwInitialSize, dwMaximumSize);
				atexit(CMemoryPool<ObjectType>::Destroy);
			}

			return CMemoryPool<ObjectType>::pInstance;
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::Destroy()
		{
			delete CMemoryPool<ObjectType>::pInstance;
			CMemoryPool<ObjectType>::pInstance = nullptr;
		}

		template<typename ObjectType>
		CMemoryPool<ObjectType>::CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
		{
			InitializeSRWLock(&this->stSRWLock);
			CMemoryPool<ObjectType>::_VerificationCode = reinterpret_cast<ULONG_PTR>(this);
			if (CMemoryPool<ObjectType>::_HeapHandle == NULL)
			{
				CMemoryPool<ObjectType>::_HeapHandle = HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
				CMemoryPool<ObjectType>::_HeapOptions = flOptions;
			}
		}

		template<typename ObjectType>
		CMemoryPool<ObjectType>::~CMemoryPool()
		{
			HeapDestroy(CMemoryPool<ObjectType>::_HeapHandle);
		}

		template<typename ObjectType>
		template<typename ...Types>
		ObjectType* CMemoryPool<ObjectType>::GetObjectFromPool(Types ...args)
		{
			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* pTmp = nullptr;		// 최대 최적화 (속도 우선) 사용 시 nullptr 대입 코드 삭제 가능.

			if (!(this->_Top))
			{
				ReleaseSRWLockExclusive(&this->stSRWLock);
				try
				{
					pTmp = new Node(args...);
				}
				catch (std::bad_alloc& e)
				{
					UNREFERENCED_PARAMETER(e);
					CMemoryPool<ObjectType>::GetObjectErrorCrash();
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

		// ----------------------------------------- CSession 특수화 -----------------------------------------
		template<>
		template<typename ...Types>
		CSession* CMemoryPool<CSession>::GetObjectFromPool(Types ...args)
		{
			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* pTmp = nullptr;		// 최대 최적화 (속도 우선) 사용 시 nullptr 대입 코드 삭제 가능.

			if (!(this->_Top))
			{
				ReleaseSRWLockExclusive(&this->stSRWLock);
				try
				{
					pTmp = new Node(true, args...);
				}
				catch (std::bad_alloc& e)
				{
					UNREFERENCED_PARAMETER(e);
					CMemoryPool<CSession>::GetObjectErrorCrash();
				}
				return reinterpret_cast<CSession*>(pTmp);
			}
			else
			{
				pTmp = this->_Top;
				this->_Top = this->_Top->below;
				ReleaseSRWLockExclusive(&this->stSRWLock);
				new (pTmp) CSession(false, args...);		// placement new
				return reinterpret_cast<CSession*>(pTmp);
			}
		}
		// ----------------------------------------- CSession 특수화 -----------------------------------------

		// 메모리 풀로 반환되는 객체에는 무조건 소멸자 호출
		template<typename ObjectType>
		void CMemoryPool<ObjectType>::ReturnObjectToPool(ObjectType* _Ret)
		{
			if (reinterpret_cast<Node*>(_Ret)->verificationCode != this->_VerificationCode)
				CMemoryPool<ObjectType>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(_Ret)->verificationCode);

			_Ret->~ObjectType();

			AcquireSRWLockExclusive(&this->stSRWLock);
			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(_Ret);
			this->_Top->below = prevTop;
			ReleaseSRWLockExclusive(&this->stSRWLock);
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::GetObjectErrorCrash()
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool_MT Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] std::bad_alloc exception occurred. Pool code is %llu", _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::ReturnObjectErrorCrash(ULONG_PTR llWrongVerificationCode)
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] An object with wrong verification code %llu was returned to the memory pool of code %llu.", llWrongVerificationCode, _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABABABAB);
		}

		template<typename ObjectType>
		template<typename ...Types>
		inline CMemoryPool<ObjectType>::Node::Node(Types ...args)
			: verificationCode(CMemoryPool<ObjectType>::_VerificationCode)
			, object(args...)
		{
		}

		template<typename ObjectType>
		inline void* CMemoryPool<ObjectType>::Node::operator new(size_t size)
		{
			return HeapAlloc(CMemoryPool<ObjectType>::_HeapHandle, CMemoryPool<ObjectType>::_HeapOptions, size);
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::Node::operator delete(void* _Block)
		{
			HeapFree(CMemoryPool<ObjectType>::_HeapHandle, 0, _Block);
		}

		// --------------------------------------------------------------------------------------
		template<typename ObjectType>
		CMemoryPool<ObjectType>* CMemoryPool<ObjectType>::pInstance = nullptr;

		template<typename ObjectType>
		ULONG_PTR CMemoryPool<ObjectType>::_VerificationCode = -1;

		template<typename ObjectType>
		HANDLE CMemoryPool<ObjectType>::_HeapHandle = NULL;

		template<typename ObjectType>
		DWORD CMemoryPool<ObjectType>::_HeapOptions = flOptions;
	}
}

#pragma warning(default:26495)

#undef _WINSOCKAPI_