// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#define _WINSOCKAPI
#include <Windows.h>
#undef _WINSOCKAPI
#include "LFStack64.hpp"

// ========================================================================================================================
// You must add this preprocessor directives to the cpp file of the class for which you want to use the TLS memory pool.
#define TLS_MEMORY_POOL_USING(class_name, DestructorOpt) \
CTLSMemoryPool64<class_name, DestructorOpt> CTLSMemoryPool64<class_name, DestructorOpt>::TLSAllocater;\
CTLSMemoryPool64<class_name, DestructorOpt>::CTLSMPCore::CTLSMPSuper CTLSMemoryPool64<class_name, DestructorOpt>::CTLSMPCore::CTLSMPSuper::SuperPool;
// ========================================================================================================================

enum class TLSMPDestructorOpt
{
	AUTO,
	MANUAL
};

template<typename ObjectType, TLSMPDestructorOpt opt>
class CTLSMemoryPool64
{
private:
	enum BucketSize
	{
		SIZE = 100
	};
	class CTLSMPCore
	{
	public:
		struct CTLSMPNode
		{
		public:
			inline void* operator new(size_t size);
			void operator delete(void* _Block);
			template<typename ...Types> inline CTLSMPNode(Types ...args) : _Obj(args...), _pNext(nullptr) {}
			ObjectType _Obj;
			CTLSMPNode* _pNext;
		};
		struct CTLSMPBucket
		{
		public:
			inline CTLSMPBucket() : _pTop(nullptr), _Count(0) {}
			CTLSMPNode* _pTop;
			int _Count;
		};
		class CTLSMPSuper
		{
		public:
			inline CTLSMPSuper() : _FullBucketStack(0, 4096 * 16, 0), _BucketPool(0, 4096 * 16, 0) {}
			SJNET::LIB::CLFStack64<CTLSMPBucket*> _FullBucketStack;
			SJNET::LIB::CLFMemoryPool64<CTLSMPBucket, LFMPDestructorCallOption::MANUAL> _BucketPool;
			static CTLSMPSuper SuperPool;
		};
	public:
		inline CTLSMPCore() : pUsingBucket(nullptr), pRetBucket(nullptr) {}
		template<typename ...Types> ObjectType* CoreNew(Types ...args);
		void CoreDelete(ObjectType* pObj);
		CTLSMPBucket* pUsingBucket;			// 현재 스레드가 할당해서 사용하고 있는 버킷
		CTLSMPBucket* pRetBucket;			// 현재 스레드가 반환용으로 사용하고 있는 버킷
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
	static CTLSMemoryPool64 TLSAllocater;
};

template<typename ObjectType, TLSMPDestructorOpt opt>
DWORD CTLSMemoryPool64<ObjectType, opt>::TLSIndex;

template<typename ObjectType, TLSMPDestructorOpt opt>
HANDLE CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle;

template<typename ObjectType, TLSMPDestructorOpt opt>
LONG CTLSMemoryPool64<ObjectType, opt>::InstanceCount;


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
ObjectType* CTLSMemoryPool64<ObjectType, opt>::New(Types ...args)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex));
	if (pCore == nullptr)
	{
		pCore = new CTLSMPCore;
		if (0 == TlsSetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex, reinterpret_cast<LPVOID>(pCore)))
			*reinterpret_cast<int*>(0x0) = 0;
	}
	return pCore->CoreNew(args...);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
void CTLSMemoryPool64<ObjectType, opt>::Delete(ObjectType* pObj)
{
	CTLSMPCore* pCore = reinterpret_cast<CTLSMPCore*>(TlsGetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex));
	if (pCore == nullptr)
	{
		pCore = new CTLSMPCore;
		if (0 == TlsSetValue(CTLSMemoryPool64<ObjectType, opt>::TLSIndex, reinterpret_cast<LPVOID>(pCore)))
			*reinterpret_cast<int*>(0x0) = 0;
	}
	pCore->CoreDelete(pObj);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
template<typename ...Types>
ObjectType* CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CoreNew(Types ...args)
{
	CTLSMPNode* pNode;
	if (this->pUsingBucket == nullptr)
	{
		CTLSMPBucket* pBucket;
		bool result = CTLSMPSuper::SuperPool._FullBucketStack.Pop(pBucket);
		if (result == false)
		{
			pNode = new CTLSMPNode(args...);
			return &pNode->_Obj;
		}
		else
		{
			// 이 스레드의 Using 버킷에 등록
			this->pUsingBucket = pBucket;
		}
	}

	pNode = this->pUsingBucket->_pTop;
	this->pUsingBucket->_pTop = this->pUsingBucket->_pTop->_pNext;
	this->pUsingBucket->_Count--;
	if (this->pUsingBucket->_Count == 0)
	{
		// 뽑아서 쓰다가 빈 버킷은 반환
		CTLSMPSuper::SuperPool._BucketPool.Delete(this->pUsingBucket);
		this->pUsingBucket = nullptr;
	}

	new (&pNode->_Obj) ObjectType(args...);
	return &pNode->_Obj;
}

template<typename ObjectType, TLSMPDestructorOpt opt>
void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CoreDelete(ObjectType* pObj)
{
	if constexpr (opt == TLSMPDestructorOpt::AUTO)
		pObj->~ObjectType();

	// 반환용 버킷이 없을 경우 슈퍼풀로부터 빈 버킷을 할당받는다.
	if (this->pRetBucket == nullptr)
		this->pRetBucket = CTLSMPSuper::SuperPool._BucketPool.New();

	reinterpret_cast<CTLSMPNode*>(pObj)->_pNext = this->pRetBucket->_pTop;
	this->pRetBucket->_pTop = reinterpret_cast<CTLSMPNode*>(pObj);
	this->pRetBucket->_Count++;
	if (this->pRetBucket->_Count == BucketSize::SIZE)
	{
		CTLSMPSuper::SuperPool._FullBucketStack.Push(this->pRetBucket);
		this->pRetBucket = nullptr;
	}
}

template<typename ObjectType, TLSMPDestructorOpt opt>
inline void* CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CTLSMPNode::operator new(size_t size)
{
	return HeapAlloc(CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle, 0, size);
}

template<typename ObjectType, TLSMPDestructorOpt opt>
void CTLSMemoryPool64<ObjectType, opt>::CTLSMPCore::CTLSMPNode::operator delete(void* _Block)
{
	HeapFree(CTLSMemoryPool64<ObjectType, opt>::TLSMPHeapHandle, 0, _Block);
}
