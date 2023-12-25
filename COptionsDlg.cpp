// COptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "afxdialogex.h"
#include "COptionsDlg.h"


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG3, pParent)
	, m_faceXML(_T(""))
	, m_info1(_T(""))
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_faceXML);
	DDX_Control(pDX, IDC_BUTTON1, m_bSelFaceCNN);
	DDX_Text(pDX, IDC_EDIT5, m_info1);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &COptionsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// COptionsDlg message handlers


void COptionsDlg::OnBnClickedButton1()
{
	CFileDialog dlg(true);
	dlg.m_ofn.lpstrTitle = _T("Select CNN network");
	dlg.m_ofn.lpstrFilter = _T("XML (*.XML;*.xml;)\0*.xml;*.XML\0\0");
	if (dlg.DoModal() == IDOK) {
		std::string mpath = ConvertToUTF8(dlg.GetPathName());
		AfxGetApp()->BeginWaitCursor();
		std::string info = GetModelInformation(mpath);
		AfxGetApp()->EndWaitCursor();
		this->m_faceXML = dlg.GetPathName();
		m_info1 = ConvertFromUTF8(info);
		UpdateData(false);
	}

}
