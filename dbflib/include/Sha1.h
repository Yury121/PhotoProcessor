#pragma once

#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif

#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CSha1 :
	public CBase<CSha1>,
	public IHash,
	public IHash2
{
public:
	CSha1();
	~CSha1();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	void get_hash(const unsigned __int8[]);
	__forceinline void F1(unsigned __int32, unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32&, unsigned __int32);
	__forceinline void F2(unsigned __int32, unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32&, unsigned __int32);
	__forceinline void F3(unsigned __int32, unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32&, unsigned __int32);
	__forceinline void F4(unsigned __int32, unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32&, unsigned __int32);
	unsigned __int8 buffer[64];
	unsigned __int32 W[80];
	unsigned __int32 digest[5];
	unsigned __int64 count, position; 
};
