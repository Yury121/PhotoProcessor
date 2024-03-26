// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "PhotoProcessor.h"

#include "MainFrm.h"
#include <Gdiplus.h>
#include <vector>
#include <string>
#include "cvdface.h"
#include "DbOldDlg.h"
#include "FacesDlg.h"
#include "slite.h"
//#include "./dbflib/dbflib.h"
#include "ProgressDlg.h"
#include "GistDlg.h"
#include <math.h>
#include "IntellCNN.h"
#include "COptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
extern ULONG_PTR gdiplusToken;
//extern int GetFileHash(LPCTSTR fpath, unsigned char hash[16]);
//extern int ParseExif(LPCTSTR src,int szsrc, LPCTSTR out, int szout);
CPen rgbPen(PS_SOLID, 0, RGB(255,0,0));
using namespace Gdiplus;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(IDC_OPENIMAGE, &CMainFrame::OnOpenimage)
	ON_COMMAND(ID_DINDIAP, &CMainFrame::OnDindiap)
	ON_COMMAND(ID_GAUSS, &CMainFrame::OnGauss)
	ON_COMMAND(ID_GAMMA_0, &CMainFrame::OnGamma0)
	ON_COMMAND(ID_GAMMA_1, &CMainFrame::OnGamma1)
	ON_COMMAND(ID_GAMMA_2, &CMainFrame::OnGamma2)
	ON_COMMAND(ID_GAMMA_3, &CMainFrame::OnGamma3)
	ON_COMMAND(ID_GAMMA_4, &CMainFrame::OnGamma4)
	ON_COMMAND(ID_GAMMA_5, &CMainFrame::OnGamma5)
	ON_COMMAND(ID_GAMMA_6, &CMainFrame::OnGamma6)
	ON_COMMAND(ID_GAMMA_7, &CMainFrame::OnGamma7)
	ON_COMMAND(ID_GAMMA_8, &CMainFrame::OnGamma8)
	ON_COMMAND(ID_GAMMA_BIG, &CMainFrame::OnGammaBig)
	ON_COMMAND(IDC_VIEWONEONE, &CMainFrame::OnSetOneToOne)
	ON_COMMAND(ID_VIEW_ALLCOLAR, &CMainFrame::OnViewAllcolar)
	ON_COMMAND(ID_VIEW_GRAYSCALE, &CMainFrame::OnViewGrayscale)
	ON_COMMAND(ID_VIEW_RED, &CMainFrame::OnViewRed)
	ON_COMMAND(ID_VIEW_BLUE, &CMainFrame::OnViewBlue)
	ON_COMMAND(ID_VIEW_GREEN, &CMainFrame::OnViewGreen)
	ON_COMMAND(ID_FILE_SAVESCREEN, &CMainFrame::OnFileSavescreen)
	ON_COMMAND(ID_TOOLS_INVERSE, &CMainFrame::OnToolsInverse)
	ON_COMMAND(IDC_SOBEL, &CMainFrame::OnSobel)
	ON_COMMAND(ID_TOOLS_INTEGRATOR, &CMainFrame::OnToolsIntegrator)
	ON_COMMAND(ID_TOOLS_TESTFACES, &CMainFrame::OnToolsTestfaces)
	ON_COMMAND(ID_EXIF, &CMainFrame::OnExif)
	ON_COMMAND(ID_MINPICT, &CMainFrame::OnMinpict)
	ON_COMMAND(IDC_SAVE_SQRT, &CMainFrame::OnSaveSqrt)
	ON_COMMAND(IDC_OPEN_OLD_IMAGE, &CMainFrame::OnOpenOldImage)
	ON_COMMAND(ID_SHOW_FACES, &CMainFrame::OnShowFaces)
	ON_COMMAND(ID_DIR_SCAN, &CMainFrame::OnDirScan)
	ON_COMMAND(ID_VIEW_SHOWALLFACES, &CMainFrame::OnViewShowallfaces)
	ON_COMMAND(ID_OPEN_FOLDER, &CMainFrame::OnOpenFolder)
	ON_COMMAND(ID_SCAN_TO_OLD, &CMainFrame::OnScanToOld)
	ON_COMMAND(ID_GISTDLG, &CMainFrame::OnGistdlg)
	ON_COMMAND(ID_CONTURMAX, &CMainFrame::OnConturmax)
	ON_COMMAND(ID_CONTUREQU, &CMainFrame::OnConturequ)
	ON_COMMAND(ID_CONTURMIN, &CMainFrame::OnConturmin)
	ON_COMMAND(ID_CONTURLEVAL, &CMainFrame::OnConturleval)
	ON_COMMAND(ID_OPER_RESTORE, &CMainFrame::OnOperRestore)
	ON_COMMAND(ID_TOOLS_RESTOREORIGINALPATH, &CMainFrame::OnToolsRestoreoriginalpath)
	ON_COMMAND(ID_SELGOODPOINT, &CMainFrame::OnSelgoodpoint)
	ON_COMMAND(ID_FILTERGRADX, &CMainFrame::OnFiltergradx)
	ON_COMMAND(ID_FILTERGRADY, &CMainFrame::OnFiltergrady)
	ON_COMMAND(ID_GRADDX_GAUSS, &CMainFrame::OnGraddxGauss)
	ON_COMMAND(ID_GRADDY_GAUSS, &CMainFrame::OnGraddyGauss)
	ON_COMMAND(ID_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_RESOLUTION, &CMainFrame::OnResolution)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
	//	ID_INDICATOR_OVR,
	//	ID_INDICATOR_NUM,
	//	ID_INDICATOR_SCRL,
};

