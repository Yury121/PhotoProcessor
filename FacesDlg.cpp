// FacesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "FacesDlg.h"
using namespace Gdiplus;

#include "./dbflib/dbflib.h"


// CFacesDlg dialog

IMPLEMENT_DYNAMIC(CFacesDlg, CDialog)

CFacesDlg::CFacesDlg(int idMain, CWnd* pParent /*=NULL*/)
	: CDialog(CFacesDlg::IDD, pParent)
	, m_fname(_T(""))
	, m_num(0)
{
	m_idMain = idMain;
	this->v_id.clear();
	workPath = _T("");
	v_enum.clear();

}

CFacesDlg::~CFacesDlg()
{
}

void CFacesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_fname);
	DDX_Text(pDX, IDC_EDIT2, m_num);
}


BEGIN_MESSAGE_MAP(CFacesDlg, CDialog)
//	ON_NOTIFY(LVN_SETDISPINFO, IDC_LIST1, &CFacesDlg::OnLvnSetdispinfoList1)
//ON_NOTIFY(NM_THEMECHANGED, IDC_LIST1, &CFacesDlg::OnNMThemeChangedList1)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CFacesDlg::OnLvnItemchangedList1)
//ON_NOTIFY(HDN_ENDTRACK, 0, &CFacesDlg::OnHdnEndtrackList1)
ON_NOTIFY(LVN_ENDSCROLL, IDC_LIST1, &CFacesDlg::OnLvnEndScrollList1)
ON_BN_CLICKED(IDOK, &CFacesDlg::OnBnClickedOk)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFacesDlg::OnNMDblclkList1)
ON_BN_CLICKED(ID_DELIMG, &CFacesDlg::OnBnClickedDelimg)
END_MESSAGE_MAP()


// CFacesDlg message handlers

BOOL CFacesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.DeleteAllItems();
	ShowWindow(SW_SHOW);
	m_imglist.Create(256,256,ILC_COLOR16,100,1);
	CBitmap bmpNull;
	bmpNull.LoadBitmap(IDB_BITMAP2);
	CString tmp_img;
	int iBuf[1000];

	int count = 0;
	if (m_idMain > 0){
		count = AddToImageList(m_idMain);
		if (v_enum.size()){
			m_fname = v_enum[0].name;
		}
	}
	else{
		int iBuf[1000] ={};
		count = GetIdCount(iBuf,1000);
		for (int i = 0; i< count; i++){
			AddToImageList(iBuf[i]);
		}
		if (v_enum.size() >0) m_fname = v_enum[0].name;
		
	}
	
	m_imglist.Add(&bmpNull, RGB(255, 255, 255));
	count = m_imglist.GetImageCount();
	m_list.SetImageList(&m_imglist,TVSIL_NORMAL);
	int k = 0;
	for(int i=0; i< v_id.size(); i++){
		tmp_img.Format(_T("image %2d"),i);
		m_list.InsertItem(i,tmp_img,i);
		m_list.SetItemData(i,v_id[i].iRef);		
 	}
	if (v_id.size()==0){
		m_list.InsertItem(0,_T("No faces found"),0);
		m_list.SetItemData(0, -1);
	}
	if (count){
		m_list.SetView(0);
		m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_list.SetSelectionMark(0);
	}

	m_num = v_id.size();
	UpdateData(false);
