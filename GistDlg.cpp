// GistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "GistDlg.h"


CPen redPen1(PS_SOLID ,0, RGB(255,0,0));
//CPen bluePen(PS_SOLID, 0, RGB(0,0,255));
//CPen greenPen(PS_SOLID, 0, RGB(0,255,0));
//CPen bnrPen(PS_DOT, 0, RGB(255,128,128));
//CPen gbPen(PS_SOLID, 0, RGB(64,64,255));

CBrush brush1(RGB(0,0,255));
CBrush wbrush(RGB(255,255,255));



// CGistDlg dialog

IMPLEMENT_DYNAMIC(CGistDlg, CDialog)

CGistDlg::CGistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGistDlg::IDD, pParent)
	, m_max(_T(""))
	, m_min(_T(""))
	, m_maxdif(_T(""))
	, m_mindif(_T(""))
	, m_count(_T(""))
	, m_smto(_T(""))
	, m_smaxdif(_T(""))
	, m_smindif(_T(""))
{

}

CGistDlg::~CGistDlg()
{
}

void CGistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT111, m_max);
	DDX_Text(pDX, IDC_EDIT112, m_min);
	DDX_Text(pDX, IDC_EDIT113, m_maxdif);
	DDX_Text(pDX, IDC_EDIT114, m_mindif);
	DDX_Text(pDX, IDC_EDIT115, m_count);
	DDX_Control(pDX, IDC_STATICW1, m_wshow);
	DDX_Text(pDX, IDC_EDIT116, m_smto);
	DDX_Text(pDX, IDC_EDIT117, m_smaxdif);
	DDX_Text(pDX, IDC_EDIT118, m_smindif);
}


BEGIN_MESSAGE_MAP(CGistDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CGistDlg message handlers

BOOL CGistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	gist.CalcResults();
	m_max.Format(_T("%d"), gist.maxpos);
	m_min.Format(_T("%d"),  gist.minpos);
	m_count.Format(_T("%d"), gist.count);
	m_maxdif.Format(_T("%d"), gist.maxdif);
	m_mindif.Format(_T("%d"),  gist.mindif);
	m_smto.Format(_T("%f%%"), 100.0*gist.mto/GESTSIZE);
	if (gist.count){
		m_smaxdif.Format(_T("%f%%"), 100.0*gist.maxdif/gist.count);
		m_smindif.Format(_T("%f%%"), 100.0*gist.mindif/gist.count);
	}
	else{
		m_smindif.Format(_T("%f%%"), 100.0*0);
		m_smaxdif.Format(_T("%f%%"), 100.0*0);
	}
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGistDlg::OnPaint()
{
	CPen * oldPen;
//	CBrush brush1;   // Must initialize!
//	CBrush wbrush;   // Must initialize!
//	brush1.CreateSolidBrush(RGB(0,0,255));
//	wbrush.CreateSolidBrush(RGB(255,255,255));
	CRect rect(0,0,256,256);
	int plas = 256/GESTSIZE;

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CDC * vDC;
	//int x, y, val;
	vDC = m_wshow.GetDC();
	vDC->FillRect(&rect,&wbrush);
	if (gist.count){
		for (int i = 0; i< sizeof(gist.distr)/sizeof(__int32); i++){
			rect.right = rect.left+plas;
			rect.top = rect.bottom - 255*gist.distr[i]/gist.distr[gist.maxpos];
			vDC->FillRect(&rect, &brush1);
			rect.left += plas;
		}
	}
	oldPen  = vDC->SelectObject(&redPen1);
	vDC->MoveTo(0,240);
	vDC->LineTo(256,240);
	vDC->SelectObject(oldPen);

	m_wshow.ReleaseDC(vDC);
}

void CGistDlg::UpdateGistogramm(void) {
	CDC * vDC;
	CPen * oldPen;
	CRect rect(0,0,256,256);
	int plas = 256/GESTSIZE;



	gist.CalcResults();
	m_max.Format(_T("%d"), gist.maxpos);
	m_min.Format(_T("%d"),  gist.minpos);
	m_count.Format(_T("%d"), gist.count);
	m_maxdif.Format(_T("%d"), gist.maxdif);
	m_mindif.Format(_T("%d"),  gist.mindif);
	m_smto.Format(_T("%f%%"), 100.0*gist.mto/GESTSIZE);
	m_smindif.Format(_T("%f%%"), 100.0*gist.maxdif/gist.count);
	m_smaxdif.Format(_T("%f%%"), 100.0*gist.mindif/gist.count);
	GetDlgItem(IDC_EDIT111)->SetWindowText(m_max);
	GetDlgItem(IDC_EDIT112)->SetWindowText(m_min);
	GetDlgItem(IDC_EDIT115)->SetWindowText(m_count);
	GetDlgItem(IDC_EDIT113)->SetWindowText(m_maxdif);
	GetDlgItem(IDC_EDIT114)->SetWindowText(m_mindif);
	GetDlgItem(IDC_EDIT116)->SetWindowText(m_smto);
	GetDlgItem(IDC_EDIT117)->SetWindowText(m_smindif);
	GetDlgItem(IDC_EDIT118)->SetWindowText(m_smaxdif);

	vDC = m_wshow.GetDC();
	vDC->FillRect(&rect,&wbrush);
	if (gist.count){
		for (int i = 0; i< sizeof(gist.distr)/sizeof(__int32); i++){
			rect.right = rect.left+plas;
			rect.top = rect.bottom - 255*gist.distr[i]/gist.distr[gist.maxpos];
			vDC->FillRect(&rect, &brush1);
			rect.left += plas;
		}
	}
	oldPen  = vDC->SelectObject(&redPen1);
	vDC->MoveTo(0,240);
	vDC->LineTo(256,240);
	vDC->SelectObject(oldPen);

	m_wshow.ReleaseDC(vDC);

}