CRect clnRect; // client rect
CSize isz;	// client size


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_startDir(_T(""))
{
//	BMP1 = 0;
//	BMP2 = 0;
	m_path = _T("");
//	if (BMP1) delete BMP1;
//	if (BMP2) delete BMP2;
	IsDinDiap = false;
	IsGauss = false;
	workPath = _T("");
	m_exifStr = _T("");
	m_imgId = -1;			 
	
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	m_wndView.m_cp = CPoint(0,0);
	scale = AfxGetApp()->GetProfileInt(_T(""),_T("scale"), 0);
	CMenu * menu = GetMenu();
	if (menu){
		if (scale)
			menu->CheckMenuItem(IDC_VIEWONEONE,MF_CHECKED);
		else 
			menu->CheckMenuItem(IDC_VIEWONEONE,MF_UNCHECKED);
	}

	std::string wpath = ConvertToUTF8(m_startDir);
	initOpenVinoModel(wpath);
	std::string sout = GetOVString();
	if (!sout.empty()) {
		CString ss = ConvertFromUTF8(sout);
		AfxMessageBox(ss, MB_OK);
		
	}


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnOpenimage()
{
	CFileDialog dlg(true);
	dlg.m_ofn.lpstrTitle = _T("Select image");
	dlg.m_ofn.lpstrFilter = _T("JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF\0*.JPG;*.JPEG;*.JPE;*.JFIF\0TIFF (*.TIF;*.TIFF)\0*.TIF;*.TIFF\0BMP (*.BMP;*.DIB;*.RLE)\0*.BMP;*.DIB;*.RLE\0PNG (*.PNG)\0*.PNG\0\0);//CR2 (*.CR2;*.CR3)\0*.CR2;*.CR3\0\0");
//#if 0
	SetWindowText(_T("PhotoProcessor ")+ m_path);
	if (dlg.DoModal() != IDOK){
//		if(gdiplusToken!= NULL){
//			Gdiplus::GdiplusShutdown(gdiplusToken);
//			gdiplusToken = NULL;
//		}
		m_wndView.Invalidate();
		return;
	}
	if (!OpenFromFile(dlg.GetPathName())){
		m_wndView.Invalidate();
	}
	return;
}

BOOL CMainFrame::DestroyWindow()
{
	ReleaseLocalDB();
	if (img1.IsNull() == false){
		img1.Destroy();
	}
	if (img2.IsNull() == false){
		img2.Destroy();
	}

	if(gdiplusToken!= NULL){
		Gdiplus::GdiplusShutdown(gdiplusToken);
		gdiplusToken = NULL;
	}
	CloseOpenVinoModel();
	return CFrameWnd::DestroyWindow();
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER+22){
		CString str;
		//		str.Format(_T("%d"), m_wndView.m_cp.x);
		//		m_wndStatusBar.SetPaneText(6, str);
		//		str.Format(_T("%d"), m_wndView.m_cp.y);
		//		m_wndStatusBar.SetPaneText(7, str);
		m_wndView.GetWindowRect(&clnRect);
		str.Format(_T("%d"), red.sz.x);
		m_wndStatusBar.SetPaneText(1, str);
		str.Format(_T("%d"), red.sz.y);
		m_wndStatusBar.SetPaneText(2, str);
		str.Format(_T("%d"), clnRect.right-clnRect.left);
		m_wndStatusBar.SetPaneText(3, str);
		str.Format(_T("%d"), clnRect.bottom-clnRect.top);
		m_wndStatusBar.SetPaneText(4, str);
		str.Format(_T("%d"), isz.cx);
		m_wndStatusBar.SetPaneText(6, str);
		str.Format(_T("%d"), isz.cy);
		m_wndStatusBar.SetPaneText(7, str);
		str.Format(_T("%d:%d"),-m_wndView.m_cp.x,-m_wndView.m_cp.y);
		m_wndStatusBar.SetPaneText(8, str);
	}
	if (message == WM_USER+11) {
		AfxGetApp()->BeginWaitCursor();
		ScaleDisplay();
		AfxGetApp()->EndWaitCursor();
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::ScaleDisplay(bool isNew){
	uint8 *	mem = 0;
	int psz = red.bpp;
	CRect rect;
	CSize oldsz = m_wndView.sz;
	m_wndView.sz.cx = red.sz.x;
	m_wndView.sz.cy = red.sz.y;
	CSize nSz = m_wndView.sz;
	CSize drob = CSize(31,32);
	CMenu * menu = GetMenu();
	IsDinDiap = false;
	IsGauss = false;

	if (menu){
		menu->CheckMenuItem(ID_DINDIAP,MF_UNCHECKED);
		menu->CheckMenuItem(ID_GAUSS,MF_UNCHECKED);
	}
	m_wndView.GetWindowRect(&rect);
	int color = AfxGetApp()->GetProfileInt(_T(""),_T("color"), 0);
	if (scale){ // display 1:1
#if 0
		if (m_wndView.mem){
			CDC * hdc = m_wndView.GetDC();
			hdc->SetWindowExt(max(rect.right, red.sz.x), max(rect.bottom, red.sz.y));
			m_wndView.ReleaseDC(hdc);
		};
		if(rect.right < red.sz.x) m_wndView.EnableScrollBarCtrl(SB_HORZ,true); 
		else m_wndView.EnableScrollBarCtrl(SB_HORZ,false);
		if(rect.bottom < red.sz.y) m_wndView.EnableScrollBarCtrl(SB_VERT,true);
		else m_wndView.EnableScrollBarCtrl(SB_VERT,false);
#endif
		sdred = red;
		sdblue = blue;
		sdgreen = green;
		sdgray = gray;
		SetDisplayChannal(color);
		m_wndView.Invalidate();
		m_wndView.UpdateWindow();
		return;
	}
	// need scale
#if 0
	m_wndView.EnableScrollBarCtrl(SB_VERT,false);
	m_wndView.EnableScrollBarCtrl(SB_HORZ,false);
	if (m_wndView.mem){
		CDC * hdc = m_wndView.GetDC();
		hdc->SetWindowExt(rect.right, rect.bottom);
		m_wndView.ReleaseDC(hdc);
	}
#endif
	if (red.sz.x*red.sz.y > 0){
		if ((rect.right-rect.left > nSz.cx)&&(rect.bottom-rect.top > nSz.cy)){
			while((rect.right-rect.left > nSz.cx)&&(rect.bottom-rect.top > nSz.cy)){
				nSz.cx = nSz.cx*drob.cy/drob.cx;//;
				nSz.cy = nSz.cy*drob.cy/drob.cx;//;
			}
		}

	}
	if ((rect.right-rect.left < nSz.cx)||(rect.bottom-rect.top < nSz.cy)){
		while((rect.right-rect.left < nSz.cx)||(rect.bottom-rect.top < nSz.cy)){
			nSz.cx = nSz.cx*drob.cx/drob.cy;//  15/16;
			nSz.cy = nSz.cy*drob.cx/drob.cy;//
		}
	}

	m_wndView.sz = nSz;
	if ((oldsz != m_wndView.sz)||isNew){
		if (m_wndView.mem) {
			free(m_wndView.mem); 
			m_wndView.mem = 0;
		}
		if (red.sz.x*red.sz.y <= 0) 
			return;
//		AfxGetApp()->BeginWaitCursor();
#if 0
		switch(color){
			case 1:
				red.Scale(nSz.cx, src_sz.cx,  sdred);
				m_wndView.red1 = sdred;
				m_wndView.green1 = sdred;
				m_wndView.blue1 = sdred;
				break;
			case 2:
				green.Scale(nSz.cx, src_sz.cx, sdgreen);
				m_wndView.red1 = sdgreen;
				m_wndView.green1 = sdgreen;
				m_wndView.blue1 = sdgreen;
			case 3:
				blue.Scale(nSz.cx, src_sz.cx, sdblue);
				m_wndView.red1 = sdblue;
				m_wndView.green1 = sdblue;
				m_wndView.blue1 = sdblue;
				break;
			case 4:
//				break;
			default:
				red.Scale(nSz.cx, src_sz.cx,  sdred);
				green.Scale(nSz.cx, src_sz.cx, sdgreen);
				blue.Scale(nSz.cx, src_sz.cx, sdblue);
				m_wndView.red1 = sdred;
				m_wndView.green1 = sdgreen;
				m_wndView.blue1 = sdblue;
				break;

		}		
#endif
	
		
		red.Scale(nSz.cx, src_sz.cx,  sdred);
		green.Scale(nSz.cx, src_sz.cx, sdgreen);
		blue.Scale(nSz.cx, src_sz.cx, sdblue);
		gray.Scale(nSz.cx, src_sz.cx, sdgray);

#if 0 //debug	
		red.Scale(128,max(red.sz.x,red.sz.y),  sdred);
		green.Scale(128,max(red.sz.x,red.sz.y), sdgreen);
		blue.Scale(128,max(red.sz.x,red.sz.y), sdblue);
		gray.Scale(128,max(red.sz.x,red.sz.y), sdgray);

#endif 

		SetDisplayChannal(color);

//		m_wndView.red1 = sdred;
//		m_wndView.green1 = sdgreen;
//		m_wndView.blue1 = sdblue;
		m_wndView.sz = nSz;
//		this->UpdateBitmap();
//		AfxGetApp()->EndWaitCursor();
	}
	else 
		return;
	m_wndView.Invalidate();

}//end
void CMainFrame::UpdateBitmap(){
	uint8 *	mem = 0;
	int psz = red.bpp;
	m_wndView.start = CPoint(0,0);

	if (m_wndView.mem) {
		free(m_wndView.mem); 
		m_wndView.mem = 0;
	}
	m_wndView.tbi.bmiHeader.biBitCount = psz*8;
	m_wndView.tbi.bmiHeader.biWidth = m_wndView.red1.sz.x;	//	m_wndView.sz.cx;
	m_wndView.tbi.bmiHeader.biHeight = m_wndView.red1.sz.y;  //m_wndView.sz.cy;
	m_wndView.tbi.bmiHeader.biSizeImage = m_wndView.red1.sz.x*m_wndView.red1.sz.y;//
	m_wndView.sz.cx = m_wndView.red1.sz.x;
	m_wndView.sz.cy = m_wndView.red1.sz.y;
	DWORD dwSize = psz*m_wndView.red1.sz.x*m_wndView.red1.sz.y;//m_wndView.sz.cx*m_wndView.sz.cy*psz;
	if (dwSize ){ 
		mem = (uint8 *) malloc(dwSize);
		if (mem){
			memset(mem, 0xff,dwSize); 
			int ind = 0;
			for (int i = 0; i< m_wndView.red1.sz.y; i++){
				for (int j = 0; j< m_wndView.red1.sz.x; j++){
					ind = i*m_wndView.red1.sz.x + j; 
					mem[ind* psz] =   m_wndView.blue1.arr[ind];
					mem[ind*psz+1] =  m_wndView.green1.arr[ind];
					mem[ind*psz+2] =  m_wndView.red1.arr[ind];
				};
			};
		};
	}

	m_wndView.mem = mem;
	if (scale <1)	m_wndView.m_cp = CPoint(0,0);
	else{
		if (m_wndView.m_size.x - m_wndView.m_cp.x > m_wndView.sz.cx) m_wndView.m_cp.x = 0;
		if (m_wndView.m_size.y - m_wndView.m_cp.y > m_wndView.sz.cy) m_wndView.m_cp.y = 0;
	}
	m_wndView.Invalidate();
}
void CMainFrame::OnDindiap()
{
	IsDinDiap = !IsDinDiap;
	CMenu * menu = GetMenu();
	AfxGetApp()->BeginWaitCursor();
	if (menu){
		if (IsDinDiap){
			menu->CheckMenuItem(ID_DINDIAP,MF_CHECKED);
			m_wndView.red1.DinDiap();
			m_wndView.green1.DinDiap();
			m_wndView.blue1.DinDiap();
		}
		else{
			menu->CheckMenuItem(ID_DINDIAP,MF_UNCHECKED);
			OriginalRestore();
		}
	}
	this->UpdateBitmap();
	AfxGetApp()->EndWaitCursor();

}

void CMainFrame::OnGauss()
{
	IsGauss = !IsGauss;
	CMenu * menu = GetMenu();
	AfxGetApp()->BeginWaitCursor();
	if (menu){
		if (IsGauss){
			menu->CheckMenuItem(ID_GAUSS,MF_CHECKED);
			m_wndView.red1.Gauss();
			m_wndView.green1.Gauss();
			m_wndView.blue1.Gauss();
		}
		else{
			menu->CheckMenuItem(ID_GAUSS,MF_UNCHECKED);
			OriginalRestore();
		}
	}
	this->UpdateBitmap();
	AfxGetApp()->EndWaitCursor();
}
void CMainFrame::OriginalRestore(){
	IsDinDiap = false;
	IsGauss = false;
	CMenu * menu = GetMenu();
	if (menu){
		menu->CheckMenuItem(ID_DINDIAP,MF_UNCHECKED);
		menu->CheckMenuItem(ID_GAUSS,MF_UNCHECKED);
	}
	int color = AfxGetApp()->GetProfileInt(_T(""),_T("color"), 0);
	this->SetDisplayChannal(color);
//	m_wndView.red1 = sdred;
//	m_wndView.green1 = sdgreen;
//	m_wndView.blue1 = sdblue;
}

void CMainFrame::Gamma(unsigned char * GAMMA){
//	OriginalRestore();
	m_wndView.red1.Gamma(GAMMA);
	m_wndView.green1.Gamma(GAMMA);
	m_wndView.blue1.Gamma(GAMMA);
	UpdateBitmap();



}

void CMainFrame::OnGamma0()
{
	Gamma((unsigned char *) GAMMA0_4);
}

void CMainFrame::OnGamma1()
{
	Gamma((unsigned char *) GAMMA0_6);
}

void CMainFrame::OnGamma2()
{
	Gamma((unsigned char *) GAMMA0_8);
}

void CMainFrame::OnGamma3()
{
	Gamma((unsigned char *) GAMMA1_0);
}

void CMainFrame::OnGamma4()
{
		Gamma((unsigned char *) GAMMA1_2);
}

void CMainFrame::OnGamma5()
{
	Gamma((unsigned char *) GAMMA1_4);
}

void CMainFrame::OnGamma6()
{
	Gamma((unsigned char *) GAMMA1_6);
}

void CMainFrame::OnGamma7()
{
	Gamma((unsigned char *) GAMMA1_8);
}

void CMainFrame::OnGamma8()
{
	Gamma((unsigned char *) GAMMA2_0);
}

void CMainFrame::OnGammaBig()
{
	Gamma((unsigned char *) GAMMA_BIG);
}

void CMainFrame::OnSetOneToOne()
{
	CMenu * menu = GetMenu();
	scale = AfxGetApp()->GetProfileInt(_T(""),_T("scale"), 0);
	if (menu){
		if (scale){
			menu->CheckMenuItem(IDC_VIEWONEONE,MF_UNCHECKED);
			scale = 0;
			m_wndView.EnableScrollBarCtrl(SB_VERT,false);
			
			
		}
		else{
			menu->CheckMenuItem(IDC_VIEWONEONE,MF_CHECKED);
			scale = 1;
		};
		AfxGetApp()->WriteProfileInt(_T(""),_T("scale"),scale);
		AfxGetApp()->BeginWaitCursor();
		ScaleDisplay(true);
		AfxGetApp()->EndWaitCursor();
	}
}

void CMainFrame::OnViewAllcolar()
{
	SetDisplayChannal(0);
}

void CMainFrame::OnViewGrayscale()
{
	SetDisplayChannal(4);
}

void CMainFrame::OnViewRed()
{
	SetDisplayChannal(1);
}

void CMainFrame::OnViewBlue()
{
	SetDisplayChannal( 3);
}

void CMainFrame::OnViewGreen()
{
	SetDisplayChannal(2);
}

void CMainFrame::SetDisplayChannal(int color){
	AfxGetApp()->WriteProfileInt(_T(""),_T("color"), color);
	CMenu * menu = GetMenu();
	if(menu){
		menu->CheckMenuItem(ID_VIEW_ALLCOLAR,MF_UNCHECKED);
		menu->CheckMenuItem(ID_VIEW_RED,MF_UNCHECKED);
		menu->CheckMenuItem(ID_VIEW_GREEN,MF_UNCHECKED);
		menu->CheckMenuItem(ID_VIEW_BLUE,MF_UNCHECKED);
		menu->CheckMenuItem(ID_VIEW_GRAYSCALE,MF_UNCHECKED);
		switch (color){
			case 1:
				menu->CheckMenuItem(ID_VIEW_RED,MF_CHECKED);
				m_wndView.red1 = sdred;
				m_wndView.green1 = sdred;
				m_wndView.blue1 = sdred;
				break;
			case 2:
				menu->CheckMenuItem(ID_VIEW_GREEN,MF_CHECKED);
				m_wndView.red1 = sdgreen;
				m_wndView.green1 = sdgreen;
				m_wndView.blue1 = sdgreen;
				break;
			case 3:
				menu->CheckMenuItem(ID_VIEW_BLUE,MF_CHECKED);
				m_wndView.red1 = sdblue;
				m_wndView.green1 = sdblue;
				m_wndView.blue1 = sdblue;
				break;
			case 4:
				menu->CheckMenuItem(ID_VIEW_GRAYSCALE,MF_CHECKED);
				m_wndView.red1 = sdgray;
				m_wndView.green1 = sdgray;
				m_wndView.blue1 = sdgray;
				break;
			default:
				menu->CheckMenuItem(ID_VIEW_ALLCOLAR,MF_CHECKED);
				m_wndView.red1 = sdred;
				m_wndView.green1 = sdgreen;
				m_wndView.blue1 = sdblue;
				break;

		}; //end switch
	}
	UpdateBitmap();
}

void CMainFrame::OnFileSavescreen()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	Bitmap  * bbmp = Bitmap::FromBITMAPINFO(&m_wndView.tbi, m_wndView.mem);
	CImage img;
	HBITMAP hb;
	bbmp->GetHBITMAP(0,&hb);
	img.Attach(hb);
	CString iname = m_path.Mid(0,m_path.ReverseFind(_T('.')));
	iname += _T("-c.jpg");
	img.Save(iname/*_T("test.jpg")*/,ImageFormatJPEG );	
	img.Destroy();

}

void CMainFrame::OnToolsInverse()
{
	m_wndView.red1.Inverse();
	m_wndView.green1.Inverse();
	m_wndView.blue1.Inverse();
	UpdateBitmap();
}

void CMainFrame::OnSobel()
{
	m_wndView.red1.Sobel();
	m_wndView.green1.Sobel();
	m_wndView.blue1.Sobel();
	UpdateBitmap();
}

void CMainFrame::OnToolsIntegrator()
{
	m_wndView.red1.Integrator();
	m_wndView.green1.Integrator();
	m_wndView.blue1.Integrator();
	UpdateBitmap();
}

void CMainFrame::OnToolsTestfaces()
{
	CPen* oldPen = NULL;
	CPen dPen(PS_SOLID, 2, RGB(0, 255, 0));
	CPen ePen(PS_SOLID, 1, RGB(0, 0, 255));
	CString stmp = _T("");
	if (m_wndView.mem == 0) {
		//		AfxGetApp()->EndWaitCursor();
		AfxMessageBox(_T("Image not loaded"), MB_ICONSTOP);
		return;
	}
	FRECT rect[100] = {};
	int rsz = 30;
	int count = 0;
	float fk1 = 1.f;
	std::vector<FRECT> faces;
	FRECT face;
	faces.clear();
	CString path = workPath + _T("/test.jpg");// _T("d:/Worker/PhotoProcessor/DLL/test.jpg");
	CString path1 = workPath + _T("/test");//_T("d:/Worker/PhotoProcessor/DLL/test");



#ifndef _M_X64
	AfxMessageBox(_T("This version does not currently support facial recognition in Win32"), MB_ICONSTOP);
	return;



	//	AfxGetApp()->BeginWaitCursor();
	char  data[1024] = {};// = "d:/Worker/PhotoProcessor/DLL/data/haarcascades/haarcascade_frontalface_alt.xml";
	char  fname[MAX_PATH] = {};// "d:/Worker/PhotoProcessor/DLL/test.jpg";
	char data1[1024] = {};
	//char * data1 = "d:/Worker/PhotoProcessor/DLL/data/haarcascades/haarcascade_profileface.xml";
	//               haarcascade_fullbody.xml
	char dat_eye[1024] = {};
	//char * dat_eye = "d:/Worker/PhotoProcessor/DLL/data/haarcascades/haarcascade_eye.xml";
	//        haarcascade_mcs_eyepair_big.xml
	stmp = m_startDir + _T("DLL/data/haarcascades/haarcascade_frontalface_alt.xml");
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, stmp, stmp.GetLength(), data, 1024, NULL, NULL);
	//haarcascade_frontalface_alt.xml
	//haarcascade_frontalface_default.xml
	//haarcascade_profileface.xml

	stmp = m_startDir + _T("DLL/data/haarcascades/haarcascade_profileface.xml");
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, stmp, stmp.GetLength(), data1, 1024, NULL, NULL);

	//char * data1 = "d:/Worker/PhotoProcessor/DLL/data/haarcascades/haarcascade_profileface.xml";
	//               haarcascade_fullbody.xml
	stmp = m_startDir + _T("DLL/data/haarcascades/haarcascade_eye.xml");
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, stmp, stmp.GetLength(), dat_eye, 1024, NULL, NULL);

	//char * dat_eye = "d:/Worker/PhotoProcessor/DLL/data/haarcascades/haarcascade_eye.xml";
	//        haarcascade_mcs_eyepair_big.xml
	//   haarcascade_eye_tree_eyeglasses.xml
	//       haarcascade_eye.xml
	char ch[1024] = {};

	//CString dll = _T("d:/Worker/PhotoProcessor/DLL/cvdface.dll");

	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, path, path.GetLength(), fname, MAX_PATH, NULL, NULL);



	int k1 = TESTVSIZE2, k2 = sdblue.sz.x;
	CPChannel test;
	if (gray.sz.x > gray.sz.y) {
		gray.Scale(TESTVSIZE1, gray.sz.x, test);
		k1 = TESTVSIZE1;
	}
	else {
		gray.Scale(TESTVSIZE2, gray.sz.x, test);
	}
	test.Gauss();
	test.DinDiap();

	SaveChannalToFile(path, test);

	std::vector<FRECT> eye;
	eye.clear();
	stmp = _T("");
	count = DetectObjectsExt(fname, data, rect, rsz, 1.1, 4, 40, 40);
	if (count < 0) count = 0;
	int count1 = DetectObjectsExt(fname, data1, &rect[count], rsz, 1.1, 4, 40, 40);
	if (count1 < 0) count1 = 0;
	rsz = count + rsz;

