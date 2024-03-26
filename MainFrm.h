// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "cvdface.h"
#include "ExifDlg.h"
#include <shlwapi.h>
//#include "DbOldDlg.h"
class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	CString m_path;
	bool IsDinDiap;
	bool IsGauss;
	int scale;
	// source display image channals
	CPChannel sdred;
	CPChannel sdgreen;
	CPChannel sdblue;
	CPChannel sdgray;
	CString workPath; // application worker path -> LocalApp 
	CString m_exifStr;
	CString m_saveStartDir;

// Operations
public:
	enum SaveImageFormat{ BMP, PNG,JPEG, TIFF, EMF}; 
	void SaveChannalToFile(CString path, CPChannel & test, SaveImageFormat imform = JPEG  );
	int DetectObjects( char * fname, char * data, FRECT * rect, int & rsz);
	int DetectObjectsExt( char * fname, char * data, FRECT * rect, int & rsz, double scale = 1.1,
						int minNeigboars = 3, int minw =30, int minh = 30);
	bool SaveToFile(CString fpath,CPChannel & svblue, CPChannel & green,CPChannel & red, SaveImageFormat imform = JPEG);
	IStream * 	SaveToStream(CPChannel& svblue, CPChannel& green, CPChannel& red, SaveImageFormat imform = JPEG);
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenimage();
	void ScaleDisplay(bool isNew = false);
	void OriginalRestore();
	void UpdateBitmap();
	afx_msg void OnDindiap();
	afx_msg void OnGauss();
	afx_msg void OnGamma0();
	afx_msg void OnGamma1();
	afx_msg void OnGamma2();
	void Gamma(unsigned char * GAMMA);
	void SetDisplayChannal(int color);
	virtual BOOL DestroyWindow();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnGamma3();
	afx_msg void OnGamma4();
	afx_msg void OnGamma5();
	afx_msg void OnGamma6();
	afx_msg void OnGamma7();
	afx_msg void OnGamma8();
	afx_msg void OnGammaBig();
	afx_msg void OnSetOneToOne();
	afx_msg void OnViewAllcolar();
	afx_msg void OnViewGrayscale();
	afx_msg void OnViewRed();
	afx_msg void OnViewBlue();
	afx_msg void OnViewGreen();
	afx_msg void OnFileSavescreen();
	afx_msg void OnToolsInverse();
	afx_msg void OnSobel();
	afx_msg void OnToolsIntegrator();
	afx_msg void OnToolsTestfaces();
	afx_msg void OnExif();
	afx_msg void OnMinpict();
	// Read exif to string
	void GetExifToStr(CString & str);
	afx_msg void OnSaveSqrt();
	afx_msg void OnOpenOldImage();
	// open image from file
	bool OpenFromFile(CString sName, bool bSilent = false );
	int m_imgId;
	CString m_startDir;
	afx_msg void OnShowFaces();
	afx_msg void OnDirScan();
	afx_msg void OnViewShowallfaces();
	afx_msg void OnOpenFolder();
	int ScanDir(CString path); // scanninfg folder to files
	afx_msg void OnScanToOld();
	afx_msg void OnGistdlg();
	afx_msg void OnConturmax();
	afx_msg void OnConturequ();
	afx_msg void OnConturmin();
	afx_msg void OnConturleval();
	afx_msg void OnOperRestore();
	afx_msg void OnToolsRestoreoriginalpath();
	afx_msg void OnSelgoodpoint();
	afx_msg void OnFiltergradx();
	afx_msg void OnFiltergrady();
	afx_msg void OnGraddxGauss();
	afx_msg void OnGraddyGauss();
	void GradientFilter(__int32 * filter, bool IsGauss = false);
	afx_msg void OnOptions();
	afx_msg void OnResolution();
};


