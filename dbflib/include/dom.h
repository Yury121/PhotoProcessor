#ifndef dom_base_header
#define dom_base_header

#include <map>
#include <string>
#include "assert.h"

#include "domerr.h"

namespace TLDOM {
#ifndef TLDOM_UIID_DECLARATION
#define TLDOM_UIID_DECLARATION
	
	/* Структура описывающая уникальный идентификатор объекта */
	class UIID {
	public:
		unsigned long	d1,d2,d3;
		UIID(){d1 = d2 = d3 = 0;}
		UIID(unsigned long dd1,unsigned long dd2,unsigned long dd3){d1 = dd1; d2 = dd2; d3 = dd3;}
		~UIID(){d1 = d2 = d3 = 0;}
		UIID& operator = (const UIID& id){d1 = id.d1; d2 = id.d2; d3 = id.d3; return *this;}
		bool operator == (const UIID& id) const {return ((d1 == id.d1)&&(d2 == id.d2)&&(d3 == id.d3));}
		bool operator != (const UIID& id) const {return !((d1 == id.d1)&&(d2 == id.d2)&&(d3 == id.d3));}
		bool operator < (const UIID& id) const	{return (memcmp(&d1,&id.d1,sizeof(unsigned long) * 3) < 0);}
		bool operator > (const UIID& id) const	{return (memcmp(&d1,&id.d1,sizeof(unsigned long) * 3) > 0);}
	};

#define UIID_ZERO		UIID(0x00000000,0x00000000,0x00000000)
#define UIID_IBase		UIID(0x00000000,0x00000000,0x00000001)
#define UIID_IMemory	UIID(0x00000000,0x00000000,0x00000002)
#define UIID_IIoStream	UIID(0x00000000,0x00000000,0x00000003)

#endif /* Базовый интерфейс объектов TLDOM */


	struct IBase {
		virtual long __stdcall AddRef(void) = 0;
		virtual long __stdcall Release(void) = 0;
		virtual long __stdcall QueryInterface(const UIID &itfid, void **ppInterface) = 0;
	};

	template <class T>
	class PTR
	{
	public:
		PTR() throw()
		{
			m_pDomPtr = NULL;
		}
		PTR(T* lp) throw()
		{
			m_pDomPtr = lp;
			if (m_pDomPtr != NULL)
				m_pDomPtr->AddRef();
		}
		PTR(const PTR<T>& lp) throw() :	m_pDomPtr(lp.m_pDomPtr)
		{
			if (m_pDomPtr != NULL)
				m_pDomPtr->AddRef();
		}
		PTR<T> &operator=(const PTR<T> &spPtr)
		{
			Release();
			m_pDomPtr=spPtr.m_pDomPtr;
			if(m_pDomPtr!=NULL)
				m_pDomPtr->AddRef();
			return *this;
		}
		~PTR() throw()
		{
			if (m_pDomPtr)
				m_pDomPtr->Release();
		}
		operator T*() const throw()	{return m_pDomPtr;}
		T& operator*() const throw(){return *m_pDomPtr;}
		T** operator&() throw(){ return &m_pDomPtr;	}
		T* operator->() const throw(){return m_pDomPtr;	}
		bool operator!() const throw(){return (m_pDomPtr == NULL);}
		bool operator<(T* pT) const throw()	{return m_pDomPtr < pT;	}
		bool operator==(T* pT) const throw(){return m_pDomPtr == pT;}