#endif // !_M_X64
#ifdef _M_X64
	ovimgsize osz = GetInferSize();
	if (std::get<0>(osz) > 0 && std::get<1>(osz)) {
		CPChannel red_sq;
		CPChannel blue_sq;
		CPChannel green_sq;

		red.ScaleVarios(std::get<0>(osz), std::get<1>(osz), red_sq);
		blue.ScaleVarios(std::get<0>(osz), std::get<1>(osz), blue_sq);
		green.ScaleVarios(std::get<0>(osz), std::get<1>(osz), green_sq);

		this->SaveToFile(_T("test1.BMP"), blue_sq, green_sq, red_sq, SaveImageFormat::BMP);
		float xprop = 1.0f * red.sz.x / std::get<0>(osz);
		float yprop = 1.0f * red.sz.y / std::get<1>(osz);
		int add_y = 0;
		float kof = xprop;
		if (red.sz.x < red.sz.y) {
			kof = yprop;
		}
		else {
			add_y = int(kof * (1.0f - yprop / xprop) * std::get<1>(osz));
		}
		fk1 = 1.0f * sdred.sz.x / red.sz.x;
		count = DetectObjectsExtOV(red_sq.arr, blue_sq.arr, green_sq.arr, red_sq.sz.x * red_sq.sz.y, kof, add_y, rect, rsz);
	}
