// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
	, m_dir(_T(""))
	, m_file(_T(""))
{
	m_szAll = 0;
	m_szCur = 0;
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_pBar);
	DDX_Text(pDX, IDC_EDIT1, m_dir);
	DDX_Text(pDX, IDC_EDIT2, m_file);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CProgressDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CProgressDlg message handlers

void CProgressDlg::OnBnClickedCancel()
{
	m_IsCancel = true;
	//OnCancel();
}

BOOL CProgressDlg::Create(CWnd* pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CProgressDlg::InitData(CString dirname , int iSizeAll)
{
    PumpMessages();
	m_dir.Format(_T("Folder : %s"), dirname);
	m_szAll = iSizeAll;
	m_pBar.SetRange(0, m_szAll);
	m_szCur = 0;
	UpdateData(false);
	m_IsCancel = false;
	m_pBar.SetPos(0);
    PumpMessages();
	UpdateWindow();

}

BOOL CProgressDlg::DestroyWindow()
{
	ReEnableParent();
	return CDialog::DestroyWindow();
}
void CProgressDlg::NextStep(CString fname){
	if (m_IsCancel) return;
    PumpMessages();
	UpdateData();
	m_pBar.StepIt();
	m_pBar.SetPos(++m_szCur);
	m_file.Format(_T("scan (%d of %d): %s"), m_szCur, m_szAll, fname);
	UpdateData(false);
	ShowWindow(SW_SHOW);
	this->CenterWindow();
	this->UpdateWindow();

}
void CProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}
BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CProgressDlg::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}
BOOL CProgressDlg::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_IsCancel;
    m_IsCancel = FALSE;

    return bResult;
}