		void Release() throw()
		{
			T* pTemp = m_pDomPtr;
			if (pTemp)
			{
				m_pDomPtr = NULL;
				pTemp->Release();
			}
		}
		void Attach(T* p2 /*=NULL*/) throw()
		{
			if (m_pDomPtr)
				m_pDomPtr->Release();
			m_pDomPtr = p2;
		}
		T* Detach() throw()
		{
			T* pt = m_pDomPtr;
			m_pDomPtr = NULL;
			return pt;
		}
		long CreateInstance(const UIID &objid,const char* rawDll) throw()
		{
			Release();
			T* pt = m_pDomPtr;
			long Result = (TLDomCreateInstance2(objid, (void **)&pt,rawDll) > 0)?1:0;
			if(Result == 1) {
				if(T::__GetUIID() != objid) {
					Result = (pt->QueryInterface(T::__GetUIID(),(void **)&m_pDomPtr) == 0)?1:0;
					pt->Release();
				}
				else {
					m_pDomPtr = pt;
				}
			}
			return Result;
		}
		long CreateInstance(const UIID &objid) throw()
		{
			Release();
			T* pt = m_pDomPtr;
			long Result = (TLDomCreateInstance(objid, (void **)&pt) > 0)?1:0;
			if(Result == 1) {
				if(T::__GetUIID() != objid) {
					Result = (pt->QueryInterface(T::__GetUIID(),(void **)&m_pDomPtr) == 0)?1:0;
					pt->Release();
				}
				else {
					m_pDomPtr = pt;
				}
			}
			return Result;
		}
		template <typename IRequiredInterface>
			long QueryInterface(const UIID &objid, IRequiredInterface **ppIRequired) const
		{
			return (m_pDomPtr->QueryInterface(objid, (void **)ppIRequired) == 0)?1:0;
		}
		T* m_pDomPtr;
	};
}

using namespace TLDOM;

#define TLDOM_DECLARE_UIID(_uiid_)	inline static TLDOM::UIID __GetUIID(void) { return _uiid_; }

struct IMemory : public virtual TLDOM::IBase 
{
	TLDOM_DECLARE_UIID(UIID_IMemory)

	virtual void* __stdcall	Alloc(long iSize) = 0;
	virtual void* __stdcall	ReAlloc(void* pm,long iSize) = 0;
	virtual void __stdcall	Free(void* pm) = 0;
	virtual long __stdcall	GetSize(void* pm) = 0;
	virtual long __stdcall	Compact() = 0;
};

struct IIoStream : public virtual TLDOM::IBase
{
	TLDOM_DECLARE_UIID(UIID_IIoStream)

	enum {Begin,Current,End};

	virtual void __stdcall		Attach(void* hHandle,__int64 nSize = -1) = 0;
	virtual void* __stdcall		Detach() = 0;
	virtual long __stdcall		Read(void* pBuffer,long nRead,long nPos = -1) = 0;
	virtual long __stdcall		Write(void* pBuffer,long nWrite,long nPos = -1) = 0;
	virtual __int64 __stdcall	SetSize(__int64 nPos) = 0;
	virtual __int64 __stdcall	Seek(__int64 nPos,int from) = 0;
	virtual __int64 __stdcall	GetLength() = 0;
	virtual __int64 __stdcall	GetCursor() = 0;
};


extern "C" long __stdcall TLDomCreateInstance(const UIID &objid,void ** ppv);
extern "C" long __stdcall TLDomCreateInstance2(const UIID &objid,void ** ppv,const char* rawDll);
extern "C" long __stdcall TLDomInitalize();
extern "C" long __stdcall TLDomFinalize();
extern "C" long __stdcall TLDomGetMemory(void** ppv);
extern "C" long __stdcall TLDomCreateFileStream(IIoStream** ppv,const char* szFileName);
extern "C" long __stdcall TLDomCreateMemStream(IIoStream** ppv,long nInitialSize,long nGranularity = 4096);

#endif //dom_base_header

#ifndef TLDOM_TYPE_DEFINATION
#define TLDOM_TYPE_DEFINATION
	typedef unsigned char	byte;
#endif

#define failed(e) (e != E_OK)
#define success(e) (e == E_OK)

#pragma warning (disable:4996)
#pragma warning (disable:4018)
#pragma warning (disable:4267)
#pragma warning (disable:4244)
#pragma warning (disable:4800)
