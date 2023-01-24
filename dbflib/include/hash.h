#ifndef HASH_CRC_INTERFACE
#define HASH_CRC_INTERFACE

#include "dom.h"
#include "smart.h"

#define UIID_IHash		UIID(0x2DFEBF0B,0x502BFD3C,0x00000001)
#define UIID_IHash2		UIID(0x2DFEBF0B,0x502BFD3C,0x00000002)

#define UIID_IMd2		UIID(0x2DFEBF0B,0x502BFD3C,0xA9A636CC)
#define UIID_IMd4		UIID(0x2DFEBF0B,0x502BFD3C,0xF05F70D6)
#define UIID_IMd5		UIID(0x2DFEBF0B,0x502BFD3C,0xC998EBD9)
#define UIID_ISha1		UIID(0x2DFEBF0B,0x502BFD3C,0x03BD8FA2)
#define UIID_ISha512	UIID(0x2DFEBF0B,0x502BFD3C,0x89853D9B)

#define UIID_IAdler32	UIID(0x2DFEBF0B,0x502BFD3C,0xBCB27960)
#define UIID_ICrc32		UIID(0x2DFEBF0B,0x502BFD3C,0xA1643455)
#define UIID_ICrc24		UIID(0x2DFEBF0B,0x502BFD3C,0xCA53BE46)

//namespace Property {
//	namespace Hash {
//		enum {blockSize = Common::Last};
//	}
//}

struct IHash : public virtual TLDOM::IBase
{
	TLDOM_DECLARE_UIID(UIID_IHash)

	virtual void __stdcall	Hash(void* pInput,int iInputSize) = 0;
	virtual void __stdcall	Final(void* pOutput) = 0;
	virtual void __stdcall	Clear() = 0;
};

struct IHash2 : public virtual TLDOM::IBase
{
	TLDOM_DECLARE_UIID(UIID_IHash2)

	virtual void __stdcall	Hash(void* pInput,int iInputSize) = 0;
	virtual void __stdcall	Final(dom::memory& out) = 0;
	virtual void __stdcall	Clear() = 0;
};

#endif //HASH_CRC_INTERFACE
