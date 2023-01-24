#ifndef RANDOM_POOL_INTERFACE
#define RANDOM_POOL_INTERFACE

#include "dom.h"

#define UIID_IRandom			UIID(0xA9946645,0xAD6FDBFB,0x00000000)
#define UIID_IRandomMt			UIID(0xA9946645,0xAD6FDBFB,0x55979ADA)
#define UIID_IRandomPgp			UIID(0xA9946645,0xAD6FDBFB,0x49E90F63)
#define UIID_IRandomX917		UIID(0xA9946645,0xAD6FDBFB,0xD9903E43)

struct IRandom : public virtual TLDOM::IBase
{
	TLDOM_DECLARE_UIID(UIID_IRandom)

	virtual void __stdcall			Initialize(void* pSeed,long szSeed) = 0;
	virtual unsigned char __stdcall	Random() = 0;
	virtual void __stdcall			Randomize(void* pvRandomPool,long szPool) = 0;
};

#endif //RANDOM_POOL_INTERFACE