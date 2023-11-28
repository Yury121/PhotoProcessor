// TestDb.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TestDb.h"
#include "TestDbDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;

ULONG_PTR gdiplusToken =NULL;
GdiplusStartupInput gdiplusStartupInput =NULL;


// CTestDbApp

BEGIN_MESSAGE_MAP(CTestDbApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestDbApp construction

CTestDbApp::CTestDbApp()
{
#if 0
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	//sqlite3_os_init();//
	int res = sqlite3_open("test.db", &sl_db);
	if (res != 0) return; //error
	char* cerr = 0;
	res = sqlite3_exec(sl_db, sql_imgref, 0, 0, &cerr);
	if (res) {
		// cout << cerr
		sqlite3_free(cerr);
	}
	res = sqlite3_exec(sl_db, sql_imgset, 0, 0, &cerr);
	if (res) {
		// cout << cerr
		sqlite3_free(cerr);
	}
	res = sqlite3_exec(sl_db, sql_indModel, 0, 0, &cerr);
	if (res) {
		sqlite3_free(cerr);
	}
	res = sqlite3_exec(sl_db, sql_faces, 0, 0, &cerr);
	if (res) {
		// cout << cerr
		sqlite3_free(cerr);
	}
	res = sqlite3_exec(sl_db, sql_indHash, 0, 0, &cerr);
	if (res) {
		sqlite3_free(cerr);
	}
#endif


}


// The one and only CTestDbApp object

CTestDbApp theApp;


// CTestDbApp initialization

BOOL CTestDbApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CTestDbDlg dlg;
	m_pMainWnd = &dlg;
	bool ret = SHGetSpecialFolderPath(NULL, dlg.workPath.GetBuffer(MAX_PATH ), CSIDL_LOCAL_APPDATA ,0);
	dlg.workPath.ReleaseBuffer();
	if (ret){
		dlg.workPath += _T("\\VYVphoto");
		::CreateDirectory(dlg.workPath,0);
	}
	dlg.workPath.Replace(_T("\\"),_T("/"));



	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}
