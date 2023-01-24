#pragma once

#include "tools.h"
// CGistDlg dialog

class CGistDlg : public CDialog
{
	DECLARE_DYNAMIC(CGistDlg)

public:
	CGistDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGistDlg();

// Dialog Data
	enum { IDD = IDD_GISTOGRAMM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CString m_max;
	CString m_min;
	CString m_maxdif;
	CString m_mindif;
	CString m_count;
	CStatic m_wshow;
	CString m_smto;
	CString m_smaxdif;
	CString m_smindif;
	GISTOGRAMM gist;
	void UpdateGistogramm(void);
};