//	if (mem) 		free(mem);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CFacesDlg::AddToImageList(int imgId){
	FACES_ENUM_STRUCT faces;
	FACES_ENUM_ID_STRUCT sid;
	faces.imgId = imgId;
	if (!m_imglist.GetSafeHandle())
		return 0;
	CString tmp_img;
	int bsize = 4*256*256;
	tmp_img.Format(_T("%s/face.JPG"), workPath);
	BITMAP bitmap;

	bitmap.bmBitsPixel = 32;
	bitmap.bmHeight = bitmap.bmWidth = 256;
	bitmap.bmPlanes = 1;
	bitmap.bmType = 0;
	bitmap.bmWidthBytes = 1024;
	CBitmap srcbmp;
	CBitmap bmpNull;
	bmpNull.LoadBitmap(IDB_BITMAP2);
	int cnt=0;
	Bitmap * bbb=0;
	unsigned char * ptrmem =0;
	unsigned char * ptrimg = 0;

	int idBuf[100] = {};
	unsigned char * mem = 0;
	int count = GetFaceIdInfo(imgId, idBuf, 100, faces.name);
	if (count){
		mem = (unsigned char * ) malloc(bsize);
		memset(mem, 0xff, bsize);
	}
	if (!mem) 
		return 0;

	bitmap.bmBits = mem;
	faces.id_start = m_imglist.GetImageCount();

	for (int i = 0; i< count; i++){
		sid.id = idBuf[i];
		sid.iRef = v_enum.size();
//		v_id.push_back(idBuf[i]);
		v_id.push_back(sid);
		if (GetFaceIdImage(idBuf[i], tmp_img) > 100){
			bbb = Bitmap::FromFile(tmp_img.GetBuffer());
			tmp_img.ReleaseBuffer();
			if (bbb){
				HBITMAP hbm;
				bbb->GetHBITMAP(/*Gdiplus::Color(0,0,0)*/ NULL, &hbm);
				CBitmap  * cBmp = CBitmap::FromHandle(hbm); 
				BITMAP bimg1;
				memset(mem,0xff,bsize);
				if (cBmp->GetBitmap(&bimg1)){
	//				memcpy(mem, bimg1.bmBits, bimg1.bmHeight*bimg1.bmWidthBytes);
					for(int k =0; k< min(255,bimg1.bmHeight); k++){
						ptrmem = mem + (255 -k )*bitmap.bmWidthBytes;
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
		}//end  for process face
	}//end loop for images
	faces.count = cnt;
	if (mem) free(mem);
	if (faces.count)
		v_enum.push_back(faces);
	return
		faces.count;

}

//void CFacesDlg::OnLvnSetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

//void CFacesDlg::OnNMThemeChangedList1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// This feature requires Windows XP or greater.
//	// The symbol _WIN32_WINNT must be >= 0x0501.
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CFacesDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (nItem < 0) {
		m_fname=_T("");
		UpdateData(false);
		return;
	}

	if (v_enum.size()>0){
		int cur = (int) m_list.GetItemData(nItem);
		if ((cur >=0)&&(cur<v_enum.size())){
			m_fname = v_enum[cur].name;
		}
#if 0
		for(int i = 0; i<v_enum.size(); i++){
			cur += v_enum[i].count;
			if (nItem < v_enum[i].id_start){
				break;
			}
			m_fname = v_enum[i].name;
		}
#endif
		UpdateData(false);
	}

	*pResult = 0;
}

//void CFacesDlg::OnHdnEndtrackList1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	int nItem = m_list.GetNextItem(-1,LVNI_FOCUSED);
//	*pResult = 0;
//}

void CFacesDlg::OnLvnEndScrollList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Internet Explorer 5.5 or greater.
	// The symbol _WIN32_IE must be >= 0x0560.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	CPoint pt;
	CPoint ptO;
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	if (m_list.GetOrigin(&ptO) ){
		for (int i =0; i<m_list.GetItemCount(); i++){
			if (m_list.GetItemPosition(i,&pt)){
				if (pt.x > ptO.x){
					nItem = i;
					m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
					m_list.SetSelectionMark(i);
					break;
				}
			}
			else
				break;
		}
		SetItemInfo(nItem);

	}
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CFacesDlg::SetItemInfo(int nItem){
	if (nItem <0) return;
	int data = (int) m_list.GetItemData(nItem);
	if ((data >= 0)&&(data<v_enum.size())){  
		m_fname = v_enum[data].name;
	}
	UpdateData(false);
}

void CFacesDlg::OnBnClickedOk()
{
	if (m_idMain == 0){
		int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
		if (nItem < 0) return;
		int cur = (int) m_list.GetItemData(nItem);
		if ((cur >=0)&&(cur<v_enum.size())){
			m_idMain = v_enum[cur].imgId;
			m_fname = v_enum[cur].name;
		}
	}

//	m_fname = GetAlternatePath(m_idMain);
	OnOK();
}

void CFacesDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();
	this->PostMessage(WM_COMMAND,IDOK);

	*pResult = 0;
}

void CFacesDlg::OnBnClickedDelimg()
{
	UpdateData();
	int nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	CString msg =_T("----");
	if (nItem <0) return;
	int data = (int) m_list.GetItemData(nItem);
	if (data< 0) return;
	if (v_id.size() <= 0) return;
	if (data < v_enum.size()){
		msg.Format(_T("Do You want delete item: %s?\r\nFile : %s"), m_list.GetItemText(nItem,0),v_enum[data].name);
	}
	
	if (AfxMessageBox(msg,MB_YESNO|MB_ICONQUESTION)== IDYES){
		m_list.DeleteItem(nItem);
		DeleteFaceImg(v_id[nItem].id);
		v_id.erase(v_id.begin()+nItem);
		if (m_list.GetItemCount() > 0){
			m_list.SetView(0);
			m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			m_list.SetSelectionMark(0);
		}
		if (v_id.size()==0){
			m_list.InsertItem(0,_T("No faces found"),m_imglist.GetImageCount()-1);
			m_list.SetItemData(0, -1);
		}

	}
	m_num = v_id.size();
	UpdateData(false);

}
