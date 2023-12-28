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
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();
	CNNINFOSTRUCT minfo1;
	CNNINFOSTRUCT minfoFaceId;
	CNNINFOSTRUCT minfoAge;
	CNNINFOSTRUCT minfoPerson;
	CNNINFOSTRUCT minfoPersonId;
	CString m_faceCalcXml;
	CString ageXML;
	CString personXML;
	CString personIdXML;
	afx_msg void OnBnFaceId();
	afx_msg void OnBnAgeCalc();
	afx_msg void OnBnPerson();
	afx_msg void OnBnPersonId();
	CString m_infoFID;
	CString m_infoAge;
	CString m_infoPerson;
	CString m_infoPersonID;
	afx_msg void OnBnClickedOk();
	inline void SelectNetwork(CString& opath, CString& info, CNNINFOSTRUCT & cnninfo) {
		CFileDialog dlg(true);
		dlg.m_ofn.lpstrTitle = _T("Select CNN network");
		dlg.m_ofn.lpstrFilter = _T("XML (*.XML;*.xml;)\0*.xml;*.XML\0\0");
		if (dlg.DoModal() == IDOK) {
			std::string mpath = ConvertToUTF8(dlg.GetPathName());
			AfxGetApp()->BeginWaitCursor();
			//	std::string info = GetModelInformation(mpath);
			opath = dlg.GetPathName();
			//	m_info1 = ConvertFromUTF8(info);
			FillModelInformation(mpath, cnninfo);
			info.Format(_T("Type: %i"), cnninfo.Type);
			AfxGetApp()->EndWaitCursor();
			UpdateData(false);
		}
	}
};
