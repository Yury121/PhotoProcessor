#pragma once

#include <windows.h>
#include <map>
#include <string>
#include "functor.h"

namespace mt {

	inline bool IsValidHandle(HANDLE hHandle)
	{
		return ((hHandle != NULL) && (hHandle != ((void*)(LONG_PTR)-1)));
	}

	template <class LockType>
	class autolock {
	public:
		inline explicit autolock(LockType &refLockObject) : m_refLockObject(refLockObject) 
		{m_refLockObject.Lock(INFINITE);}
		inline explicit autolock(LockType &refLockObject, DWORD nTimeOut) : m_refLockObject(refLockObject) 
		{m_refLockObject.Lock(nTimeOut);}
		inline ~autolock()
		{m_refLockObject.Unlock();}
	protected:
		LockType &m_refLockObject;
	};

	class securityattributes : public _SECURITY_ATTRIBUTES {
	protected:
		SECURITY_DESCRIPTOR m_SID;
	public:
		securityattributes()
		{
			nLength              = sizeof( _SECURITY_ATTRIBUTES );
			lpSecurityDescriptor = NULL;
			bInheritHandle       = FALSE;
			memset(&m_SID,0,sizeof(SECURITY_DESCRIPTOR));
		}
		~securityattributes()
		{
			nLength              = sizeof( _SECURITY_ATTRIBUTES );
			lpSecurityDescriptor = NULL;
			bInheritHandle       = FALSE;
			memset(&m_SID,0,sizeof(SECURITY_DESCRIPTOR));
		}
		securityattributes(BOOL bInherit,DWORD dwRevision = SECURITY_DESCRIPTOR_REVISION,BOOL bDaclPresent = TRUE,PACL pAcl = NULL,BOOL bDaclDefault = FALSE)
		{
			InitializeSecurityDescriptor(&m_SID, SECURITY_DESCRIPTOR_REVISION);

			SetSecurityDescriptorDacl(&m_SID, bDaclPresent, pAcl, bDaclDefault);
			nLength              = sizeof( _SECURITY_ATTRIBUTES );
			lpSecurityDescriptor = &m_SID;
			bInheritHandle       = bInherit;

			SECURITY_ATTRIBUTES sa = { sizeof (SECURITY_ATTRIBUTES), &m_SID, bInherit };
		}
	};

	class event {
	protected:
		HANDLE	m_hHandle;
	public:
		event(const char* name, bool Inherit, long Access)
		{
			m_hHandle = ::OpenEvent( Access, Inherit, name);
		}
		event(bool ManualReset, bool State, const char* name,securityattributes* pSA = NULL)
		{
			m_hHandle = ::CreateEvent( pSA, ManualReset, State, name);
		}
		~event(void)
		{
			if(IsValidHandle(m_hHandle) == TRUE)
				CloseHandle(m_hHandle);
			m_hHandle = NULL;
		}
		inline BOOL Set(void)
		{
			return SetEvent(m_hHandle);
		}
		inline BOOL Reset(void)
		{
			return ResetEvent(m_hHandle);
		}
		inline BOOL Pulse(void)
		{
			return PulseEvent(m_hHandle);
		}
		inline BOOL Unlock(void)
		{
			return ResetEvent(m_hHandle);
		}
		inline BOOL Lock(long TimeOut = INFINITE)
		{
			if((m_hHandle == INVALID_HANDLE_VALUE))
				return FALSE;
			if(WaitForSingleObject(m_hHandle,TimeOut) == WAIT_OBJECT_0)
				return Set();
			return FALSE;
		}
		operator HANDLE () {return m_hHandle;}
	};

