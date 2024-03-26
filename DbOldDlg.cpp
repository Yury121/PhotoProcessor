// DbOldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "DbOldDlg.h"


using namespace Gdiplus;

#include "slite.h"
//#include "./dbflib/dbflib.h"


// CDbOldDlg dialog

IMPLEMENT_DYNAMIC(CDbOldDlg, CDialog)

CDbOldDlg::CDbOldDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbOldDlg::IDD, pParent)
	, m_info(_T(""))
	, m_count(0)
{
	v_fname.clear();
	v_exif.clear();
	v_path.clear();
	m_path =_T("");
	v_id.clear();

}

CDbOldDlg::~CDbOldDlg()
{
}

void CDbOldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Text(pDX, 1002, m_info);
	DDX_Text(pDX, IDC_EDIT1, m_count);
}


BEGIN_MESSAGE_MAP(CDbOldDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CDbOldDlg::OnNMDblclkList2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CDbOldDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDOK, &CDbOldDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DELIMG, &CDbOldDlg::OnBnClickedDelimg)
END_MESSAGE_MAP()


// CDbOldDlg message handlers

BOOL CDbOldDlg::OnInitDialog()
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
	ShowWindow(SW_SHOW);
	m_imglist.Create(128,128,ILC_COLOR24,100,1);
	CBitmap bmpNull;
	bmpNull.LoadBitmap(IDB_BITMAP1);

	
	InitLocalDB(workPath);
	int Ids[1000] = {};
	int count = GetIdCount(Ids,1000);
	m_count = count;
	m_list.DeleteAllItems();
	CString tmp_name;
	CString tmp_exif;
	CString tmp_img;
	Bitmap * bbb=0;
	int istart = 0;
	int istop = 0;
	CString tmp;
	CString tmp1;

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
		GetIdInfo(Ids[i],tmp_name, tmp_img, tmp_exif, tmp);
		v_id.push_back(Ids[i]);
		v_fname.push_back(tmp_name);
		dx = tmp_exif.Find( _T("System") );
		tmp1 = tmp_exif.Mid(dx);
		dx = tmp1.Find( _T("FileName") );
		tmp_exif = tmp1.Mid(dx);
		

		v_exif.push_back(tmp_exif);
		v_path.push_back(tmp);
		bbb = Bitmap::FromFile(tmp_img.GetBuffer());
		tmp_img.ReleaseBuffer();
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
		m_list.SetItemData(i, i);
	}
	if (count){
		m_list.SetView(0);
		m_info = v_exif[0];
		m_path = v_path[0];
		m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_list.SetSelectionMark(0);
	}
	UpdateData(false);
	
	
	if (mem) free(mem);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDbOldDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0) return;
	nItem = m_list.GetItemData(nItem);
	if (nItem < 0){
		m_info = _T("");
		m_path =_T("");
	}
	else{
		if (nItem < v_exif.size())	{
			m_info = v_exif[nItem];
			m_path = v_path[nItem];
		}
		else{
			m_info = _T("");
			m_path =_T("");
		}
		
	}
	UpdateData(false);
	*pResult = 0;
	this->PostMessage(WM_COMMAND,IDOK);
//	EndDialog(IDOK);

}

void CDbOldDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0) return;
	nItem = m_list.GetItemData(nItem);

	if (nItem < 0){
		m_info = _T("");
		m_path =_T("");
	}
	else{
		if (nItem < v_exif.size())	{
			m_info = v_exif[nItem];
			m_path = v_path[nItem];
		}
		else{
			m_info = _T("");
			m_path =_T("");
		}
		
	}
	UpdateData(false);
	*pResult = 0;
}

void CDbOldDlg::OnBnClickedOk()
{
	UpdateData();
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0){
		m_info = _T("");
		m_path =_T("");
		AfxMessageBox(_T("Select image, please"),MB_ICONINFORMATION);
		return;// OnOK();
	}
	nItem = m_list.GetItemData(nItem);

#if 0

	HANDLE hFile = CreateFile(m_path,FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (hFile == INVALID_HANDLE_VALUE){
		CString newpath = _T("");
		CString disk = _T("");
		CString path =_T("");
		int idBuf[100] ={};
		int count = GetDubId(v_id[nItem],idBuf, 100);
		for (int i = 0; i<count; i++){
			path = _T("");
			newpath = GetDubPath(idBuf[i], disk);
			disk = FindDiskSerial(disk);
			if (disk.IsEmpty()) 
				continue;
			else{
				path.Format(_T("%s%s"),disk.Left(2), newpath.Mid(2));
				hFile = CreateFile(path,FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
				if (hFile != INVALID_HANDLE_VALUE){
					CloseHandle(hFile);
					m_path = path;
					break;
				}
			}
				
		}
	}
	else
		CloseHandle(hFile);
#endif
	CString str;
	str = GetAlternatePath(v_id[nItem], m_path);
	if (str.IsEmpty()){
		int k = m_path.ReverseFind(_T('/'));
		if (k >= 0){
			//str.Format(_T("%s/%s"), this->workPath, m_path.Mid(k+1));
			str.Format(_T("%s"),  m_path.Mid(k + 1));
		}
	}
	m_path = str;

//	UpdateData();
	OnOK();
}

void CDbOldDlg::OnBnClickedDelimg()
{
	UpdateData();
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0){
		m_info = _T("");
		m_path =_T("");
		AfxMessageBox(_T("There are no selected images"),MB_ICONINFORMATION);
		return;// OnOK();
	}
	int idNum = m_list.GetItemData(nItem);
	CString msg;
	msg.Format(_T("Do you want to remove image %s from database?"), v_fname[idNum]);
	if (AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION) != IDYES){
		return;
	}
	DeleteImg(v_id[idNum]);
	m_list.DeleteItem(nItem);
	m_info = _T("");
	m_path =_T("");

	this->m_count = m_list.GetItemCount();
	if (m_count){
		m_list.RedrawItems(min(0,nItem-1),max(0,m_count-1));
		m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_list.SetSelectionMark(0);
		m_list.SetView(0);
		m_list.EnsureVisible(0, FALSE);
		m_list.Invalidate();
		m_list.UpdateWindow();
		m_list.Arrange(LVA_DEFAULT );
		nItem = m_list.GetItemData(0);
		m_info = v_exif[nItem];
		m_path = v_path[nItem];
	}
	UpdateData(false);
	this->UpdateWindow();

}