#endif
	if (count > 0) {
		//	if (DetectObjects(fname, data, rect, rsz)> 0){
		if (rsz > 0) {
			CDC* dc = m_wndView.GetDC();
			oldPen = dc->SelectObject(&dPen);
#ifndef _M_X64
			for (int i = 0; i < rsz; i++) {
				//		dc->MoveTo(rect[i].x, rect[i].y);
				face.x = max(0, (rect[i].x - 5)) * blue.sz.x / k1;
				face.y = max(0, (rect[i].y - 5)) * blue.sz.x / k1;
				face.width = (rect[i].width + 10) * blue.sz.x / k1;
				face.height = (rect[i].height + 10) * blue.sz.x / k1;
				faces.push_back(face);
				dc->MoveTo(rect[i].x * k2 / k1, rect[i].y * k2 / k1);
				dc->LineTo(rect[i].x * k2 / k1 /*a*/, (rect[i].y + rect[i].height) * k2 / k1 /*b+h*/);
				dc->LineTo((rect[i].x + rect[i].width) * k2 / k1 /*a+w*/, (rect[i].y + rect[i].height) * k2 / k1 /*b+h*/);
				dc->LineTo((rect[i].x + rect[i].width) * k2 / k1, rect[i].y * k2 / k1);
				dc->LineTo(rect[i].x * k2 / k1, rect[i].y * k2 / k1);
				//				dc->Rectangle(rect[i].x*k2/k1, rect[i].y*k2/k1, rect[i].x*k2/k1 + rect[i].width*k2/k1, rect[i].y*k2/k1 + rect[i].height*k2/k1);
			}
			if (oldPen) dc->SelectObject(oldPen);
			m_wndView.ReleaseDC(dc);
		}
#else

			int mx = m_wndView.m_cp.x;
			int my = (m_wndView.m_cp.y == 0) ? m_wndView.sz.cy : m_wndView.m_size.y - m_wndView.m_cp.y;
			for (int i = 0; i < rsz; i++) {
				//check resualt
				if (rect[i].x + rect->width >= red.sz.x) continue;
				//if (rect[i].y - rect[i].height >= red.sz.y) continue;

				//		dc->MoveTo(rect[i].x, rect[i].y);
				face.x = max(0, int((rect[i].x - 5)));
				face.y = max(0, int(red.sz.y - rect[i].y) - 5);
				face.width = int(rect[i].width) + 10;
				face.height = int(rect[i].height) + 10;
				faces.push_back(face);
				dc->MoveTo(int(fk1 * rect[i].x + m_wndView.m_cp.x), int(my - fk1 * rect[i].y));
				dc->LineTo(m_wndView.m_cp.x + fk1 * rect[i].x, my - fk1 * (rect[i].y - rect[i].height));
				dc->LineTo(int(m_wndView.m_cp.x + fk1 * (rect[i].x + rect[i].width)), int(my - fk1 * (rect[i].y - rect[i].height)));
				dc->LineTo((m_wndView.m_cp.x + int(fk1 * (rect[i].x + rect[i].width))), int(my - fk1 * rect[i].y));
				dc->LineTo(m_wndView.m_cp.x + int(fk1 * rect[i].x), my - int(fk1 * rect[i].y));
				//				dc->Rectangle(rect[i].x*k2/k1, rect[i].y*k2/k1, rect[i].x*k2/k1 + rect[i].width*k2/k1, rect[i].y*k2/k1 + rect[i].height*k2/k1);
			}
			if (oldPen) dc->SelectObject(oldPen);
			m_wndView.ReleaseDC(dc);
		}

#endif
		std::vector<int> scale;
		CPChannel rtest, btest, gtest;
		for (unsigned int i = 0; i < faces.size(); i++) {
			green.SelectRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height, gtest);
			gtest.DinDiap();
			gtest.Scale(TESTEYE);
			red.SelectRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height, rtest);
			rtest.DinDiap();
			rtest.Scale(TESTEYE);
			blue.SelectRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height, btest);
			btest.DinDiap();
			btest.Scale(TESTEYE);

			stmp.Format(_T("%s-%d.jpg"), path1, i + 1);
			//			SaveChannalToFile(stmp, test/*, CMainFrame::BMP*/);
			this->SaveToFile(stmp, btest, gtest, rtest);
			AddFaceToDb(m_imgId, stmp, faces[i]);
#ifndef _M_X64



			/* //home
						test.Sobel();
						stmp.Format(_T("%s-%d.bmp"), path1, i+1);
						SaveChannalToFile(stmp, test, CMainFrame::BMP);

			*/
			//			stmp.Replace(_T("\\"),_T("/"));
			if (WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, stmp, stmp.GetLength(), ch, 1000, NULL, NULL)) {
				rsz = 100;
				if (DetectObjectsExt(ch, dat_eye, rect, rsz, 1.1, 5, 20, 20)) {
					for (int k = 0; k < rsz; k++) {
						face.x = faces[i].x + rect[k].x * faces[i].width / TESTEYE;
						face.y = faces[i].y + rect[k].y * faces[i].width / TESTEYE;
						face.width = rect[k].width * faces[i].width / TESTEYE;
						face.height = rect[k].height * faces[i].width / TESTEYE;
						eye.push_back(face);
					}
				}
				else {
					//				DeleteFile(stmp);
				}
			}//end conversion			   
#endif // !_MX64
			DeleteFile(stmp); // clea working file
		}
#ifndef _M_X64
		k2 = sdblue.sz.x;
		k1 = blue.sz.x;
		CDC* dc = m_wndView.GetDC();
		oldPen = dc->SelectObject(&ePen);
		for (unsigned int i = 0; i < eye.size(); i++) {
			dc->MoveTo(eye[i].x * k2 / k1, eye[i].y * k2 / k1);
			dc->LineTo(eye[i].x * k2 / k1 /*a*/, (eye[i].y + eye[i].height) * k2 / k1 /*b+h*/);
			dc->LineTo((eye[i].x + eye[i].width) * k2 / k1 /*a+w*/, (eye[i].y + eye[i].height) * k2 / k1 /*b+h*/);
			dc->LineTo((eye[i].x + eye[i].width) * k2 / k1, eye[i].y * k2 / k1);
			dc->LineTo(eye[i].x * k2 / k1, eye[i].y * k2 / k1);
			//				dc->Rectangle(rect[i].x*k2/k1, rect[i].y*k2/k1, rect[i].x*k2/k1 + rect[i].width*k2/k1, rect[i].y*k2/k1 + rect[i].height*k2/k1);
		}//end draw eye

		if (oldPen) dc->SelectObject(oldPen);
		m_wndView.ReleaseDC(dc);

#endif // !_M_X64
	}
	DeleteFile(path);
	//	AfxGetApp()->EndWaitCursor();

}//end on toolstestfaces

void CMainFrame::SaveChannalToFile(CString path, CPChannel & test, SaveImageFormat imform  )
{
	CPChannel cst;
	test.SelectRect(0,0,((test.sz.x>>2)<<2),test.sz.y, cst);
	Bitmap  * bbmp = Bitmap::FromBITMAPINFO(cst.bi, cst.arr);
	CImage img;
	HBITMAP hb;
	bbmp->GetHBITMAP(0,&hb);
	img.Attach(hb);
	switch(imform){
		case CMainFrame::BMP:
			img.Save(path,ImageFormatBMP);
			break;
		case CMainFrame::PNG:
			img.Save(path,ImageFormatPNG);
			break;
		case CMainFrame::TIFF:
			img.Save(path,ImageFormatTIFF);
			break;
		case CMainFrame::EMF:
			img.Save(path,ImageFormatEMF);
			break;
		default:
			img.Save(path,ImageFormatJPEG );	
			break;
	}//end switch
	img.Destroy();
}
int CMainFrame::DetectObjects( char * fname, char * data, FRECT * rect, int & rsz){
	memset((unsigned char *) rect, 0, sizeof(FRECT)*rsz);
	HMODULE hLib =  ::LoadLibrary(_T("cvdface.dll"));
	if (!hLib) 	return -1;
	FARPROC cv = GetProcAddress(hLib, "cvfind");
	if (!cv) return -1;
	((void (__stdcall*)( char *, char *, FRECT *, int &))(PROC) cv)(fname, data, rect, rsz);
	FreeLibrary(hLib);
	return rsz;
}

 int CMainFrame::DetectObjectsExt( char * fname, char * data, FRECT * rect, int & rsz, double scale,
						int minNeigboars, int minw, int minh)
 {
	memset((unsigned char *) rect, 0, sizeof(FRECT)*rsz);
	HMODULE hLib =  ::LoadLibrary(_T("cvdface.dll"));
	if (!hLib) 	return -1;
	FARPROC cv = GetProcAddress(hLib, "cvfindext");
	if (!cv) return -1;
	((void (__stdcall*)( char *, char *, FRECT *, int &, double , int, int, int))(PROC) cv)
		(fname, data, rect, rsz, scale, minNeigboars, minw, minh);
	FreeLibrary(hLib);
	return rsz;
 }


