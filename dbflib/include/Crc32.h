#pragma once
#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif
#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CCrc32 :
	public CBase<CCrc32>,
	public IHash,
	public IHash2
{
public:
	CCrc32();
	~CCrc32();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	static unsigned __int32 TABLE[256];
	unsigned __int32 crc;
};
