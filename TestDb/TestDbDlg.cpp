// TestDbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestDb.h"
#include "TestDbDlg.h"
using namespace Gdiplus;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ULONG_PTR gdiplusToken;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestDbDlg dialog




CTestDbDlg::CTestDbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDbDlg::IDD, pParent)
	, m_info(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_info);
}

BEGIN_MESSAGE_MAP(CTestDbDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
//	ON_NOTIFY(HDN_ITEMCLICK, 0, &CTestDbDlg::OnHdnItemclickList2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CTestDbDlg::OnNMDblclkList2)
//	ON_NOTIFY(HDN_ITEMCHANGED, 0, &CTestDbDlg::OnHdnItemchangedList2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CTestDbDlg::OnLvnItemchangedList2)
END_MESSAGE_MAP()


// CTestDbDlg message handlers

BOOL CTestDbDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	BITMAP bitmap;
	int bsize = 4*128*128;
	unsigned char * mem = (unsigned char * ) malloc(bsize);
	memset(mem, 0xff, bsize);
	bitmap.bmBits = mem;
	bitmap.bmBitsPixel = 32;
	bitmap.bmHeight = bitmap.bmWidth = 128;
	bitmap.bmPlanes = 1;
	bitmap.bmType = 0;
	bitmap.bmWidthBytes = 512;
	CBitmap srcbmp;
//	srcbmp.CreateBitmapIndirect(&bitmap);
	

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_SHOW);
	m_imglist.Create(128,128,ILC_COLOR24,100,1);
	CBitmap bmpNull;
	bmpNull.LoadBitmap(IDB_BITMAP1);

	
	InitLocalDB(workPath);
	int Ids[1000] = {};
	int count = GetIdCount(Ids,1000);
	m_list.DeleteAllItems();
	CString tmp_name;
	CString tmp_exif;
	CString tmp_img;
	CString tmp;
	Bitmap * bbb=0;

	v_fname.clear();
	v_exif.clear();
	int cnt=0;
	unsigned char * ptrmem =0;
	unsigned char * ptrimg = 0;
	int dx = 0;
	int dy = 0;
//	count = min(count, 10); // DEBUG
	for (int i = 0; i< count; i++){
		tmp_img.Format(_T("%s/mem%d.JPG"), workPath, Ids[i]);
		tmp_name = _T("");
		tmp_exif =_T("");
		GetIdInfo(Ids[i],tmp_name, tmp_img, tmp_exif,tmp);
		v_fname.push_back(tmp_name);
		v_exif.push_back(tmp_exif);
		bbb = Bitmap::FromFile(tmp_img.GetBuffer());
		tmp_name.ReleaseBuffer();
		if (bbb){
			HBITMAP hbm;
			bbb->GetHBITMAP(/*Gdiplus::Color(0,0,0)*/ NULL, &hbm);
			CBitmap  * cBmp = CBitmap::FromHandle(hbm); 
			BITMAP bimg1;
			memset(mem,0xff,bsize);
			if (cBmp->GetBitmap(&bimg1)){
				dx = max(0, (127- bimg1.bmHeight)/2);
				dy = max(0, (128- bimg1.bmWidth)/2)*4;
//				memcpy(mem, bimg1.bmBits, bimg1.bmHeight*bimg1.bmWidthBytes);
				for(int k =0; k< min(128,bimg1.bmHeight); k++){
					ptrmem = mem + (127 -k -dx )*bitmap.bmWidthBytes +dy;
					ptrimg = ((unsigned char *) bimg1.bmBits) + bimg1.bmWidthBytes*k;
					memcpy(ptrmem,ptrimg, bimg1.bmWidthBytes);
				}
			}
			srcbmp.CreateBitmapIndirect(&bitmap);
			if (m_imglist.Add(&srcbmp, RGB(0, 0, 0))< cnt){
				m_imglist.Add(&bmpNull, RGB(255, 255, 255));
			}
			srcbmp.DeleteObject();
			delete[] bbb;
			bbb = 0;
		}
		else{
			m_imglist.Add(&bmpNull, RGB(255, 255, 255));
		}
		cnt++;
		DeleteFile(tmp_img);
	};//end loop count
	count = m_imglist.GetImageCount();
	m_list.SetImageList(&m_imglist,TVSIL_NORMAL);
	for(int i=0; i< v_fname.size(); i++){
		m_list.InsertItem(i,v_fname[i],i);
	}
	m_list.SetView(0);
	m_info = v_exif[0];
	m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_list.SetSelectionMark(0);
	UpdateData(false);
	
	
	
	
	
	if (mem) free(mem);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDbDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDbDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDbDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestDbDlg::OnDestroy()
{
	ReleaseLocalDB();
	m_imglist.DeleteImageList();
	if(gdiplusToken!= NULL){
		Gdiplus::GdiplusShutdown(gdiplusToken);
		gdiplusToken = NULL;
	}
	
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

//void CTestDbDlg::OnHdnItemclickList2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CTestDbDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

//void CTestDbDlg::OnHdnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CTestDbDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0){
		m_info = _T("");
	}
	else{
		if (nItem < v_exif.size())	m_info = v_exif[nItem];
		else m_info = _T("");
	}
	UpdateData(false);
	*pResult = 0;
}
