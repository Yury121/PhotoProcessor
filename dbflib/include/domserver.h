#ifndef DOM_OBJECT_SERVER
#define DOM_OBJECT_SERVER

#include "dom.h"

#include "windows.h"
#include "olectl.h"

#pragma warning (disable:4995)

#undef BEGIN_INTERFACE_MAP
#undef END_INTERFACE_MAP

#define REG_ENTRY_POINT "Software\\TL_Dynamic Object Model"
extern volatile long ServerReffernceCount;

#ifndef EXCLUDE_DLL_ENTRY
HINSTANCE	g_hModuleHandle;
#ifndef MFC_DLL
HINSTANCE GetDllHandle() {return g_hModuleHandle;}
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
if(ul_reason_for_call == DLL_PROCESS_ATTACH){
g_hModuleHandle = reinterpret_cast<HINSTANCE>(hModule);
DisableThreadLibraryCalls(static_cast<HINSTANCE>(hModule));
}
return TRUE;
}
#else
	HINSTANCE GetDllHandle() {return AfxGetInstanceHandle();}
#endif
#else
extern HINSTANCE	g_hModuleHandle;
HINSTANCE GetDllHandle();
#endif

#pragma warning (disable:4250)

template <class CLASS>
class CBase : virtual public IBase 
{
public:
	long		m_refCount;
	CBase():m_refCount(0){;}
	virtual ~CBase(){;}
	static void* SelfCreate(const UIID& uiid)
	{
		CLASS *cls = new CLASS;
		if(cls != NULL){
			void* intfc; 
			cls->QueryInterface(uiid,&intfc);
			return intfc;
		}
		return NULL;
	}
	virtual long __stdcall AddRef()
	{
#ifdef MFC_DLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		InterlockedIncrement(&ServerReffernceCount);
		return InterlockedIncrement(&m_refCount);
	}
	virtual long __stdcall Release()
	{
#ifdef MFC_DLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		InterlockedDecrement(&ServerReffernceCount);
		long RefCount = InterlockedDecrement(&m_refCount);
		if(RefCount == 0){
			delete static_cast<CLASS*>(this);
			return 0;
		}
		return RefCount;
	}
	virtual long __stdcall QueryInterface (const UIID& iid,void** ppv);
	static UIID GetUIID();
};

#define BEGIN_INTERFACE_MAP(CLS,CLS_UIID)\
	UIID CBase<CLS>::GetUIID(){return CLS_UIID;}\
	long __stdcall CBase<CLS>::QueryInterface(const UIID& iid,void** ppv)\
{\
	AFX_MANAGE_STATE(AfxGetStaticModuleState());\
typedef CLS CLSTYPE;\
*ppv = NULL;\
if(iid == UIID_IBase) { *ppv = static_cast<IBase*>((CLSTYPE*)this); AddRef(); return 0; }
#define ENTRY(i_i,i_id)\
	if(i_id == iid) { *ppv = static_cast<i_i*>((CLSTYPE*)this); AddRef(); return 0; }
#define END_INTERFACE_MAP return -1; }

#define BEGIN_CLASS_EXPORT struct {UIID iid;void* pfnC;} exports_table[] = {
#define CLASS(CLS) {CBase<CLS>::GetUIID(),CBase<CLS>::SelfCreate},
#define END_CLASS_EXPORT {UIID(0,0,0),NULL}};\
	volatile long ServerReffernceCount = 0;\
/*	long __stdcall DllRegisterServer(){\
	AFX_MANAGE_STATE(AfxGetStaticModuleState());\
	char pszUIID[MAX_PATH]; char pszPathToComponent[2048];\
	g_hModuleHandle = GetDllHandle();\
	GetModuleFileName(g_hModuleHandle,pszPathToComponent,MAX_PATH);\
	HKEY hKey;\
	for(int i=0;exports_table[i].iid != UIID(0,0,0);i++)\
{\
	wsprintf(pszUIID,"%s\\{%08X-%08X-%08X}",REG_ENTRY_POINT,exports_table[i].iid.d1,exports_table[i].iid.d2,exports_table[i].iid.d3);\
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,pszUIID,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL) == ERROR_SUCCESS)\
{\
	DWORD dwType;\
	DWORD dwSize = MAX_PATH;\
	DWORD dwRef = 0;\
	RegQueryValueEx(hKey,"Reference",NULL,&dwType,(LPBYTE)(&dwRef),&dwSize);\
	dwRef ++;\
	RegSetValueEx(hKey,"Reference",NULL,REG_DWORD,(LPBYTE)&dwRef,sizeof(DWORD));\
	RegSetValueEx(hKey,"UIID",NULL,REG_BINARY,(LPBYTE)&(exports_table[i].iid),sizeof(UIID));\
	RegSetValueEx(hKey,"Inproc Server",NULL,REG_SZ,(LPBYTE)pszPathToComponent,lstrlen(pszPathToComponent)+1);\
	RegCloseKey (hKey);\
}\
}\
	return 0;\
	}\
	long __stdcall DllUnregisterServer(){\
	AFX_MANAGE_STATE(AfxGetStaticModuleState());\
	CHAR pszUIID[MAX_PATH];\
	bool bUndelete = false;\
	for(int i=0;exports_table[i].iid != UIID(0,0,0);i++){\
	wsprintf(pszUIID,"%s\\{%08X-%08X-%08X}",REG_ENTRY_POINT,exports_table[i].iid.d1,exports_table[i].iid.d2,exports_table[i].iid.d3);\
	DWORD dwRef = 0;\
	HKEY hKey;\
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,pszUIID,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)\
	{\
		DWORD dwType;\
		DWORD dwSize = MAX_PATH;\
		RegQueryValueEx(hKey,"Reference",NULL,&dwType,(LPBYTE)(&dwRef),&dwSize);\
		dwRef --;\
		RegSetValueEx(hKey,"Reference",NULL,REG_DWORD,(LPBYTE)&dwRef,sizeof(DWORD));\
		RegCloseKey (hKey);\
		bUndelete = true;\
	}\
	if(dwRef == 0){\
		RegDeleteKey(HKEY_LOCAL_MACHINE,pszUIID);\
	}}\
	return (bUndelete == true)?SELFREG_E_TYPELIB:S_OK;\
	}*/\
	long __stdcall DllCanUnloadNow(){AFX_MANAGE_STATE(AfxGetStaticModuleState()); return (ServerReffernceCount == 0);}\
	bool __stdcall DllCreateInstance(const UIID& iid,void** ppv){\
	AFX_MANAGE_STATE(AfxGetStaticModuleState());\
	if(ppv == NULL)return false;\
	for(int i=0;exports_table[i].iid != UIID(0,0,0);i++){\
	if(exports_table[i].iid == iid && exports_table[i].pfnC != NULL){\
	typedef IBase* (*fnc)(const UIID& iid);\
	*ppv = (reinterpret_cast<fnc>(exports_table[i].pfnC))(iid);\
	return (*ppv != NULL);\
	}}return false;}

#endif //DOM_OBJECT_SERVER