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
	, m_faceCalcXml(_T(""))
	, ageXML(_T(""))
	, personXML(_T(""))
	, personIdXML(_T(""))
	, m_infoFID(_T(""))
	, m_infoAge(_T(""))
	, m_infoPerson(_T(""))
	, m_infoPersonID(_T(""))
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
	DDX_Text(pDX, IDC_EDIT2, m_faceCalcXml);
	DDX_Text(pDX, IDC_EDIT3, ageXML);
	DDX_Text(pDX, IDC_EDIT4, personXML);
	DDX_Text(pDX, IDC_EDIT6, personIdXML);
	DDX_Text(pDX, IDC_EDIT7, m_infoFID);
	DDX_Text(pDX, IDC_EDIT8, m_infoAge);
	DDX_Text(pDX, IDC_EDIT9, m_infoPerson);
	DDX_Text(pDX, IDC_EDIT10, m_infoPersonID);
}



BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &COptionsDlg::OnBnClickedButton1)
//	ON_WM_ACTIVATE()
ON_BN_CLICKED(IDC_BUTTON2, &COptionsDlg::OnBnFaceId)
ON_BN_CLICKED(IDC_BUTTON3, &COptionsDlg::OnBnAgeCalc)
ON_BN_CLICKED(IDC_BUTTON4, &COptionsDlg::OnBnPerson)
ON_BN_CLICKED(IDC_BUTTON5, &COptionsDlg::OnBnPersonId)
ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionsDlg message handlers



void COptionsDlg::OnBnClickedButton1()
{
	SelectNetwork(m_faceXML, m_info1, minfo1);
}




BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COptionsDlg::OnBnFaceId()
{
	SelectNetwork(m_faceCalcXml, m_infoFID, minfoFaceId);
}


void COptionsDlg::OnBnAgeCalc()
{
	SelectNetwork(ageXML, m_infoAge, minfoAge);

}


void COptionsDlg::OnBnPerson()
{
	SelectNetwork(personXML, m_infoPerson, minfoPerson);
}


void COptionsDlg::OnBnPersonId()
{
	SelectNetwork(personIdXML, m_infoPersonID, minfoPersonId);
}


void COptionsDlg::OnBnClickedOk()
{
	CString msg = _T("");
	std::vector<CString> str = { m_faceXML, m_faceCalcXml, ageXML, personXML, personIdXML };
	std::vector<CNNINFOSTRUCT*> cnn = { &minfo1, &minfoFaceId, &minfoAge, &minfoPerson, &minfoPersonId };
	std::vector<int> focusID = { IDC_EDIT1, IDC_EDIT2, IDC_EDIT3,IDC_EDIT4, IDC_EDIT6 };
	for (int i = 0; i < str.size(); i++) {
		if (!str[i].IsEmpty()) {
			msg += _T("Path: ");
			msg += str[i];
			msg += _T("\r\n");
			msg += ConvertFromUTF8(sModelParameters(*cnn[i]));
			msg += _T("\r\n ----- *** -----\r\n");
#if 0




			if (cnn[i]->Type != (i+1)) {
				msg = _T("This network cannot be used for facial recognition!\r\n");
				AfxMessageBox(msg, MB_ICONHAND);
//				GetDlgItem(focusID[i])->SetFocus();
//				return;
			}
#endif
		}

	}
	if (AfxMessageBox(msg, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK) {
		CDialog::OnOK();
	}
	return;
}
