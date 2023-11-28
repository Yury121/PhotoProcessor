// TestDb.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "slite.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestDbApp:
// See TestDb.cpp for the implementation of this class
//



class CTestDbApp : public CWinApp
{
public:
	CTestDbApp();
	~CTestDbApp() {
//		if (sl_db != nullptr) 	sqlite3_close(sl_db);
		
	};

// Overrides
	public:
	virtual BOOL InitInstance();
	//HMODULE hSL = NULL;
	//sqlite3 *sl_db = nullptr;


	


// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestDbApp theApp;