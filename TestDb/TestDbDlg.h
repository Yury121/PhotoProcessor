// TestDbDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <stdio.h>
#include <Gdiplus.h>
#include <vector>


// CTestDbDlg dialog
class CTestDbDlg : public CDialog
{
// Construction
public:
	CTestDbDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTDB_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	CImageList m_imglist;
	CString workPath;
	afx_msg void OnDestroy();
	CString m_info;
	std::vector<CString> v_fname;
	std::vector<CString> v_exif;

//	afx_msg void OnHdnItemclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
};
