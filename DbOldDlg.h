#pragma once
#include "afxcmn.h"
#include <vector>

// CDbOldDlg dialog

class CDbOldDlg : public CDialog
{
	DECLARE_DYNAMIC(CDbOldDlg)

public:
	CDbOldDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDbOldDlg();

// Dialog Data
	enum { IDD = IDD_TESTDB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	CString m_info;
	CString workPath;
	CString m_path;
	CImageList m_imglist;
	std::vector<CString> v_fname;
	std::vector<CString> v_exif;
	std::vector<CString> v_path;
	std::vector<int> v_id;


	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	int m_count;
	afx_msg void OnBnClickedDelimg();
};