void CMainFrame::OnExif()
{
	if (!m_wndView.mem) return;
	CExifDlg dlg;
	DWORD cbWrite = m_exifStr.GetLength();
	EXIFSTR exifinfo;

	if (!m_exifStr.IsEmpty()){
//		dlg.m_msg = m_exifStr;
		ParseExif(m_exifStr.GetBuffer(), cbWrite, dlg.m_msg.GetBuffer(cbWrite), cbWrite, exifinfo);
		dlg.m_msg.ReleaseBuffer();
		m_exifStr.ReleaseBuffer();
		dlg.DoModal();
		return;
	}
#if 0
	AfxGetApp()->BeginWaitCursor();
//	CString m_path = 
	CString ename = m_startDir+_T("DLL/exiftool.exe");// _T("d:\\Worker\\PhotoProcessor\\DLL\\exiftool.exe"); //  -a -u -g1 
	CString einf = m_startDir + _T("DLL/out.txt");//	_T("d:\\Worker\\PhotoProcessor\\DLL\\out.txt");
	//LPWSTR cmdln[MAX_PATH*2] ={};
	STARTUPINFO si;
    PROCESS_INFORMATION pi;


    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
    ZeroMemory( &pi, sizeof(pi) );
	CString cmd;// = ename + m_path;
//	cmd.Format(_T("%sDLL/test.bat \"%s\""), m_startDir,  m_path,  einf);
	cmd.Format(_T("d:\\Worker\\PhotoProcessor\\DLL\\test.bat \"%s\""),  m_path, einf);
	//cmd.Format(_T("exiftool.exe -a -u -g1 \"%s\""),  m_path);
	
//	cmd.Format(_T("%s \"%s\" > \"%s\""), ename, m_path, einf);
//	if (WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS, cmd, cmd.GetLength(), chcmd, 1000,NULL, NULL)){
//	cmd = _T("../DLL/exiftool.exe");
	cbWrite = cmd.GetLength();
	LPWSTR cmdptr = cmd.GetBuffer(cbWrite + 1000);

	//HANDLE hFile = ::CreateFile(einf, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS,0,0);
	//if (hFile != INVALID_HANDLE_VALUE){
	//	si.hStdOutput = hFile;
	//	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	//	if (!WriteFile(hFile, cmdptr, cbWrite, &cbWrite,0)){
	//		cmd.Format(_T("Write failed (%d)."), GetLastError() );
	//	}
	
	if( !CreateProcess( NULL,
        cmdptr,//_TEXT("../DLL/exiftool.exe"), // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        _T("d:\\Worker\\PhotoProcessor\\DLL"),             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 

    {
		cmd.ReleaseBuffer();
		AfxGetApp()->EndWaitCursor();
		cmd.Format(_T("CreateProcess failed (%d)."), GetLastError() );
        AfxMessageBox( cmd, MB_ICONSTOP);
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	cmd.ReleaseBuffer();
	HANDLE hFile = ::CreateFile(einf, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,0,0);
	if (hFile != INVALID_HANDLE_VALUE){
		char * buf= 0;
		LPWSTR wbuf = 0;
		cbWrite = GetFileSize(hFile,0);
		buf = (char *) malloc(cbWrite +100 + cbWrite*sizeof(WCHAR)+100);
		wbuf = (WCHAR *) &buf[((cbWrite+107)/8)*8];
		if (buf){
			memset(buf,0, cbWrite +100 + cbWrite*sizeof(WCHAR)+100);
			ReadFile(hFile, buf, cbWrite, &cbWrite,0);
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, buf, cbWrite, wbuf, cbWrite);
			ParseExif(wbuf, cbWrite, dlg.m_msg.GetBuffer(cbWrite), cbWrite, exifinfo);
			dlg.m_msg.ReleaseBuffer();



//			dlg.m_msg = wbuf;
//			ParseExif(wbuf, dlg.m_msg.GetLength(), 0, 100);
			free(buf);
		}
		AfxGetApp()->EndWaitCursor();
		CloseHandle(hFile);
//		DeleteFile(einf);
		dlg.DoModal();
	}
	else{
		AfxGetApp()->EndWaitCursor();
	}
#endif
}

void CMainFrame::OnMinpict() //save image to file .jpg size 128
{
	if (!m_wndView.mem) return;
	CPChannel minred;
	CPChannel minblue;
	CPChannel mingreen;
	if (red.sz.x <= 0) return;
	int sz = max(red.sz.x,red.sz.y);
	int nw = red.sz.x*128/sz;
	if (((nw>>2)<<2)!=nw ){
		nw =(nw>>2)<<2;
		sz = 128*red.sz.x/nw;
	}

	red.Scale(128, sz, minred);
	blue.Scale(128,sz, minblue);
	green.Scale(128,sz, mingreen);
	CString fpath =  this->workPath + _T("/mem.jpg");
	SaveToFile(fpath, minblue,mingreen, minred);


}

bool CMainFrame::SaveToFile(CString fpath,CPChannel& svblue, CPChannel& svgreen,CPChannel& svred, SaveImageFormat imform ){
	bool ret = false;
	BITMAPINFO tbi;
	memset( &tbi,0, sizeof(BITMAPINFOHEADER));
	
	int width = min(svblue.sz.x, min(svgreen.sz.x, svred.sz.x));
	int height = min(svblue.sz.y, min(svgreen.sz.y, svred.sz.y));
	int size = width*height*3;
	if (size <=0) return ret; // bad image size;
// create header

	tbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	tbi.bmiHeader.biBitCount = 24;
	tbi.bmiHeader.biClrImportant =0;
	tbi.bmiHeader.biClrUsed = 0;
	tbi.bmiHeader.biPlanes = 1;
	tbi.bmiHeader.biCompression =   BI_RGB;
	tbi.bmiHeader.biXPelsPerMeter = 0;
	tbi.bmiHeader.biYPelsPerMeter = 0;
	tbi.bmiHeader.biWidth = width;
	tbi.bmiHeader.biHeight = height;
	tbi.bmiHeader.biSizeImage = size;
	unsigned char * mem = (unsigned char *) malloc(size);
	if (!mem) return ret;
	memset(mem, 0xff,size);
	int ind = 0;
	for (int i = 0; i< height; i++){
		for (int j = 0; j< width; j++){
			ind = i*width + j; 
			mem[ind*3] =   svblue.arr[ind];
			mem[ind*3+1] =  svgreen.arr[ind];
			mem[ind*3+2] =  svred.arr[ind];
		};
	};
	Bitmap  * bbmp = Bitmap::FromBITMAPINFO(&tbi, mem);
	CImage img;
	HBITMAP  hb;
	bbmp->GetHBITMAP(0,&hb);
	img.Attach(hb);
	switch(imform){
		case CMainFrame::BMP:
			img.Save(fpath,ImageFormatBMP);
			break;
		case CMainFrame::PNG:
			img.Save(fpath,ImageFormatPNG);
			break;
		case CMainFrame::TIFF:
			img.Save(fpath,ImageFormatTIFF);
			break;
		case CMainFrame::EMF:
			img.Save(fpath,ImageFormatEMF);
			break;
		default:
			img.Save(fpath,ImageFormatJPEG );	
			break;
	}//end switch
	img.Destroy();
	if (mem) free(mem);
	ret  = true;

	return ret;
}

IStream* CMainFrame::SaveToStream(CPChannel& svblue, CPChannel& svgreen, CPChannel& svred, SaveImageFormat imform)
{
	BITMAPINFO tbi;
	memset(&tbi, 0, sizeof(BITMAPINFOHEADER));

	int width = min(svblue.sz.x, min(svgreen.sz.x, svred.sz.x));
	int height = min(svblue.sz.y, min(svgreen.sz.y, svred.sz.y));
	int size = width * height * 3;
	if (size <= 0) return nullptr;

	IStream* istr = SHCreateMemStream(NULL, NULL);
	// create header
	if (istr != nullptr) {

		tbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		tbi.bmiHeader.biBitCount = 24;
		tbi.bmiHeader.biClrImportant = 0;
		tbi.bmiHeader.biClrUsed = 0;
		tbi.bmiHeader.biPlanes = 1;
		tbi.bmiHeader.biCompression = BI_RGB;
		tbi.bmiHeader.biXPelsPerMeter = 0;
		tbi.bmiHeader.biYPelsPerMeter = 0;
		tbi.bmiHeader.biWidth = width;
		tbi.bmiHeader.biHeight = height;
		tbi.bmiHeader.biSizeImage = size;
		unsigned char* mem = (unsigned char*)malloc(size);
		if (!mem) {
			istr->Release();
			return nullptr;
		}
		memset(mem, 0xff, size);
		int ind = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				ind = i * width + j;
				mem[ind * 3] = svblue.arr[ind];
				mem[ind * 3 + 1] = svgreen.arr[ind];
				mem[ind * 3 + 2] = svred.arr[ind];
			};
		};
		Bitmap* bbmp = Bitmap::FromBITMAPINFO(&tbi, mem);
		CImage img;
		HBITMAP  hb;
		bbmp->GetHBITMAP(0, &hb);
		img.Attach(hb);
		switch (imform) {
		case CMainFrame::BMP:
			img.Save(istr, ImageFormatBMP);
			break;
		case CMainFrame::PNG:
			img.Save(istr, ImageFormatPNG);
			break;
		case CMainFrame::TIFF:
			img.Save(istr, ImageFormatTIFF);
			break;
		case CMainFrame::EMF:
			img.Save(istr, ImageFormatEMF);
			break;
		default:
			img.Save(istr, ImageFormatJPEG);
			break;
		}//end switch
		img.Destroy();
		if (mem) free(mem);
	}
	return istr;
	// istr->Release() free memory
}

