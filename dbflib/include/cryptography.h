#ifndef CRYPTOGRAPHY_INTERFACE
#define CRYPTOGRAPHY_INTERFACE

#include "dom.h"
#include "keys.h"
#include "smart.h"

#ifndef TYPEDEF_BYTE
#define TYPEDEF_BYTE
typedef unsigned char	byte;
#endif
//{2C7A80F5-7724-4127-B9C3-E7490D216EB9}

#define	UIID_ICipher		UIID(0x2C7A80F5,0x7423721D,0x00000001)
#define	UIID_ICipher2		UIID(0x2C7A80F5,0x7423721D,0x00000002)

#define	UIID_IRijndael		UIID(0x2C7A80F5,0xFA1E0A5B,0xBAC3D164)
#define	UIID_IDes			UIID(0x2C7A80F5,0xFA1E0A5B,0x67B5517A)
#define	UIID_ICast256		UIID(0x2C7A80F5,0xFA1E0A5B,0xC695AFAC)
#define	UIID_IBlowfish		UIID(0x2C7A80F5,0xFA1E0A5B,0xED3CD9E5)
#define	UIID_IGost			UIID(0x2C7A80F5,0xFA1E0A5B,0xDC352C2A)
#define	UIID_IRc6			UIID(0x2C7A80F5,0xFA1E0A5B,0x2CA50958)
#define	UIID_IBelt			UIID(0x2C7A80F5,0xFA1E0A5B,0x2CA50960)


#define	UIID_IAgreement		UIID(0x2C7A80F5,0xFB178326,0x6A2DE42A)
#define	UIID_IRsa			UIID(0x2C7A80F5,0xFB178326,0x6A2DE42D)
#define	UIID_IDiffieHellman	UIID(0x2C7A80F5,0xFB178326,0x91FBDE9F)

#define	UIID_IMV2			UIID(0x2C7A80F5,0xFB178326,0x671845F2)

//namespace Property {
//	namespace Cipher {
//		enum {keyBitsMin = Common::Last,keyBitsMax,blockSize};
//	}
//}

struct ICipher : public virtual TLDOM::IBase {
	TLDOM_DECLARE_UIID(UIID_ICipher)
	virtual bool __stdcall SetKey(IKey& Key) = 0;
	virtual bool __stdcall Encrypt(byte inBlock[],long inLength,byte outBlock[],long& outLength) = 0;
	virtual bool __stdcall Decrypt(byte inBlock[],long inLength,byte outBlock[],long& outLength) = 0;
	virtual bool __stdcall Generate(IKey& key,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
	virtual bool __stdcall Generate(IKey& keyPublic,IKey& keyPrivate,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
};

struct ICipher2 : public virtual TLDOM::IBase {
	TLDOM_DECLARE_UIID(UIID_ICipher2)
	virtual bool __stdcall SetKey(IKey& Key) = 0;
	virtual bool __stdcall Encrypt(const dom::memory& in,dom::memory& out) = 0;
	virtual bool __stdcall Decrypt(const dom::memory& in,dom::memory& out) = 0;
	virtual bool __stdcall Generate(IKey& key,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
	virtual bool __stdcall Generate(IKey& keyPublic,IKey& keyPrivate,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
};


struct IAgreement : public virtual TLDOM::IBase {
	TLDOM_DECLARE_UIID(UIID_IAgreement)
	virtual bool __stdcall Generate(IKey& keyPublic,IKey& keyPrivate,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
	virtual bool __stdcall Secret(IKey& keyPublic,IKey& keyPrivate,byte* CommonSecret,long& LengthOfCommonSecret) = 0;
};

struct IMV2 : public virtual TLDOM::IBase {
	TLDOM_DECLARE_UIID(UIID_IMV2)
	enum {Rounds = 0x01,Rest = 0x02,Crc = 0x04};
	virtual bool __stdcall SetKey(IKey& Key) = 0;
	virtual bool __stdcall SetMode(unsigned Mode,unsigned Param = 0) = 0;
	virtual bool __stdcall Encrypt(dom::memory& data,dom::memory& core,dom::memory& flags) = 0;
	virtual bool __stdcall Decrypt(dom::memory& core,dom::memory& flags,dom::memory& data) = 0;
	virtual bool __stdcall Generate(IKey& key,byte* Seed = NULL,long sizeOfSeed = 0) = 0;
};

#endif //CRYPTOGRAPHY_INTERFACE