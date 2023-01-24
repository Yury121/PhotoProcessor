#pragma once

#ifndef EXCLUDE_DLL_ENTRY
#define EXCLUDE_DLL_ENTRY
#endif

#include "../Include/Hash.h"
#include "../Include/domserver.h"

class CMd5 :
	public CBase<CMd5>,
	public IHash,
	public IHash2
{
public:
	CMd5();
	~CMd5();
	virtual void __stdcall Hash(void* pInput,int iInputSize);
	virtual void __stdcall Final(void* pOutput);
	virtual void __stdcall Clear();
	virtual void __stdcall	Final(dom::memory& out);
protected:
	__forceinline void	Transform(unsigned __int8* block);
	__forceinline void	Encode(unsigned __int8* dest, unsigned __int32* src, unsigned __int32 nLength);
	__forceinline void	Decode(unsigned __int32* dest, unsigned __int8* src, unsigned __int32 nLength);
	unsigned __int32	m_State[4];
	unsigned __int32	m_Count[2];
	unsigned __int8		m_Buffer[64];
	unsigned __int8		m_Finalized;
};
