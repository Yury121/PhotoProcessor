#pragma once

#define EXCLUDE_DLL_ENTRY
#include <Authorization.h>
#include <domserver.h>
#include <Smart.h>

//#define EXCERBER_ALWAYS_CLEAR_RECENT	0x00000001
//#define EXCERBER_USE_USERNAME_FIELD		0x00000002

class CCerberAuthorization :
	public CBase<CCerberAuthorization>,
	public IFWAuthorization
{
public:
	CCerberAuthorization(void);
	~CCerberAuthorization(void);
	TLDOM::memory	m_Password;
	std::string m_strUserName;

	virtual RESULT __stdcall Prompt(const wchar_t* szUserName = NULL,ULONG dwModeFlags = 0);
	virtual RESULT __stdcall Change(const wchar_t* szUserName = NULL,ULONG dwModeFlags = 0);
//	virtual RESULT __stdcall SetUiString(int idString,const wchar_t* szString){return E_FAILED;}
	virtual RESULT __stdcall SetUiString(int idString,const wchar_t* szString);
	virtual RESULT __stdcall Retrieve(wchar_t* szUserName,void* pPassword,ULONG dwSize);
};
