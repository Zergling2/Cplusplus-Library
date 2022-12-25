// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#include "LFMemoryPool64.hpp"
#include "Dumper.h"

#define ALLOC 0xABABABABABABABAB;
#define FREE 0xCDCDCDCDCDCDCDCD;
#define PUSH_DO_WHILE_ESCAPE 0xAAEEEEEEEEEEEEEE;
#define POP_DO_WHILE_ESCAPE 0xDDEEEEEEEEEEEEEE;
#define PUSH_DO_WHILE 0xAAAAAAAAAAAAAAAA;
#define POP_DO_WHILE 0xDDDDDDDDDDDDDDDD;

#define PUSH_CAS_SUCCESS 0xAA11EEEEEEEEEEEE;
#define POP_CAS_SUCCESS 0xDD11EEEEEEEEEEEE;

namespace SJNET
{
	namespace LIB
	{
		template<typename T>
		class CLFQueue64
		{
		private:
			struct CLFQueue64Node
			{
				friend CLFQueue64;
			public:
				inline CLFQueue64Node(T data);
			private:
				T data;
				CLFQueue64Node* _pNext;
			};
		public:
			template<typename ...Types> CLFQueue64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize, Types ...args);
			void Push(const T data);
			bool Pop(T& buf);
		private:
			alignas(64) CLFQueue64Node* _pHead;
			alignas(64) CLFQueue64Node* _pTail;
			alignas(64) CLFMemoryPool64<CLFQueue64Node, LFMPDestructorCallOption::AUTO> _NodePool;
		};

		template<typename T>
		template<typename ...Types>
		inline CLFQueue64<T>::CLFQueue64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize, Types ...dummyConstructorArgs)
			: _NodePool(flOptions, dwInitialSize, dwMaximumSize)
		{
			_pHead = _pTail = _NodePool.GetObjectFromPool(dummyConstructorArgs...);
			_pHead->_pNext = nullptr;
		}

		template<typename T>
		void CLFQueue64<T>::Push(const T data)
		{
			CLFQueue64Node* pNewNode = _NodePool.GetObjectFromPool(data);

			CLFQueue64Node* volatile pTempTail;
			CLFQueue64Node* volatile pTempTailNext;
			for (;;)
			{
				pTempTail = this->_pTail;
				pTempTailNext = reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempTail))->_pNext;
				if (pTempTailNext == nullptr)
				{
					CLFQueue64Node** ppTarget = &reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempTail))->_pNext;
					if (nullptr == InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(ppTarget), pNewNode, nullptr))
					{
						InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pTail), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pNewNode) | (GetLFStamp(pTempTail) + LF_MASK_INC)), pTempTail);
						break;
					}
				}
				else
				{
					InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pTail), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempTailNext) | (GetLFStamp(pTempTail) + LF_MASK_INC)), pTempTail);
				}
			}
		}

		template<typename T>
		bool CLFQueue64<T>::Pop(T& buf)
		{
			CLFQueue64Node* volatile pTempHead;
			CLFQueue64Node* volatile pTempHeadNext;
			CLFQueue64Node* volatile pTempTail;
			do
			{
				pTempTail = this->_pTail;
				pTempHead = this->_pHead;
				pTempHeadNext = reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead))->_pNext;
				if (pTempHead == pTempTail)
				{
					if (pTempHeadNext == nullptr)		// ��¥�� �� �������� ��尡 ������ ���
					{
						return false;
					}
					else
					{
						InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pTail), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempHeadNext) | (GetLFStamp(pTempHead) + LF_MASK_INC)), pTempTail);
						continue;
					}
				}
				else
				{
					if (pTempHeadNext == nullptr)		// �������� �޾ƿ� Head�� Tail�� ���� ���� ���� �󸶵��� TempHead�� next �ʵ尡 null�� �� �ִ�. ���� �߿��� ����Ʈ
						continue;
					else
						buf = pTempHeadNext->data;	// do while �ȿ��� �̸� �ؾ���.
						// CAS ���� �� ������ �ϸ� ����� ����� data �ʵ带 ������ ������ �߻��� �� �ִ�.
				}

				// _pNext�� �������� �����Ƿ� OR���길���� ����� �ۼ��� �ڵ�
			} while (pTempHead != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pHead), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempHeadNext) | (GetLFStamp(pTempHead) + LF_MASK_INC)), pTempHead));

			_NodePool.ReturnObjectToPool(reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead)));		// ���� ���� ������ ��带 �����Ѵ�.
			return true;
		}

		template<typename T>
		inline CLFQueue64<T>::CLFQueue64Node::CLFQueue64Node(T data)
			: data(data)
			, _pNext(nullptr)
		{
		}
	}
}
