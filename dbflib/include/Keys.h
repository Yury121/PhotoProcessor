#ifndef CRYPTOGRAPHY_KEYS_INTERFACE
#define CRYPTOGRAPHY_KEYS_INTERFACE

enum KeyType {	
	UnknownKey			= 0,
	SymmtericEcb		= 0xFA1E0A5B,
	SymmtericCbc		= 0xB5A0E1AF,
	SymmtericGm 		= 0xB5A0E2AF,
	SymmtericSt 		= 0xB5A0E3AF,
	AsymmtericPublic	= 0xFB178326,
	AsymmtericPrivate	= 0x623871BF,
	MV2					= 0x671845F2,
};

enum KeySize {
	sizeZero		= 0,
	size64bits		= 8,
	size128bits		= 16,
	size160bits		= 20,
	size192bits		= 24,
	size256bits		= 32,
	size384bits		= 48,
	size512bits		= 64,
	size1024bits	= 128,
	size2048bits	= 256,
	size4096bits	= 512,
	size8192bits	= 1024,
	sizeLast
};

struct IKey {
	KeySize			keySize;
	KeyType			keyType;
	unsigned short	keyLength;
};

template<KeySize ks>
struct CKeyEcb : public IKey {
	unsigned char	keyData[ks];
	CKeyEcb(void* Key)
	{
		keySize		= ks;
		keyType		= SymmtericEcb;
		keyLength	= ks;
		memcpy(keyData,Key,keySize);
	}
	CKeyEcb()
	{
		keySize		= ks;
		keyType		= SymmtericEcb;
		keyLength	= ks;
		memset(keyData,0,keySize);
	}
	~CKeyEcb()
	{
		keySize		= sizeZero;
		keyType		= UnknownKey;
		keyLength	= 0;
		memset(keyData,0,ks);
	}
};

template<KeySize ks>
struct CKeyMV2 : public CKeyEcb<ks> 
{
	CKeyMV2()
	{
		keyType = MV2;
	}
};

template<KeySize ks,KeySize bs>
struct CKeyCbc : public CKeyEcb<ks> {
	KeySize			keyBlock;
	unsigned char	keySeed[bs];

	CKeyCbc(void* Key,void* Seed):CKeyEcb<ks>(Key)
	{
		keyType		= SymmtericCbc;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memcpy(keySeed,Seed,bs);
	}
	CKeyCbc():CKeyEcb<ks>()
	{
		keyType		= SymmtericCbc;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memset(keySeed,0,keyBlock);
	}
	~CKeyCbc()
	{
		keyBlock	= sizeZero;
		keyLength	= 0;
		memset(keySeed,0,bs);
	}
};
template<KeySize ks,KeySize bs>
struct CKeyGm : public CKeyEcb<ks> {
	KeySize			keyBlock;
	unsigned char	keySeed[bs];

	CKeyGm(void* Key,void* Seed):CKeyEcb<ks>(Key)
	{
		keyType		= SymmtericGm;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memcpy(keySeed,Seed,bs);
	}
	CKeyGm():CKeyEcb<ks>()
	{
		keyType		= SymmtericGm;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memset(keySeed,0,keyBlock);
	}
	~CKeyGm()
	{
		keyBlock	= sizeZero;
		keyLength	= 0;
		memset(keySeed,0,bs);
	}
};
template<KeySize ks,KeySize bs>
struct CKeySt : public CKeyEcb<ks> {
	KeySize			keyBlock;
	unsigned char	keySeed[bs];

	CKeySt(void* Key,void* Seed):CKeyEcb<ks>(Key)
	{
		keyType		= SymmtericSt;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memcpy(keySeed,Seed,bs);
	}
	CKeySt():CKeyEcb<ks>()
	{
		keyType		= SymmtericSt;
		keyBlock	= bs;
		keyLength	= ks + bs;
		memset(keySeed,0,keyBlock);
	}
	~CKeySt()
	{
		keyBlock	= sizeZero;
		keyLength	= 0;
		memset(keySeed,0,bs);
	}
};

template<KeySize ks>
struct CKeyRsaPublic : public IKey {
	unsigned char		keyModulus[ks];
	unsigned char		keyExponent[ks];

