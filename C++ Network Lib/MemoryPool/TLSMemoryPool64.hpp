// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#define _WINSOCKAPI
#include <Windows.h>
#undef _WINSOCKAPI
#include <list>

constexpr DWORD BIT_SCAN_SIZE = 64;

enum class TLSMPDestructorOpt
{
	AUTO,
	MANUAL
};

template<typename ObjectType, TLSMPDestructorOpt opt>
class CTLSMemoryPool64
{
private:
	class CTLSMPCore
	{
	private:
		struct CTLSMPBucket;
		struct CTLSMPNode
		{
			ObjectType _Obj;
			LONG64 _Mask;
			LONG64* _pFlag;
		};
		struct CTLSMPBucket
		{
			inline void* operator new(size_t size);
			inline void operator delete(void* _Block);
			inline CTLSMPBucket() : _Flag(0xFFFFFFFFFFFFFFFE)
			{
				ULONG64 mask = 0x8000000000000000;
				for (int i = 0; i < BIT_SCAN_SIZE; i++)
				{
					reinterpret_cast<CTLSMPNode*>(_Buffer)[i]._Mask = mask;
					reinterpret_cast<CTLSMPNode*>(_Buffer)[i]._pFlag = &this->_Flag;
					mask >>= 1;
				}
			}
			BYTE _Buffer[sizeof(CTLSMPNode) * BIT_SCAN_SIZE];
			LONG64 _Flag;
		};
	public:
		template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
		void ReturnObjectToPool(ObjectType* pObj);
	private:
		std::list<CTLSMPBucket*> _BucketList;
	};
public:
	CTLSMemoryPool64();
	~CTLSMemoryPool64();
	template<typename ...Types> inline static ObjectType* GetObjectFromPool(Types ...args);
	inline static void ReturnObjectToPool(ObjectType* pObj);
private:
	static CTLSMemoryPool64 _TLSAllocater;
	static DWORD _TLSIndex;
	static HANDLE _HeapHandle;
	static LONG _InstanceCount;
};

template<typename ObjectType, TLSMPDestructorOpt opt>
CTLSMemoryPool64<ObjectType, opt>::CTLSMemoryPool64()
{
	if (CTLSMemoryPool64<ObjectType, opt>::_InstanceCount != 0)
		*reinterpret_cast<int*>(0x0) = 0;
	DWORD newIndex = TlsAlloc();
	if (newIndex == TLS_OUT_OF_INDEXES)
		*reinterpret_cast<int*>(0x0) = 0;
	CTLSMemoryPool64<ObjectType, opt>::_TLSIndex = newIndex;
	CTLSMemoryPool64<ObjectType, opt>::_HeapHandle = HeapCreate(0, static_cast<SIZE_T>(4096) * 16, 0);
	if (CTLSMemoryPool64<ObjectType, opt>::_HeapHandle == NULL)
		*reinterpret_cast<int*>(0x0) = 0;

	_InstanceCount++;
}

template<typename ObjectType, TLSMPDestructorOpt opt>
CTLSMemoryPool64<ObjectType, opt>::~CTLSMemoryPool64()
{
	TlsFree(CTLSMemoryPool64<ObjectType, opt>::_TLSIndex);
	HeapDestroy(CTLSMemoryPool64<ObjectType, opt>::_HeapHandle);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
template<typename ...Types>
inline ObjectType* CTLSMemoryPool64<ObjectType, opt>::GetObjectFromPool(Types ...args)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::_TLSIndex));
	if (pCore == NULL)
	{
		pCore = new CTLSMPCore;
		TlsSetValue(CTLSMemoryPool64<ObjectType, opt>::_TLSIndex, reinterpret_cast<LPVOID>(pCore));
	}
	return pCore->GetObjectFromPool(args...);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void CTLSMemoryPool64<ObjectType, opt>::ReturnObjectToPool(ObjectType* pObj)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::_TLSIndex));
	if (pCore == NULL)
		*reinterpret_cast<int*>(0x0) = 0;		// ���������� �ʴ� Ǯ�� Free�� ��û�� ���
	pCore->ReturnObjectToPool(pObj);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
template<typename ...Types>
ObjectType* CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::GetObjectFromPool(Types ...args)
{
	DWORD index;
	BOOLEAN find = false;
	BOOLEAN ret;
	CTLSMPNode* pNode;

	auto end = this->_BucketList.end();
	for (auto iter = this->_BucketList.begin(); iter != end; ++iter)
	{
		ret = _BitScanForward64(&index, (*iter)->_Flag);
		if (ret != 0)
		{
			// �ε�����°�� ���� ��� ����
			pNode = reinterpret_cast<CTLSMPNode*>(&reinterpret_cast<CTLSMPNode*>((*iter)->_Buffer)[BIT_SCAN_SIZE - 1 - index]);
			new (pNode) ObjectType(args...);
			LONG64 com;
			LONG64 mask = ~(pNode->_Mask);
			do
			{
				com = *(pNode->_pFlag);
			} while (com != InterlockedCompareExchange64(reinterpret_cast<volatile LONG64*>(pNode->_pFlag), (*iter)->_Flag & mask, com));

			return reinterpret_cast<ObjectType*>(pNode);
		}
	}

	CTLSMPBucket* pNewBucket = new CTLSMPBucket;
	pNode = &reinterpret_cast<CTLSMPNode*>(pNewBucket->_Buffer)[BIT_SCAN_SIZE - 1];
	new (pNode) ObjectType(args...);
	_BucketList.push_front(pNewBucket);		// ���� ���� ��Ŷ�� ���� �켱������ Ž���ǰ� �Ѵ�.

	return reinterpret_cast<ObjectType*>(pNode);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::ReturnObjectToPool(ObjectType* pObj)
{
	if constexpr (opt == TLSMPDestructorOpt::AUTO)
		pObj->~ObjectType();
	volatile  CTLSMPNode* pNode = reinterpret_cast<CTLSMPNode*>(pObj);

	LONG64 com;
	do
	{
		com = *(pNode->_pFlag);
	} while (com != InterlockedCompareExchange64(reinterpret_cast<volatile LONG64*>(pNode->_pFlag), *(pNode->_pFlag) | pNode->_Mask, com));
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void* CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CTLSMPBucket::operator new(size_t size)
{
	return HeapAlloc(CTLSMemoryPool64<ObjectType, opt>::_HeapHandle, 0, sizeof(CTLSMPBucket));
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CTLSMPBucket::operator delete(void* _Block)
{
	HeapFree(CTLSMemoryPool64<ObjectType, opt>::_HeapHandle, 0, _Block);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
DWORD CTLSMemoryPool64<ObjectType, opt>::_TLSIndex = 0;

template<typename ObjectType, TLSMPDestructorOpt opt>
HANDLE CTLSMemoryPool64<ObjectType, opt>::_HeapHandle = NULL;

template<typename ObjectType, TLSMPDestructorOpt opt>
CTLSMemoryPool64<ObjectType, opt> CTLSMemoryPool64<ObjectType, opt>::_TLSAllocater;

template<typename ObjectType, TLSMPDestructorOpt opt>
LONG CTLSMemoryPool64<ObjectType, opt>::_InstanceCount = 0;