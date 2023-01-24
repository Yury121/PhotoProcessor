// ExifDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "ExifDlg.h"


// CExifDlg dialog

IMPLEMENT_DYNAMIC(CExifDlg, CDialog)

CExifDlg::CExifDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExifDlg::IDD, pParent)
	, m_msg(_T(""))
{

}

CExifDlg::~CExifDlg()
{
}

void CExifDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_msg);
}


BEGIN_MESSAGE_MAP(CExifDlg, CDialog)
END_MESSAGE_MAP()


// CExifDlg message handlers
