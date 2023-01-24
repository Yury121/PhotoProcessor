#pragma once

#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif

#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CSha512 :
	public CBase<CSha512>,
	public IHash,
	public IHash2
{
public:
	CSha512();
	~CSha512();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	void get_hash(const unsigned __int8[]);
	__forceinline void F1(unsigned __int64, unsigned __int64, unsigned __int64, unsigned __int64&, unsigned __int64,
		unsigned __int64, unsigned __int64, unsigned __int64&, unsigned __int64, unsigned __int64);
	__forceinline unsigned __int64 rho(unsigned __int64, unsigned __int32, unsigned __int32, unsigned __int32);
	__forceinline unsigned __int64 sigma(unsigned __int64, unsigned __int32, unsigned __int32, unsigned __int32);
	unsigned __int8 buffer[128];
	unsigned __int64 W[80];
	unsigned __int64 digest[8];
	unsigned __int64 count, position;
};
