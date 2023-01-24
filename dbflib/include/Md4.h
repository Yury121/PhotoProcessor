#pragma once

#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif

#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CMd4 :
	public CBase<CMd4>,
	public IHash,
	public IHash2
{
public:
	CMd4();
	~CMd4();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	inline void get_hash(const unsigned __int8[]);
	inline void FF(unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int8);
	inline void GG(unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int8);
	inline void HH(unsigned __int32&, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int32, unsigned __int8);
	unsigned __int8 buffer[64];
	unsigned __int32 M[16];
	unsigned __int32 digest[4];
	unsigned __int64 count, position; 
};