	class mutex {
	protected:
		HANDLE		m_hHandle;
	public:
		mutex(bool LockObject = false,const char* name = NULL,securityattributes* pSA = NULL)
		{
			m_hHandle = ::CreateMutex(pSA,LockObject,name);
		}
		mutex(const char* name,bool Inherit = false,long Access = MUTEX_ALL_ACCESS)
		{
			m_hHandle = ::OpenMutex(Access,Inherit,name);
		}
		~mutex(void)
		{
			if(IsValidHandle(m_hHandle) == TRUE)
				CloseHandle(m_hHandle);
			m_hHandle = NULL;
		}
		inline BOOL Lock(long TimeOut = INFINITE)
		{
			return (WaitForSingleObject(m_hHandle,TimeOut) == WAIT_OBJECT_0);
		}
		inline BOOL Unlock(void)
		{
			return ReleaseMutex(m_hHandle);
		}
		operator HANDLE () {return m_hHandle;}
	};

	class optex {
	protected:
		struct SHARED {
			unsigned long	m_dwSpinCount;
			long			m_lLockCount;
			unsigned long	m_dwThreadId;
			long  m_lRecurseCount;
		} *m_psi;
		HANDLE      m_hevt;
		HANDLE      m_hfm;
	public:
		optex(const char* name = NULL,long nSpinCount = 4000)
		{
			char szResult[100];
			wsprintf(szResult,"OPTEX_E_%s",name);	m_hevt = CreateEvent(NULL, FALSE, FALSE, szResult);
			wsprintf(szResult,"OPTEX_M_%s",name);	
			m_hfm = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,PAGE_READWRITE, 0, sizeof(*m_psi), szResult);
			m_psi = (SHARED*) MapViewOfFile(m_hfm,FILE_MAP_WRITE, 0, 0, 0);
		}
		~optex(void)
		{
			if(IsValidHandle(m_hevt) == TRUE)
				CloseHandle(m_hevt);
			if(m_psi != NULL)
				UnmapViewOfFile(m_psi);
			if(m_hfm != NULL)
				CloseHandle(m_hfm);
		}
		inline BOOL TryEnter(long nSpinCount)
		{
			if(m_psi == NULL)
				return TRUE;

			DWORD dwThreadId = GetCurrentThreadId();
			BOOL fThisThreadOwnsTheOptex = FALSE;     
			DWORD dwSpinCount = nSpinCount; 
			do {
				fThisThreadOwnsTheOptex = (0 == 
					InterlockedCompareExchange(&m_psi->m_lLockCount, 1, 0)); 

				if (fThisThreadOwnsTheOptex) {
					m_psi->m_dwThreadId = dwThreadId;
					m_psi->m_lRecurseCount = 1;

				} else {
					if (m_psi->m_dwThreadId == dwThreadId) {
						InterlockedIncrement(&m_psi->m_lLockCount);
						m_psi->m_lRecurseCount++;
						fThisThreadOwnsTheOptex = TRUE;
					}
				}
			} while (!fThisThreadOwnsTheOptex && (dwSpinCount-- > 0));
			return(fThisThreadOwnsTheOptex);
		}
		inline BOOL Lock(long TimeOut = INFINITE)
		{
			if (TryEnter(TimeOut) == TRUE || m_psi == NULL) 
				return true;  

			DWORD dwThreadId = GetCurrentThreadId();
			if (InterlockedIncrement(&m_psi->m_lLockCount) == 1) {
				m_psi->m_dwThreadId = dwThreadId;
				m_psi->m_lRecurseCount = 1;

			} else {
				if (m_psi->m_dwThreadId == dwThreadId) {
					m_psi->m_lRecurseCount++;

				} else {
					WaitForSingleObject(m_hevt, INFINITE);
					m_psi->m_dwThreadId = dwThreadId;
					m_psi->m_lRecurseCount = 1;
				}
			}
			return FALSE;
		}
		inline BOOL Unlock(void)
		{
			if(m_psi == NULL)
				return TRUE;

			if (--m_psi->m_lRecurseCount > 0) {
				InterlockedDecrement(&m_psi->m_lLockCount);
			} else {
				m_psi->m_dwThreadId = 0;
				if (InterlockedDecrement(&m_psi->m_lLockCount) > 0) {
					SetEvent(m_hevt);
				}
			}
			return TRUE;
		}
	};

	class criticalsection {
	protected:
		CRITICAL_SECTION	m_hHandle;
	public:
		criticalsection(void)
		{
			::InitializeCriticalSection(&m_hHandle);
		}
		~criticalsection(void)
		{
			::DeleteCriticalSection( &m_hHandle);
		}
		inline void Lock(long Unused = INFINITE)
		{
			::EnterCriticalSection( &m_hHandle);
		}
		inline void Unlock(void)
		{
			::LeaveCriticalSection( &m_hHandle);
		}
	};
	typedef autolock<event>				AutoEvent;
	typedef autolock<mutex>				AutoMutex;
	typedef autolock<optex>				AutoOptex;
	typedef autolock<criticalsection>	AutoCriticalSection;

	class mapped {
	protected:
		optex	syncControlAccess;
		void*	remap(unsigned long LBA,long LBS);
		long	nPageSize;
		void*	pMappedRegion;
		unsigned long nMappedOffset;
		HANDLE	hFileMapped;
		HANDLE	hMemoryMapped;
		std::string	szMemoryName;
	public:
		mapped(const char* szFileName,const char* szName,long PageSize = 65536);
		mapped(const char* szName,long PageSize = 65536);
		~mapped();
		void Resize(unsigned long nNewSize);
		inline BOOL Lock(long SpinLock = INFINITE)
		{
			return syncControlAccess.Lock(SpinLock);
		}
		inline BOOL Unlock(void)
		{
			return syncControlAccess.Unlock();
		}
		inline unsigned char* operator ()(unsigned long LBA,long LBS = -1)
		{
			return static_cast<unsigned char*>(remap(LBA,LBS));
		}
	};

	class thread
	{
	protected:
		friend class pool;
		ULONG	threadState;
		ULONG	threadId;
		void*	threadHandle;
		event	threadGuard;
		void*	threadParam;
	public:
		BOOL Create(unsigned long (__stdcall *ThreadFunction)(thread* thread),void* pParam,BOOL bSuspend = FALSE);
		BOOL Terminate();
		BOOL Suspend(void);
		BOOL Resume(void);
		inline ULONG GetId() {return threadId;}
		inline HANDLE GetHandle() {return threadHandle;}
		template<typename TYPE>
			inline TYPE* GetParam() {return (TYPE*)threadParam;}
			inline BOOL IsPossible(DWORD dwTimeOut = 10) 
			{
				return (WaitForSingleObject(threadGuard,dwTimeOut) == WAIT_TIMEOUT);
			}
			thread(void);
			~thread(void);
	};

	class pool {
	protected:
		enum {Idle = 1,Busy,Normal};
		typedef std::map<ULONG,thread*>		Map;
		typedef Map::iterator				It;
		typedef Map::value_type				Value;
		Map									taskJobs;
		thread								taskManager;
		static unsigned long __stdcall ManagementThread(thread* thread);
		static unsigned long __stdcall WorkerThread(thread* thread);
		inline BOOL InsertJob();
		inline void RemoveJobs();
		inline void RemoveJob(unsigned long ThreadId);
		inline void ChangeJobState(unsigned long Tid,long State);
		inline long GetStatus();
	protected:
		long								taskManageTime;
		long								taskInitialTasks;
		long								taskMaximumTasks;
		criticalsection						taskGuard;
		HANDLE								taskIoPort;
		long								taskJobsState;
	public:
		pool();
		~pool();
		inline long GetManageTime() {return taskManageTime;}
		inline void SetManageTime(long Time) {taskManageTime = Time;}
		BOOL Initialize(long InitialTasksCount = 2,long MaxTasksCount = 10);
		BOOL Finalize();
		//void Execute(unsigned long (__stdcall *ThreadFunction)(void*),void* pParam);
		void Execute(Functor* pfn,void* pParam);
	};

}

using namespace mt;