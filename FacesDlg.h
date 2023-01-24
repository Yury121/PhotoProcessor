#pragma once
#include "afxcmn.h"
#include <vector>


// CFacesDlg dialog
typedef struct {
	int imgId;
	int count;
	int id_start;
	CString name;
}FACES_ENUM_STRUCT;
typedef struct {
	int id;
	int iRef; // reference to index into FACES_ENUM_STRUCT
}FACES_ENUM_ID_STRUCT;

class CFacesDlg : public CDialog
{
	DECLARE_DYNAMIC(CFacesDlg)

public:
	CFacesDlg(int idMain, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFacesDlg();

// Dialog Data
	enum { IDD = IDD_FACESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	CString m_fname;
	CImageList m_imglist;
	CString workPath;

	std::vector<FACES_ENUM_ID_STRUCT> v_id;
	std::vector<FACES_ENUM_STRUCT> v_enum; // this vector must be live all dialog's life-cycle 
//	std::vector<CString> v_fname;
	int m_idMain;
	
	int AddToImageList(int imgId);
	void SetItemInfo(int nItem);


	virtual BOOL OnInitDialog();
//	afx_msg void OnLvnSetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMThemeChangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnEndtrackList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndScrollList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDelimg();
	int m_num;
};