	CKeyRsaPublic(void* Modulus,void* Exponent)
	{
		keySize		= ks;
		keyType		= AsymmtericPublic;
		keyLength	= ks + ks;
		memcpy(keyModulus,Modulus,keySize);
		memcpy(keyExponent,Exponent,keySize);
	}
	CKeyRsaPublic()
	{
		keySize		= ks;
		keyType		= AsymmtericPublic;
		keyLength	= ks + ks;
		memset(keyModulus,0,keySize);
		memset(keyExponent,0,keySize);
	}
	~CKeyRsaPublic()
	{
		keySize		= sizeZero;
		keyType		= UnknownKey;
		keyLength	= 0;
		memset(keyModulus,0,ks);
		memset(keyExponent,0,ks);
	}
};

template<KeySize ks>
struct CKeyRsaPrivate : public IKey {
	unsigned char		keyModulus			[ks];
	unsigned char		keyPublicExponent	[ks];
	unsigned char		keyExponent			[ks];
	unsigned char		keyPrime			[2][ks>>1];
	unsigned char		keyPrimeExponent	[2][ks>>1];
	unsigned char		keyCoefficient		[ks>>1];

	CKeyRsaPrivate(void* Modulus,void* Exponent)
	{
		keySize		= ks;
		keyType		= AsymmtericPrivate;
		keyLength	= ks + ks + ks + ks + ks + (ks>>1);
		memcpy(keyModulus,Modulus,keySize);
		memcpy(keyExponent,Exponent,keySize);
	}
	CKeyRsaPrivate()
	{
		keySize		= ks;
		keyType		= AsymmtericPrivate;
		keyLength	= ks + ks + ks + ks + ks + (ks>>1);
		memset(keyModulus,0,keySize);
		memset(keyPublicExponent,0,keySize);
		memset(keyExponent,0,keySize);
		memset(keyPrime[0],0,keySize>>1);
		memset(keyPrime[1],0,keySize>>1);
		memset(keyPrimeExponent[0],0,keySize>>1);
		memset(keyPrimeExponent[1],0,keySize>>1);
		memset(keyCoefficient,0,keySize>>1);
	}
	~CKeyRsaPrivate()
	{
		keySize		= sizeZero;
		keyType		= UnknownKey;
		keyLength	= 0;
		memset(keyModulus,0,keySize);
		memset(keyPublicExponent,0,keySize);
		memset(keyExponent,0,keySize);
		memset(keyPrime[0],0,keySize>>1);
		memset(keyPrime[1],0,keySize>>1);
		memset(keyPrimeExponent[0],0,keySize>>1);
		memset(keyPrimeExponent[1],0,keySize>>1);
		memset(keyCoefficient,0,keySize>>1);
	}
};

template <KeySize ks>
struct CKeyDhPublic : public IKey {
	unsigned char	keyP[ks];
	CKeyDhPublic(void* P)
	{
		keySize = ks;
		keyType	= AsymmtericPublic;
		keyLength = (unsigned short)ks;
		memcpy(keyP,P,ks);
	}
	CKeyDhPublic()
	{
		keySize = ks;
		keyType	= AsymmtericPublic;
		keyLength = (unsigned short)ks;
		memset(keyP,0,ks);
	}
	~CKeyDhPublic()
	{
		keySize		= sizeZero;
		keyType		= UnknownKey;
		keyLength	= 0;
		memset(keyP,0,ks);
	}
};

template <KeySize ks>
struct CKeyDhPrivate : public IKey {
	unsigned char	keyX[ks];
	CKeyDhPrivate(void* X)
	{
		keySize = ks;
		keyType	= AsymmtericPrivate;
		keyLength = (unsigned short)ks;
		memcpy(keyX,X,ks);
	}
	CKeyDhPrivate()
	{
		keySize = ks;
		keyType	= AsymmtericPublic;
		keyLength = (unsigned short)ks;
		memset(keyX,0,ks);
	}
	~CKeyDhPrivate()
	{
		keySize		= sizeZero;
		keyType		= UnknownKey;
		keyLength	= 0;
		memset(keyX,0,ks);
	}
};

#endif //CRYPTOGRAPHY_KEYS_INTERFACE