#pragma once
#include <dom.h>
#include <Properties.h>
#include <smart.h>

enum LzoPropId {CompressLevel = 1};
enum LzmaPropId {DictionarySize = 1,PosStateBits,LitContextBits,LitPosBits,Algorithm,NumFastBytes};

#define UIID_ICompress	UIID(0x12000000,0x00000000,0x00000001)
#define UIID_ICompress2	UIID(0x12000000,0x00000000,0x00000002)
#define UIID_ILzo		UIID(0x12000000,0x00000000,0x00000011)
#define UIID_ILzma		UIID(0x12000000,0x00000000,0x00000012)

struct ICompress : 
	public virtual IBase
{
	DOM_DECLARE_UIID(UIID_ICompress)
	virtual bool __stdcall Compress(const byte* in,long szIn,byte* out,long& szOut) = 0;
	virtual bool __stdcall Decompress(const byte* in,long szIn,byte* out,long& szOut) = 0;
};

struct ICompress2 : 
	public virtual IBase
{
	DOM_DECLARE_UIID(UIID_ICompress2)
	virtual bool __stdcall Compress(const dom::memory& in,dom::memory& out) = 0;
	virtual bool __stdcall Decompress(const dom::memory& in,dom::memory& out) = 0;
};

