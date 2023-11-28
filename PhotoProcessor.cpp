// PhotoProcessor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "MainFrm.h"
#include "slite.h"
//#include <Psapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;

ULONG_PTR gdiplusToken =NULL;
GdiplusStartupInput gdiplusStartupInput =NULL;


//Gdiplus::Bitmap * BMP1;
//Gdiplus::Bitmap * BMP2;
//HBITMAP hbmp;
//BITMAP bimg;
CBitmap * cBmp = 0;
CSize  src_sz(0,0);
//BITMAPINFOHEADER hbi;
//BITMAPINFO * bi = 0; 
CImage img1;
CImage img2;





// CPhotoProcessorApp

BEGIN_MESSAGE_MAP(CPhotoProcessorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPhotoProcessorApp::OnAppAbout)
END_MESSAGE_MAP()


// CPhotoProcessorApp construction

CPhotoProcessorApp::CPhotoProcessorApp()
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPhotoProcessorApp object

CPhotoProcessorApp theApp;


// CPhotoProcessorApp initialization

BOOL CPhotoProcessorApp::InitInstance()
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

	// initialisation GDIPLUS
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("PhotoProcessor"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	CString apppath = GetCommandLine() ;
	apppath = apppath.Mid(0,apppath.Find(_T("PhotoProcessor.exe"))-1);
	pFrame->m_startDir = apppath.Mid(1,apppath.ReverseFind(_T('\\')));
	pFrame->m_startDir.Replace(_T("\\"),_T("/"));
	pFrame->workPath = m_lpCmdLine;
//	DWORD derr;
	BOOL ret = SHGetSpecialFolderPath(pFrame->m_hWnd, 	pFrame->m_saveStartDir.GetBuffer(MAX_PATH ), CSIDL_LOCAL_APPDATA ,0);
	pFrame->m_saveStartDir.ReleaseBuffer();
	if (ret){
		pFrame->m_saveStartDir += _T("\\VYVphoto");
		::CreateDirectory(pFrame->m_saveStartDir,0);
	}
	pFrame->m_saveStartDir.Replace(_T("\\"),_T("/"));
	if (pFrame->workPath.IsEmpty()){
		pFrame->workPath = pFrame->m_saveStartDir;
	}
	else{
		pFrame->workPath.Trim(_T('\"'));			
		if (!CreateDirectory(pFrame->workPath,NULL)){
//			pFrame->workPath = _T("");
//			pFrame->workPath = m_lpCmdLine;
			if (GetLastError() != ERROR_ALREADY_EXISTS ){
				AfxMessageBox(_T("Incorrect parameter\r\nDefault place will be used"),MB_ICONINFORMATION);
				ret = SHGetSpecialFolderPath(pFrame->m_hWnd, 	pFrame->workPath.GetBuffer(MAX_PATH ), CSIDL_LOCAL_APPDATA ,0);
				pFrame->workPath.ReleaseBuffer();
				if (ret){
					pFrame->workPath += _T("\\VYVphoto");
					::CreateDirectory(pFrame->workPath,0);
				}
			}
		}
	}
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	pFrame->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME1),true);
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CPhotoProcessorApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CPhotoProcessorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPhotoProcessorApp message handlers

