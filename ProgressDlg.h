#pragma once
#include "afxcmn.h"


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

    BOOL m_bParentDisabled;

    void ReEnableParent();
    void PumpMessages();
    BOOL CheckCancelButton();


	CProgressCtrl m_pBar;
	CString m_dir;
	CString m_file;
	DWORD m_szAll;
	DWORD m_szCur;
	bool m_IsCancel;
	afx_msg void OnBnClickedCancel();
	virtual BOOL Create(CWnd* pParent = NULL);
	void InitData(CString dirname , int iSizeAll);
	void NextStep(CString fname);
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
};
