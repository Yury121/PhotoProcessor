#pragma once
#include "afxdialogex.h"
#include "IntellCNN.h"


// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_faceXML;
	CButton m_bSelFaceCNN;
	afx_msg void OnBnClickedButton1();
	CString m_info1;
};
