// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#include "LFMemoryPool64.hpp"

namespace SJNET
{
	namespace LIB
	{
		template<typename T>
		class CLFStack64
		{
		private:
			class CLFStack64Node
			{
				friend class CLFStack64;
			public:
				inline CLFStack64Node(T data);
			private:
				T data;
				CLFStack64Node* _pBelow;
			};
		public:
			CLFStack64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
			void Push(const T& data);
			bool Pop(T& buf);
			bool Empty() { return !GetLFStampRemovedAddress(_pTop); }
		private:
			alignas(8) CLFStack64Node* volatile _pTop;		// 64��Ʈ ��迡 ����
			alignas(64) CLFMemoryPool64<CLFStack64<T>::CLFStack64Node, LFMPDestructorCallOption::AUTO> _NodePool;
		};

		template<typename T>
		CLFStack64<T>::CLFStack64Node::CLFStack64Node(T data)
			: data(data)
		{
		}

		template<typename T>
		CLFStack64<T>::CLFStack64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _pTop(nullptr)
			, _NodePool(flOptions, dwInitialSize, dwMaximumSize)
		{
		}

		template<typename T>
		void CLFStack64<T>::Push(const T& data)
		{
			CLFStack64Node* pNewNode = _NodePool.GetObjectFromPool(data);
			CLFStack64Node* pTemp;
			CLFStack64Node* pNewTop;
			do
			{
				pTemp = this->_pTop;		// load
				pNewNode->_pBelow = reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pTemp));	// load
				pNewTop = reinterpret_cast<CLFStack64Node*>(reinterpret_cast<ULONG64>(pNewNode) | (GetLFStamp(pTemp) + LF_MASK_INC));
			} while (reinterpret_cast<long long>(pTemp) != _InterlockedCompareExchange64(reinterpret_cast<volatile long long*>(&_pTop), reinterpret_cast<long long>(pNewTop), reinterpret_cast<long long>(pTemp)));
		}

		template<typename T>
		bool CLFStack64<T>::Pop(T& buf)
		{
			CLFStack64Node* pNode;
			CLFStack64Node* pNewTop;
			do
			{
				pNode = this->_pTop;
				if (GetLFStampRemovedAddress(pNode) == nullptr)
					return false;
				pNewTop = reinterpret_cast<CLFStack64Node*>(reinterpret_cast<LONG64>(reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pNode))->_pBelow) | GetLFStamp(pNode));	// ���� Pop������ GetLFStamp ��ȯ���� LF_MASK_INC�� ���� �ʿ�� ���ٰ� ������.
			} while (reinterpret_cast<LONG64>(pNode) != _InterlockedCompareExchange64(reinterpret_cast<volatile LONG64*>(&_pTop), reinterpret_cast<LONG64>(pNewTop), reinterpret_cast<LONG64>(pNode)));

			pNode = reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pNode));
			buf = pNode->data;
			_NodePool.ReturnObjectToPool(pNode);
			return true;
		}
	}
}
