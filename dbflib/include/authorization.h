#pragma once

#include <windows.h>
#include "dom.h"
#include "properties.h"


#define UIID_IFirmwareAuthorizationSimple		UIID(0x1F54BBE5,0x44cc883E,0xAA99715C)
#define UIID_IFirmwareAuthorization2			UIID(0x1F54BBE5,0x44cc883E,0xAA99715D)
#define UIID_IFirmwareAuthorizationToken		UIID(0x1F54BBE5,0x44cc883E,0x600156A7)
#define UIID_IFirmwareAuthorizationCerber		UIID(0x1F54BBE5,0x44cc883E,0x896B3EF1)
#define UIID_IUserInterface						UIID(0x1F54BBE5,0x44cc883E,0x896B3EF2)

#define UIID_IAuthorizationSimple	UIID_IFirmwareAuthorizationSimple
#define UIID_IAuthorization2		UIID_IFirmwareAuthorization2
#define UIID_IAuthorization			UIID(0x1F54BBE5,0x44cc883E,0xAA99715C)
#define UIID_IAuthorizationToken	UIID_IFirmwareAuthorizationToken
#define UIID_IAuthorizationCerber	UIID_IFirmwareAuthorizationCerber

#define MAKE_TIMEOUT(Second) MAKELONG(Second,0x8000)

#define E_AUTH_BUFSIZE				MAKE_RESULT(1,0x0001,0x0001)
#define E_AUTH_PASSCHANGE			MAKE_RESULT(1,0x0001,0x0002)

#define AUTH_DISABLE_PASSCHANGE		0x00000001
#define AUTH_DISABLE_OK_PASSCHANGE	0x00000002


enum AuthPropId{txtAuthCaption};

struct Authorization {
	wchar_t		userName[64];
	byte*		userPassword;
	byte*		userOldPassword;
	long		userPasswordLength;
public:
	void Clear()
	{
		memset(userName,0,sizeof(wchar_t) * 64);
		if(userPassword != NULL)
			memset(userPassword,0,sizeof(byte) * userPasswordLength);
		if(userOldPassword != NULL)
			memset(userOldPassword,0,sizeof(byte) * userPasswordLength);
	}
	Authorization(long PassLength)
	{
		userPasswordLength = PassLength; 
		userPassword = new byte [PassLength];
		userOldPassword = new byte [PassLength];
		Clear();
	}
	~Authorization()
	{
		Clear();
		delete[] userOldPassword;
		delete[] userPassword;
	}
};

struct IFWAuthorization2 : public virtual IBase
{
	TLDOM_DECLARE_UIID(UIID_IAuthorization2)
	virtual RESULT __stdcall Ask(Authorization& auth,ULONG dwModeFlags = 0) = 0;
};
struct IFWAuthorization : public virtual IBase
{
	TLDOM_DECLARE_UIID(UIID_IAuthorization);
	virtual RESULT __stdcall Prompt(const wchar_t* szUserName = NULL,ULONG dwModeFlags = 0) = 0;
	virtual RESULT __stdcall Change(const wchar_t* szUserName = NULL,ULONG dwModeFlags = 0) = 0;
	virtual RESULT __stdcall SetUiString(int idString,const wchar_t* szString) = 0;
	virtual RESULT __stdcall Retrieve(wchar_t* szUserName,void* pPassword,ULONG dwSize) = 0;
};
