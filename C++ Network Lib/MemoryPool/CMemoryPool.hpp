#pragma once

#pragma warning(disable:26495)

#include <Windows.h>
#include <strsafe.h>
#include "CLogger.h"
#include "ForceCrash.h"

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
			inline CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (경고!) dwMaximumSize가 0이 아닌 경우 LFH 적용 불가 및 해당 힙에서의 객체 최대 할당 크기는 0x7FFF8로 제한됨.
			~CMemoryPool();
			template<typename... Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
		private:
			static void GetObjectErrorCrash();
			static void ReturnObjectErrorCrash(ULONG_PTR wrongVerificationCode);
			Node* _Top;
			inline static ULONG_PTR _VerificationCode = -1;
			inline static HANDLE _HeapHandle = NULL;
			inline static DWORD _HeapOptions;
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
		inline CMemoryPool<ObjectType>::CMemoryPool(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _Top(nullptr)
		{
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
			Node* pDel = this->_Top;
			Node* temp;

			while (pDel)
			{
				temp = pDel->below;
				delete pDel;
				pDel = temp;
			}

			HeapDestroy(CMemoryPool<ObjectType>::_HeapHandle);
		}

		template<typename ObjectType>
		template<typename ...Types>
		ObjectType* CMemoryPool<ObjectType>::GetObjectFromPool(Types ...args)
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
					CMemoryPool<ObjectType>::GetObjectErrorCrash();
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

		// 메모리 풀로 반환되는 객체는 무조건 소멸자가 호출된다.
		template<typename ObjectType>
		void CMemoryPool<ObjectType>::ReturnObjectToPool(ObjectType* _Ret)
		{
			if (reinterpret_cast<Node*>(_Ret)->verificationCode != this->_VerificationCode)
				ReturnObjectErrorCrash(reinterpret_cast<Node*>(_Ret)->verificationCode);

			_Ret->~ObjectType();
			Node* prevTop = this->_Top;
			this->_Top = reinterpret_cast<Node*>(_Ret);
			this->_Top->below = prevTop;
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::GetObjectErrorCrash()
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] std::bad_alloc exception occurred. Pool code is %llu", _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xAABBCCDD);
		}

		template<typename ObjectType>
		inline void CMemoryPool<ObjectType>::ReturnObjectErrorCrash(ULONG_PTR wrongVerificationCode)
		{
			SJNET::LIB::CFileLogger fLogger(L"CMemoryPool Error_", true);
			wchar_t logBuffer[128];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[MEMORY POOL ERROR] An object with wrong verification code %llu was returned to the memory pool of code %llu.", wrongVerificationCode, _VerificationCode);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xABCDABCD);
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
	}
}

#pragma warning(default:26495)