// Read exif to string
void CMainFrame::GetExifToStr(CString &str)
{
	if (!m_wndView.mem) return;
	str = _T("");
	//	AfxGetApp()->BeginWaitCursor();

	CString ename = m_startDir + _T("DLL/exiftool.exe");// _T("d:\\Worker\\PhotoProcessor\\DLL\\exiftool.exe"); //  -a -u -g1 
	CString einf = m_startDir + _T("DLL/out.txt"); //	_T("d:\\Worker\\PhotoProcessor\\DLL\\out.txt");
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	ZeroMemory( &pi, sizeof(pi) );
	CString cmd;// = ename + m_path;
	CString tmpWork = m_startDir +_T("/DLL");
	//cmd.Format(_T("d:\\Worker\\PhotoProcessor\\DLL\\test.bat \"%s\""),  m_path, einf);
	cmd.Format(_T("%s/DLL/test.bat \"%s\""), m_startDir,  m_path);//, einf);
	//cmd.Format(_T("exiftool.exe -a -u -g1 \"%s\""),  m_path);
	//	cmd.Format(_T("%s \"%s\" > \"%s\""), ename, m_path, einf);
	//	if (WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS, cmd, cmd.GetLength(), chcmd, 1000,NULL, NULL)){
	//	cmd = _T("../DLL/exiftool.exe");
	DWORD cbWrite = cmd.GetLength();
	LPWSTR cmdptr = cmd.GetBuffer(cbWrite + 1000);

	if( !CreateProcess( NULL,
		cmdptr,//_TEXT("../DLL/exiftool.exe"), // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		tmpWork,//_T("d:\\Worker\\PhotoProcessor\\DLL"),             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 

	{
		cmd.ReleaseBuffer();
		//		AfxGetApp()->EndWaitCursor();
		cmd.Format(_T("CreateProcess failed (%d)."), GetLastError() );
		AfxMessageBox( cmd, MB_ICONSTOP);
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );
	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	cmd.ReleaseBuffer();
	EXIFSTR exifinfo;
	HANDLE hFile = ::CreateFile(einf, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,0,0);
	if (hFile != INVALID_HANDLE_VALUE){
		char * buf= 0;
		LPWSTR wbuf = 0;
		cbWrite = GetFileSize(hFile,0);
		buf = (char *) malloc(cbWrite +100 + cbWrite*sizeof(WCHAR)+100);
		wbuf = (WCHAR *) &buf[((cbWrite+107)/8)*8];
		if (buf){
			memset(buf,0, cbWrite +100 + cbWrite*sizeof(WCHAR)+100);
			ReadFile(hFile, buf, cbWrite, &cbWrite,0);
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, buf, cbWrite, wbuf, cbWrite);
			str = wbuf;
			free(buf);
		}
//		AfxGetApp()->EndWaitCursor();
		CloseHandle(hFile);
//		AfxGetApp()->EndWaitCursor();
	}
	else{
//		AfxGetApp()->EndWaitCursor();
	}


}//end read exif to str

void CMainFrame::OnSaveSqrt()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	CPChannel red3;
	red3 = red;
	CPChannel green3;
	green3 = green;
	CPChannel blue3;
	blue3 = blue;
	CPChannel red2;
	CPChannel green2;
	CPChannel blue2;
	
	// 100:141 707:500

	CString iname = m_path.Mid(0,m_path.ReverseFind(_T('.')));
	iname += _T("-s2.jpg");
	
	AfxGetApp()->BeginWaitCursor();
	red3.Gauss();
	green3.Gauss();
	blue3.Gauss();
	red3.Scale(500,707, red2);
	green3.Scale(500,707,green2);
	blue3.Scale(500,707,blue2);
	SaveToFile(iname,blue2,green2,red2);
	AfxGetApp()->EndWaitCursor();

}

void CMainFrame::OnOpenOldImage()
{
	CDbOldDlg dlg(this);
	CString msg;
	dlg.workPath = workPath;
	if (dlg.DoModal()== IDOK){
		if (!this->OpenFromFile(dlg.m_path)){
			msg.Format(_T("File \" %s \" not found"),dlg.m_path);
			this->m_wndView.Invalidate();
			AfxMessageBox(msg, MB_ICONSTOP);
		}
	}
}

// open image from file
bool CMainFrame::OpenFromFile(CString sName, bool bSilent)
{
	unsigned char hashbuf[16]={};
	m_path = sName;
	CString str;
	CString sHash;
	InitLocalDB(workPath);
	m_imgId = 0;
	m_wndView.m_cp = CPoint(0,0);
	m_wndStatusBar.SetPaneText(1,_T(""));
	m_wndStatusBar.SetPaneText(2,_T(""));
	m_wndStatusBar.SetPaneText(3,_T(""));
	m_wndStatusBar.SetPaneText(4,_T(""));
	str.Format(_T("%d"), m_wndView.m_cp.x);
	m_wndStatusBar.SetPaneText(6, str);
	m_wndStatusBar.SetPaneText(7, str);
	//m_wndStatusBar.SetPaneText(8,_T("111"));
	red.Init(0,0);
	blue.Init(0,0);
	green.Init(0,0);
	sdred.Init(0,0);
	sdgreen.Init(0,0);
	sdblue.Init(0,0);
	gray.Init(0,0);
	sdgray.Init(0,0);
	m_exifStr = _T("");
	if (img1.IsNull() == false){
		img1.Destroy();
	}
	if (img2.IsNull() == false){
		img2.Destroy();
	}
	m_wndView.red1.Init(0,0);
	m_wndView.blue1.Init(0,0);
	m_wndView.green1.Init(0,0);
	m_wndView.m_cp = CPoint(0,0);
	m_wndView.sz = CSize(0,0);
	uint8 * hmem = m_wndView.mem;
	if (hmem){
		m_wndView.mem = 0;
		free(hmem);
	}
	if (!bSilent) AfxGetApp()->BeginWaitCursor();
	int sz11 = GetFileHash(sName, hashbuf);
	if (sz11 < 100){
		return false;
	}
	m_imgId = -1;
	ConvertHashToString(sHash, hashbuf);

//	AfxGetApp()->BeginWaitCursor();
	Bitmap * bbb=0;
	m_path.ReleaseBuffer();

	bbb = Bitmap::FromFile(m_path.GetBuffer());
	if (!bbb)
		return false;
	UINT psize = bbb->GetPropertyItemSize(PropertyTagOrientation);
	PropertyItem* pp = nullptr;
	if (psize > 0) {
		pp = (PropertyItem*)malloc(psize + 16);
		if (pp != nullptr) {
			bbb->GetPropertyItem(PropertyTagOrientation, psize, pp);
			uint8_t* rotp = (uint8_t*)pp->value;
			switch (rotp[0])
			{
			case 2:
				bbb->RotateFlip((Gdiplus::RotateNoneFlipX));
				break;
			case 3:
				bbb->RotateFlip((Gdiplus::Rotate180FlipNone));
				break;
			case 4:
				bbb->RotateFlip((Gdiplus::Rotate180FlipX));
				break;
			case 5:
				bbb->RotateFlip((Gdiplus::Rotate90FlipX));
				break;
			case 6:
				bbb->RotateFlip((Gdiplus::Rotate90FlipNone));
				break;
			case 7:
				bbb->RotateFlip((Gdiplus::Rotate270FlipX));
				break;
			case 8:
				bbb->RotateFlip((Gdiplus::Rotate270FlipNone));
				break;
			default:
				break;
			}
			free(pp);
		}
	}
//	unsigned short  pcount[18] = {};
#if 0
	if (psize ||(psize > 18)){
		// Get the property item.
		bbb->GetPropertyItem(PropertyTagOrientation, psize, (PropertyItem *) &pcount);
				
		PropertyItem* pp = (PropertyItem*) pcount;
	
		switch(pcount[8])
		{
			case 2:
				bbb->RotateFlip((Gdiplus::RotateNoneFlipX));
				break;
			case 3:
				bbb->RotateFlip((Gdiplus::Rotate180FlipNone));
				break;
			case 4:
				bbb->RotateFlip((Gdiplus::Rotate180FlipX));
				break;
			case 5:
				bbb->RotateFlip((Gdiplus::Rotate90FlipX));
				break;
			case 6:
				bbb->RotateFlip((Gdiplus::Rotate90FlipNone));
				break;
			case 7:
				bbb->RotateFlip((Gdiplus::Rotate270FlipX));
				break;
			case 8:
				bbb->RotateFlip((Gdiplus::Rotate270FlipNone));
				break;
			default:
				break;
		}
	}
#endif
	if (bbb){
		HBITMAP hbm;
		bbb->GetHBITMAP(/*Gdiplus::Color(0,0,0)*/ NULL, &hbm);
		CBitmap  * cBmp1 = CBitmap::FromHandle(hbm);
		BITMAP bimg1;
		if (!cBmp1->GetBitmap(&bimg1)){
			if (!bSilent){
				AfxGetApp()->EndWaitCursor();
				AfxMessageBox(_T("Error bitmap"), MB_ICONSTOP);
			}
			delete[] bbb;
			return false;
		}

		gray.InitFromCBitmap(4, bimg1);
		int bpp = 3;

		blue.InitFromCBitmap(0, bimg1);
		if (blue.sz.x*blue.sz.y > 0) bpp--;
		green.InitFromCBitmap(1, bimg1);
		if (green.sz.x*green.sz.y > 0) bpp--;
		red.InitFromCBitmap(2, bimg1);
		if (red.sz.x*red.sz.y > 0) bpp--;
		if (bpp){
			if (!bSilent){
				AfxGetApp()->EndWaitCursor();
				AfxMessageBox(_T("Error create image"), MB_ICONSTOP);
			}
			blue.Init(0,0);
			red.Init(0,0);
			green.Init(0,0);
			if (cBmp1) 
				cBmp1->DeleteObject();
			if (bbb) 
				delete bbb;
			return false;
		}
		src_sz.cx = red.sz.x;//img1.GetWidth();
		src_sz.cy = red.sz.y;//img1.GetHeight();
//		if (bbb) 
		if (cBmp1) 
			cBmp1->DeleteObject();

		if (bbb) 
			delete bbb;
	}
	if (img1.IsNull() == false){
		img1.Destroy();
	}
	if (img2.IsNull() == false){
		img2.Destroy();
	}
	
	SetWindowText(_T("PhotoProcessor ")+ m_path);
//	AfxGetApp()->EndWaitCursor();

	str.Format(_T("%d"), m_wndView.m_cp.x);
	m_wndStatusBar.SetPaneText(6, str);
	str.Format(_T("%d"), m_wndView.m_cp.y);
	m_wndStatusBar.SetPaneText(7, str);
	m_wndView.GetWindowRect(&clnRect);
	str.Format(_T("%d"), red.sz.x);
	m_wndStatusBar.SetPaneText(1, str);
	str.Format(_T("%d"), red.sz.y);
	m_wndStatusBar.SetPaneText(2, str);
	str.Format(_T("%d"), clnRect.right-clnRect.left);
	m_wndStatusBar.SetPaneText(3, str);
	str.Format(_T("%d"), clnRect.bottom-clnRect.top);
	m_wndStatusBar.SetPaneText(4, str);

	//	str.Format(_T("%d "), red.sz.y);
	//	m_wndStatusBar.SetPaneText(3, str);
	//	m_wndStatusBar.SetPaneText(4, _T(" 22222 x 333333 "));
	ScaleDisplay(true);
	GetExifToStr(m_exifStr);
	if (m_exifStr.IsEmpty()) {
		if (!bSilent) AfxGetApp()->EndWaitCursor();
		AfxMessageBox(_T("Exif information is missing\r\nCheck that file \"exiftool.exe\" exist"), MB_ICONSTOP | MB_OK);
		this->m_wndView.Invalidate();
		return true;

	}
	
	OnMinpict();
	CString fpath = this->workPath + _T("/mem.jpg");
	m_imgId = AddFileToDb(this->m_path, fpath, m_exifStr);
	DeleteFile(fpath);
	this->m_wndView.Invalidate();
	if (!bSilent) AfxGetApp()->EndWaitCursor();
	
	return true;
}

