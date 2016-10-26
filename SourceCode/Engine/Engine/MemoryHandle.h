// Copyright 2016-2017 ?????????????. All Rights Reserved.
struct HandleInfo
{
public:
	HandleInfo(void *Address, unsigned int Size, bool IsFree) :
		Address(Address),
		Size(Size),
		IsFree(IsFree),
		Previous(nullptr),
		Next(nullptr),
		ReferencedCount(0)
	{
	}

	HandleInfo(void *Address, unsigned int Size, bool IsFree, HandleInfo *Previous) :
		Address(Address),
		Size(Size),
		IsFree(IsFree),
		Previous(Previous),
		Next(nullptr),
		ReferencedCount(0)
	{
	}

	HandleInfo(void *Address, unsigned int Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next) :
		Address(Address),
		Size(Size),
		IsFree(IsFree),
		Previous(Previous),
		Next(Next),
		ReferencedCount(0)
	{
	}

	void Grab(void)
	{
		++ReferencedCount;
	}

	void Drop(void)
	{
		--ReferencedCount;
	}

public:
	void* Address;
	//std::atomic<void*> Address;
	unsigned int Size;
	bool IsFree;
	HandleInfo *Previous;
	HandleInfo *Next;
	unsigned int ReferencedCount;
};

template <typename T> class MemoryHandle
{
	friend class Allocator;

private:
	HandleInfo *m_Info;

private:
	MemoryHandle(HandleInfo *Info) :
		m_Info(Info)
	{
		m_Info->Grab();
	}

public:
	~MemoryHandle(void)
	{
		m_Info->Drop();
	}

	MemoryHandle(const MemoryHandle<T> &Other) :
		m_Info(Other.m_Info)
	{
		m_Info->Grab();
	}

public:
	//T &Get(void)
	//{
	//	return *(T*)(m_Info->Address);
	//}

	//const T &Get(void) const
	//{
	//	return *(T*)(m_Info->Address);
	//}

	void operator = (const T &Value)
	{
		memcpy(m_Info->Address, &Value, sizeof(T));
	}

	void operator = (const MemoryHandle<T> &Other)
	{
		m_Info = Other.m_Info;
		m_Info->Grab();
	}
};