// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#include "LFMemoryPool64.hpp"

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
			alignas(64) CLFMemoryPool64<CLFQueue64Node, LFMPDestructorCallOption::AUTO> _NodePool;
			alignas(64) CLFQueue64Node* _pHead;
			alignas(64) CLFQueue64Node* _pTail;
			alignas(64) LONG _Size;
		};

		template<typename T>
		template<typename ...Types>
		inline CLFQueue64<T>::CLFQueue64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize, Types ...dummyConstructorArgs)
			: _NodePool(flOptions, dwInitialSize, dwMaximumSize)
			, _Size(0)
		{
			_pHead = _pTail = _NodePool.New(dummyConstructorArgs...);
			_pHead->_pNext = nullptr;
		}

		template<typename T>
		void CLFQueue64<T>::Push(const T data)
		{
			CLFQueue64Node* pNewNode = _NodePool.New(data);

			CLFQueue64Node* volatile pTempTail;
			CLFQueue64Node* volatile pTempTailNext;
			for (;;)
			{
				pTempTail = this->_pTail;
				pTempTailNext = reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempTail))->_pNext;
				if (pTempTailNext == nullptr)
				{
					if (nullptr == InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempTail))->_pNext), pNewNode, nullptr))
					{
						InterlockedIncrement(&this->_Size);
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

			if (InterlockedDecrement(&this->_Size) < 0)
			{
				InterlockedIncrement(&this->_Size);
				return false;
			}

			for (;;)
			{
				pTempTail = this->_pTail;
				pTempHead = this->_pHead;
				pTempHeadNext = reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead))->_pNext;
				if (pTempHead == pTempTail)
				{
					if (pTempHeadNext == nullptr)		// 사실 추가된 노드가 있었을 수 있다. 그러나 그게 락프리 큐 인스턴스와 '연결'된 상태는 아니었던 것.
					{
						continue;
					}
					else
					{
						InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pTail), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempHeadNext) | (GetLFStamp(pTempHead) + LF_MASK_INC)), pTempTail);
						continue;
					}
				}
				else
				{
					if (pTempHeadNext == nullptr)		// 지역으로 받아온 Head와 Tail이 같지 않은 경우라도 얼마든지 TempHead의 next 필드가 null일 수 있다. 정말 중요한 포인트
						continue;
					else
						buf = pTempHeadNext->data;	// do while 안에서 미리 해야함.
					// CAS 성공 후 나가서 하면 재사용된 노드의 data 필드를 얻어오는 결함이 발생할 수 있다.
				}

				if (pTempHead == InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pHead), reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempHeadNext) | (GetLFStamp(pTempHead) + LF_MASK_INC)), pTempHead))
					break;
			}

			_NodePool.Delete(reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead)));		// 더미 노드로 쓰였던 노드를 해제한다.
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
