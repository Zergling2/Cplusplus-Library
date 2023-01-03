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
		template<typename ...Types> ObjectType* New(Types ...args);
		void Delete(ObjectType* pObj);
	private:
		std::list<CTLSMPBucket*> _BucketList;
	};
public:
	CTLSMemoryPool64();
	~CTLSMemoryPool64();
	template<typename ...Types> inline static ObjectType* New(Types ...args);
	inline static void Delete(ObjectType* pObj);
private:
	static DWORD TLSIndex;
	static HANDLE TLSMPHeapHandle;
	static LONG InstanceCount;
};

template<typename ObjectType, TLSMPDestructorOpt opt>
CTLSMemoryPool64<ObjectType, opt>::CTLSMemoryPool64()
{
	if (CTLSMemoryPool64<ObjectType, opt>::InstanceCount != 0)
		*reinterpret_cast<int*>(0x0) = 0;
	DWORD newIndex = TlsAlloc();
	if (newIndex == TLS_OUT_OF_INDEXES)
		*reinterpret_cast<int*>(0x0) = 0;
	CTLSMemoryPool64<ObjectType, opt>::TLSIndex = newIndex;
	CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle = HeapCreate(0, static_cast<SIZE_T>(4096) * 16, 0);
	if (CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle == NULL)
		*reinterpret_cast<int*>(0x0) = 0;

	CTLSMemoryPool64<ObjectType, opt>::InstanceCount++;
}

template<typename ObjectType, TLSMPDestructorOpt opt>
CTLSMemoryPool64<ObjectType, opt>::~CTLSMemoryPool64()
{
	TlsFree(CTLSMemoryPool64<ObjectType, opt>::TLSIndex);
	HeapDestroy(CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
template<typename ...Types>
inline ObjectType* CTLSMemoryPool64<ObjectType, opt>::New(Types ...args)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex));
	if (pCore == NULL)
	{
		pCore = new CTLSMPCore;
		TlsSetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex, reinterpret_cast<LPVOID>(pCore));
	}
	return pCore->New(args...);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void CTLSMemoryPool64<ObjectType, opt>::Delete(ObjectType* pObj)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex));
	if (pCore == NULL)
		*reinterpret_cast<int*>(0x0) = 0;		// 존재하지도 않는 풀에 Free를 요청한 경우
	pCore->Delete(pObj);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
template<typename ...Types>
ObjectType* CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::New(Types ...args)
{
	DWORD index;
	BOOLEAN ret;
	CTLSMPNode* pNode;

	auto end = this->_BucketList.end();
	for (auto iter = this->_BucketList.begin(); iter != end; ++iter)
	{
		ret = _BitScanForward64(&index, (*iter)->_Flag);
		if (ret != 0)
		{
			// 인덱스번째에 옵젝 사용 가능
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
	_BucketList.push_front(pNewBucket);		// 새로 만든 버킷이 가장 우선적으로 탐색되게 한다.

	return reinterpret_cast<ObjectType*>(pNode);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::Delete(ObjectType* pObj)
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
	return HeapAlloc(CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle, 0, sizeof(CTLSMPBucket));
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CTLSMPBucket::operator delete(void* _Block)
{
	HeapFree(CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle, 0, _Block);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
DWORD CTLSMemoryPool64<ObjectType, opt>::TLSIndex = 0;

template<typename ObjectType, TLSMPDestructorOpt opt>
HANDLE CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle = NULL;

template<typename ObjectType, TLSMPDestructorOpt opt>
LONG CTLSMemoryPool64<ObjectType, opt>::InstanceCount = 0;