void CMainFrame::OnShowFaces()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	CFacesDlg dlg(m_imgId,this);
	dlg.workPath = this->workPath;
	dlg.DoModal();
}
int CMainFrame::ScanDir(CString path){
	if (path.IsEmpty()) 	return 0;
	path.Replace(_T('\\'),_T('/'));
	int count =0;
	CString file;
	CFileFind finder;	
	std::vector<CString> info;
	// find jpg
	std::vector<CString> exts;
	exts.push_back(_T("/*.jpg"));
	exts.push_back(_T("/*.bmp"));
	exts.push_back(_T("/*.png"));
	exts.push_back(_T("/*.tif"));

	CString strWildcard;// = path + _T("/*.jpg");
	BOOL bWorking;
	for (int i = 0; i<exts.size(); i++){
		strWildcard = path + exts[i];
		bWorking = finder.FindFile(strWildcard);
		while (bWorking)  {
			bWorking = finder.FindNextFile();
			if (finder.IsDots()) {
				continue;
			} // end file processing
			if (finder.IsDirectory())    {
				continue;
			}
			else {
				file = finder.GetFilePath();
				file.Replace(_T('\\'),_T('/'));
				//			file.iFileSize = finder.GetLength();
				info.push_back(file);
			}
		}//end while
		finder.Close();
	}//end loop for extentions



	if (info.size() > 0){
		CProgressDlg dlg;
		dlg.Create(this);
		dlg.InitData(path, (int) info.size());
		for(unsigned int i = 0; i<info.size(); i++){
			if (dlg.CheckCancelButton()) 
				break;
			dlg.NextStep(info[i]);
			if ( OpenFromFile(info[i],true)){
				count++;
				OnToolsTestfaces();
			}
		}
		dlg.DestroyWindow();
	}
	return count;
};

void CMainFrame::OnDirScan()
{
	CString oldPath = this->workPath;
	CString path = GetDirectory(this->m_hWnd, _T("Select folder to scan and save"));
	if (path.IsEmpty())
		return;
	path.Replace(_T('\\'),_T('/'));
	ReleaseLocalDB();
    workPath=path;

	ScanDir(path);
#if 0	
	CString file;
	CFileFind finder;	
	std::vector<CString> info;
	CString strWildcard = path + _T("/*.jpg");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)  {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		} // end file processing
		if (finder.IsDirectory())    {
			continue;
		}
		else {
			file = finder.GetFilePath();
			file.Replace(_T('\\'),_T('/'));
//			file.iFileSize = finder.GetLength();
			info.push_back(file);
		}
   }//end while
   finder.Close();

   if (info.size() > 0){
	   CProgressDlg dlg;
	   dlg.Create(this);
	   dlg.InitData(path, info.size());
	   workPath=path;
	   for(int i = 0; i<info.size(); i++){
		   if (dlg.CheckCancelButton()) 
			   break;
		   dlg.NextStep(info[i]);
		   OpenFromFile(info[i]);
		   OnToolsTestfaces();
	   }
	   dlg.DestroyWindow();
   }
#endif
}

void CMainFrame::OnViewShowallfaces()
{
#if 0
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
#endif
	InitLocalDB(workPath);

	CFacesDlg dlg(0,this);
	dlg.workPath = this->workPath;
	if (dlg.DoModal()==IDOK){
		CString path =  GetAlternatePath(dlg.m_idMain);
		this->OpenFromFile(path);
	}

}

void CMainFrame::OnOpenFolder()
{
	CString path = GetDirectory(this->m_hWnd, _T("Select folder"));
	if (path.IsEmpty())
		return;
	path.Replace(_T('\\'),_T('/'));
	ReleaseLocalDB();
	CString dbname = path + _T("/vb.db3");
	if (CheckFileExist(dbname)){
		workPath = path;
		OnOpenOldImage();
	}
	else{
		if (AfxMessageBox(_T("Folder not scaned yet\r\nDo You want scan this folder?"), MB_ICONQUESTION|MB_YESNO) ==IDYES){
			workPath = path;
			ScanDir(path);
		}
		OnOpenOldImage();

	}


}

void CMainFrame::OnScanToOld()
{
	CString path = GetDirectory(this->m_hWnd, _T("Select folder to scan"));
	if (path.IsEmpty())
		return;
	path.Replace(_T('\\'),_T('/'));
//	ReleaseLocalDB();
	ScanDir(path);
}

void CMainFrame::OnGistdlg()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}

	CGistDlg dlg;
	unsigned int clr = 0;
	int size =  m_wndView.red1.sz.y* m_wndView.red1.sz.x;
//	m_wndView.green1.FillHistogram();
//	memcpy(dlg.gist.distr, m_wndView.green1.gist.distr, GESTSIZE*sizeof(__int32));
//	dlg.gist.count = m_wndView.green1.gist.count;
//	dlg.gist.CalcResults();
	for (int i=0; i<size; i++){
		clr = m_wndView.red1.arr[i] + m_wndView.blue1.arr[i] + m_wndView.green1.arr[i];
		dlg.gist.addvalue((unsigned char) ((clr+1)/3));
	}
	dlg.DoModal();
}

void CMainFrame::OnConturmax()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	OriginalRestore();
//	OnViewGrayscale();
//	return;
//	GISTOGRAMM gist;
	CPChannel proc;
	int count = m_wndView.green1.sz.x * m_wndView.green1.sz.y;
	proc = m_wndView.green1;
	proc.DinDiap();
	proc.Gauss();
	proc.Sobel();
	proc.FillHistogram();
	proc.gist.CalcResults();
	int chg = proc.gist.maxpos;
	unsigned char chmin = 0;
	unsigned char chmax = 255;
	int add = (proc.gist.maxdif/*.mindif*/*GESTSIZE)/proc.gist.count;
	if ((add +chg )< chmax) chmax = (unsigned char) (add+chg);
	if ( chg -add  > chmin) chmin =  (unsigned char) (chg-add);
	for (int i =0; i< count; i++){
		//if (proc.arr[i] == chg){
		if ((proc.arr[i] >= chmin)&&(proc.arr[i]<= chmax)){
			proc.arr[i]=0;
		}
		else {
			proc.arr[i] = 0xff;
		}
	}
	m_wndView.green1 = proc;
	m_wndView.red1 = proc;
	m_wndView.blue1 = proc;


	UpdateBitmap();
	
}

void CMainFrame::OnConturequ()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	OriginalRestore();
//	OnViewGrayscale();
//	return;
//	GISTOGRAMM gist;
	CPChannel proc;
	int count = m_wndView.green1.sz.x * m_wndView.green1.sz.y;
	proc = m_wndView.green1;
	proc.DinDiap();
	proc.Gauss();
	proc.Sobel();
	proc.FillHistogram();
	proc.gist.CalcResults();
	unsigned char chg = (unsigned char) proc.gist.maxpos;
	for (int i =0; i< count; i++){
		if (proc.arr[i] == chg){
			proc.arr[i]=0;
		}
		else {
			proc.arr[i] = 0xff;
		}
	}
	m_wndView.green1 = proc;
	m_wndView.red1 = proc;
	m_wndView.blue1 = proc;


	UpdateBitmap();
}

void CMainFrame::OnConturmin()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("A image not loaded"),MB_ICONSTOP);
		return;
	}
	OriginalRestore();
//	OnViewGrayscale();
//	return;
//	GISTOGRAMM gist;
	CPChannel proc;
	int count = m_wndView.green1.sz.x * m_wndView.green1.sz.y;
	proc = m_wndView.green1;
	proc.DinDiap();
	proc.Gauss();
	proc.Sobel();
	proc.FillHistogram();
	proc.gist.CalcResults();
	int chg = proc.gist.maxpos;
	unsigned char chmin = 0;
	unsigned char chmax = 255;
	int add = (proc.gist.mindif*GESTSIZE)/proc.gist.count;
	if ((add +chg )< chmax) chmax = (unsigned char) (add+chg);
	if ( chg -add  > chmin) chmin =  (unsigned char) (chg-add);
	for (int i =0; i< count; i++){
		//if (proc.arr[i] == chg){
		if ((proc.arr[i] >= chmin)&&(proc.arr[i]<= chmax)){
			proc.arr[i]=0;
		}
		else {
			proc.arr[i] = 0xff;
		}
	}
	m_wndView.green1 = proc;
	m_wndView.red1 = proc;
	m_wndView.blue1 = proc;


	UpdateBitmap();
}

void CMainFrame::OnConturleval()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	OriginalRestore();
//	OnViewGrayscale();
//	return;
//	GISTOGRAMM gist;
	CPChannel proc;
	int count = m_wndView.green1.sz.x * m_wndView.green1.sz.y;
	proc = m_wndView.green1;
	proc.DinDiap();
	proc.Gauss();
	proc.Sobel();
	proc.FillHistogram();
	proc.gist.CalcResults();
	int chg = proc.gist.maxpos;
	unsigned char chmin = 0;
	unsigned char chmax = 255;
