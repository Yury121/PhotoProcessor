#pragma once

#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif

#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CMd2 :
	public CBase<CMd2>,
	public IHash,
	public IHash2
{
public:
	CMd2();
	~CMd2();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	unsigned __int8 m_X[48], m_C[16], m_buf[16];
	unsigned int m_count;
};
