#pragma once


// CExifDlg dialog

class CExifDlg : public CDialog
{
	DECLARE_DYNAMIC(CExifDlg)

public:
	CExifDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExifDlg();

// Dialog Data
	enum { IDD = IDD_EXIFDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_msg;
};