//	int add = (proc.gist.mto/*.mindif*/*GESTSIZE)/proc.gist.count;
//	if ((add +chg )< chmax) chmax = (unsigned char) (add+chg);
//	if ( chg -add  > chmin) chmin =  (unsigned char) (chg-add);

	int add = (proc.gist.maxdif/*.mindif*/*GESTSIZE)/proc.gist.count;
	if ((add +chg )< chmax) chmax = (unsigned char) (add+chg);
	if ( chg -add  > chmin) chmin =  (unsigned char) (chg-add);
	for (int i =0; i< count; i++){
		//if (proc.arr[i] == chg){
#if 0
		if ((proc.arr[i] >= chmin)&&(proc.arr[i]<= chmax)){
			proc.arr[i]=0;
		}
#endif
		if (proc.arr[i] > chmin){
			if (proc.arr[i] > chmax)
				proc.arr[i] = 0xff;
			else{
				if (proc.arr[i]==chg) 
					proc.arr[i]= 0x00;
				else
					proc.arr[i] =0x7f;
			}
		}
		else {
			proc.arr[i] = 0xff;
		}
	}
	m_wndView.green1 = proc;
	m_wndView.red1 = proc;
	m_wndView.blue1 = proc;


	UpdateBitmap();
}

void CMainFrame::OnOperRestore()
{
	this->OriginalRestore();
}

void CMainFrame::OnToolsRestoreoriginalpath()
{
	ReleaseLocalDB();
	workPath = this->m_saveStartDir;
	OnOpenOldImage();
}

void CMainFrame::OnSelgoodpoint()
{
	if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}
	CString str= _T("");
	m_wndStatusBar.SetPaneText(5,_T(""));
	int cnt  = 0;
	CPChannel img;
	INT_FeatureList fl;
	AfxGetApp()->BeginWaitCursor();
	if (fl.CreateList( MAXPOINTNUM+2)){
		fl.Clear();
		img = m_wndView.red1;//.Scale(max(red.sz.x,red.sz.y)/2, max(red.sz.x,red.sz.y), img);
		img.DinDiap();
		//img = m_wndView.red1;
		int x,y;
		int nrows = m_wndView.red1.sz.y;
		double fk = 1.0*m_wndView.red1.sz.x/img.sz.x;
		cnt = img.SelectGoodPoints(fl);
		if (cnt > 0){
			cnt =0;
			for (int i =0; i< fl.nFeatures; i++){
				if (fl.feature[i].val > 0){
					fl.feature[i].x = (int) floor(fk*fl.feature[i].x +0.5);
					fl.feature[i].y = (int) floor(fk*fl.feature[i].y +0.5);
					cnt++;
				}
			}
			CRect rect;
			m_wndView.GetClientRect(&rect);
			CDC * vDC = m_wndView.GetDC();
			CPen * oldPen = vDC->SelectObject(&rgbPen);
			for (int i =0; i< fl.nFeatures; i++){
				if (fl.feature[i].val > 0){
					if (rect.bottom > img.sz.y ){
						y = nrows - fl.feature[i].y;
					}
					else {
						y = rect.bottom - fl.feature[i].y  - m_wndView.m_cp.y;
					}
					vDC->Ellipse(fl.feature[i].x - 2 + m_wndView.m_cp.x, 
						y - 2, 
						fl.feature[i].x + 2 + m_wndView.m_cp.x, 
						y + 2  );
				}
			}
			if (oldPen) vDC->SelectObject(oldPen);
			m_wndView.ReleaseDC(vDC);
		}
		fl.FreeList();
	}

	AfxGetApp()->EndWaitCursor();
#if 0
	INTIMGSTRUCT gimps;
	//m_wndView.red1.Scale(600, max(red.sz.x,red.sz.y), img);
	img = m_wndView.red1;
	int x,y;
	int cnt  = 0;
	int nrows = m_wndView.red1.sz.y;
	double fk = 1.0*m_wndView.red1.sz.x/img.sz.x;


	if (gimps.Initilise(img.sz.x, img.sz.y, MAXPOINTNUM+2))
	{
		_iMaxX = img.sz.x - _iMinX;
		_iMaxY = img.sz.y - _iMinY;
		double fk = 1.0*m_wndView.red1.sz.x/img.sz.x;
		IntWindowSelGoodPoints(img.arr,    			// source bw 8-bits image
			img.sz.x, img.sz.y, 				// sizeof image
			gimps.gradx.data, gimps.grady.data,	// temp matrixes for gradient must be ncols*nrows size ????
			gimps.featuremap,			// temp bufer for sorting point ncols*nrows
			gimps.pointlist,					// temp buffer for value must be 3*nrows*ncols
			gimps.tc,			// tracert contect
			gimps.fl,				// feature list
			(_iMaxX+_iMinX)/2, (_iMaxY+_iMinY)/2,//gimps.ncols/2, gimps.nrows/2,						// center of window
			(_iMaxX-_iMinX)/2, (_iMaxY-_iMinY)/2);//gimps.ncols/3, gimps.nrows/3 );				// 1/2 window size
		for (int i =0; i< gimps.fl.nFeatures; i++){
			if (gimps.fl.feature[i].val > 0){
				gimps.fl.feature[i].x = (int) floor(fk*gimps.fl.feature[i].x +0.5);
				gimps.fl.feature[i].y = (int) floor(fk*gimps.fl.feature[i].y +0.5);
//				cnt++;
			}
		}
//		gimps.gRect.Clear();
//		getBoundsRects(gimps.gRect,gimps.fl);

		CDC * vDC = m_wndView.GetDC();
		CPen * oldPen = vDC->SelectObject(&rgbPen);
		for (int i =0; i< gimps.fl.nFeatures; i++){
			if (gimps.fl.feature[i].val > 0){
				x = gimps.fl.feature[i].x;
				y = gimps.fl.feature[i].y;
				vDC->Ellipse(x - 2, nrows - y - 2, x + 2, nrows - y + 2);
				cnt++;
			}
		}
		if (oldPen) vDC->SelectObject(oldPen);
		m_wndView.ReleaseDC(vDC);
		gimps.Release();
	}//end work
#endif
	if (cnt < 0){
		AfxMessageBox(_T("Out of memory"),MB_ICONSTOP );
	}
	str.Format(_T("_%d_"), cnt);
	m_wndStatusBar.SetPaneText(4,str);
	
}//end sel good point

void CMainFrame::OnFiltergradx()
{
	GradientFilter(( __int32 *) GRAD_FILTERX1);	
}

void CMainFrame::OnFiltergrady()
{
	GradientFilter( (__int32 *) GRAD_FILTERY1);
}

void CMainFrame::OnGraddxGauss()
{
	GradientFilter( (__int32 *) GRAD_FILTERX1,true);
}

void CMainFrame::OnGraddyGauss()
{
	GradientFilter( (__int32 *) GRAD_FILTERY1,true);
}

void CMainFrame::GradientFilter(__int32 * filter, bool IsGauss)
{
		if (m_wndView.mem == 0){
		AfxMessageBox(_T("Image not loaded"),MB_ICONSTOP);
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	OriginalRestore();
	bool ret = false;
	if (m_wndView.red1.GradientCreate(filter,IsGauss) ){
	}
	m_wndView.green1.GradientCreate(filter,IsGauss);
	m_wndView.blue1.GradientCreate(filter,IsGauss);
	UpdateBitmap();
	AfxGetApp()->EndWaitCursor();
}


void CMainFrame::OnOptions()
{
	COptionsDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CString msg = dlg.m_faceXML;
		msg += "\r\n\r\n";
		msg += ConvertFromUTF8(sModelParameters(dlg.minfo1));
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
}


void CMainFrame::OnResolution()
{
	if (m_wndView.mem == 0) {
		AfxMessageBox(_T("Image not loaded"), MB_ICONSTOP);
		return;
	}
	int msize = max(red.sz.x, red.sz.y);
	if ( msize >= 1920) {
		AfxMessageBox(_T("No need to increase the resolution for this image"), MB_ICONSTOP);
		return;
  	}
	bool bRotate = red.sz.x > red.sz.y ? false : true;
	CPChannel red_sq, red_sq1;
	CPChannel blue_sq, blue_sq1;
	CPChannel green_sq, green_sq1;
	float FM_PI = float(1.5707963267948966192313216916398);
	CString path = workPath + _T("/test1.bmp");// _T("d:/Worker/PhotoProcessor/DLL/test.jpg");
	CString path1 = workPath + _T("/test");//_T("d:/Worker/PhotoProcessor/DLL/test");
	if (bRotate) {
		CPChannel tmp;
		red.Rotate(FM_PI, tmp);
		tmp.ScaleVarios(1920, 1080, red_sq1);
		tmp.ScaleVarios(640, 360, red_sq);
		blue.Rotate(FM_PI, tmp);
		tmp.ScaleVarios(1920, 1080, blue_sq1);
		tmp.ScaleVarios(640, 360, blue_sq);
		green.Rotate(FM_PI, tmp);
		tmp.ScaleVarios(1920, 1080, green_sq1);
		tmp.ScaleVarios(640, 360, green_sq);
		SaveToFile(path, blue_sq, green_sq, red_sq, SaveImageFormat::BMP);
		

	}
	else {
		red.ScaleVarios(1920, 1080, red_sq1);
		red.ScaleVarios(640, 360, red_sq);
		blue.ScaleVarios(1920, 1080, blue_sq1);
		blue.ScaleVarios(640, 360, blue_sq);
		green.ScaleVarios(1920, 1080, green_sq1);
		green.ScaleVarios(640, 360, green_sq);
		SaveToFile(path, blue_sq, green_sq, red_sq, SaveImageFormat::BMP);

	}

	



	

}
