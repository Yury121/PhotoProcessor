#include "StdAfx.h"
#include "PhotoDB.h"

CString dbPath =_T("");

CPhotoDB::CPhotoDB(void)
{
	m_path = _T("");
}

CPhotoDB::~CPhotoDB(void)
{
	if (m_imgset.IsOpen()) m_imgset.Close();
	if (m_dubset.IsOpen()) m_dubset.Close();
	if (m_faceset.IsOpen()) m_faceset.Close();
	if (IsOpen()) this->Close();

}

int CPhotoDB::Init(CString path){
	if (IsOpen()){
		if (path.CompareNoCase(m_path)==0) return 1; // the same database
		return -1; // database already open
	}
	CString tmpname = path + _T("/vf.db");
	dbPath = _T("");
	HANDLE hFile = ::CreateFile(tmpname, 0,0,0, OPEN_EXISTING,FILE_ATTRIBUTE_HIDDEN,0);
	if (hFile != INVALID_HANDLE_VALUE){
		CloseHandle(hFile);
		hFile =  INVALID_HANDLE_VALUE;
	}
	else{
		HMODULE  hModule = GetModuleHandle(_T("dbflib"));
		// create database
		HRSRC hrc = FindResource(hModule,_T("#102"),_T("DB1"));
		HGLOBAL hgbl = LoadResource(hModule,hrc);
		unsigned char * hMem = (unsigned char *) LockResource(hgbl);
		DWORD dwWrite;
		if (hMem){
			hFile = CreateFile(tmpname.GetBuffer(), GENERIC_READ|GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN,0);
			if (hFile  != INVALID_HANDLE_VALUE){
				WriteFile(hFile, hMem, SizeofResource(hModule,hrc), &dwWrite, 0);
				CloseHandle(hFile);
				hFile =  INVALID_HANDLE_VALUE;
			}
			else{
//				AfxGetApp()->EndWaitCursor();
				AfxMessageBox(_T("Не могу создать базу данных"));
				return -3;
			}
		}

	}
	try{
		CString cmpname = path + _T("/~~vf.db");
		if (CopyFile(tmpname,cmpname,false) ){
			DeleteFile(tmpname);
			this->m_pWorkspace->CompactDatabase(cmpname,tmpname);
			DeleteFile(cmpname);
			SetFileAttributes(tmpname, FILE_ATTRIBUTE_HIDDEN);
		}
		Open(tmpname, true);

	}
	catch( CDaoException* e )
	{
		AfxMessageBox( e->m_pErrorInfo->m_strDescription );
		return -2;
	}
	m_path = path;
	dbPath = tmpname;
	if (!m_imgset.IsOpen()) m_imgset.Open();
	if (!m_dubset.IsOpen()) m_dubset.Open();
	if (!m_faceset.IsOpen()) m_faceset.Open();


	return 1;
}
// RECORDSET CLASSES
CImgSet::CImgSet(void)
{
	m_id = 0;
 	m_nFields = 37;

}

CImgSet::~CImgSet(void)
{
	if (this->IsOpen()) this->Close();
}

CString CImgSet::GetDefaultDBName(){
	return dbPath; // db name
}// Default database name
 void CImgSet::DoFieldExchange(CDaoFieldExchange* pFX){  // RFX support
		//{{AFX_FIELD_MAP(VUserSet)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Long(pFX, _T("[Id]"), m_id);
	DFX_Text(pFX, _T("[FileName]"), m_filename);
	DFX_Text(pFX, _T("[Directory]"), m_dir);	
	DFX_LongBinary(pFX, _T("[exif]"), m_exif, 16000);	
	DFX_DateTime(pFX, _T("[FileCreateDate]"), m_fcDate);
	DFX_Text(pFX, _T("[FileType]"), m_fType);	
	DFX_Long(pFX, _T("[ImageWidth]"), m_width);
	DFX_Long(pFX, _T("[ImageHeight]"), m_height);	
	DFX_Text(pFX, _T("[Make]"), m_make);
	DFX_Text(pFX, _T("[Model]"), m_model);
	DFX_Text(pFX, _T("[ExposureTime]"), m_exposureTime);
	DFX_Text(pFX, _T("[FNumber]"), m_Fnumber);	
	DFX_Text(pFX, _T("[ExposureProgram]"), m_exProg);	
	DFX_Text(pFX, _T("[ISO]"), m_ISO);	
	DFX_DateTime(pFX, _T("[DateTimeOriginal]"), m_dtOrigin);
	DFX_Text(pFX, _T("[OwnerName]"),  m_Owner);	
	DFX_Text(pFX, _T("[SerialNumber]"), m_serial);	
	DFX_Text(pFX, _T("[FacesDetected]"), m_facesDetect);	
	DFX_Text(pFX, _T("[LensInfo]"), m_LensInfo);	
	DFX_Text(pFX, _T("[Lens]"), m_Lens);	
	DFX_Text(pFX, _T("[LensModel]"), m_LensModel);	
	DFX_Text(pFX, _T("[LensSerialNumber]"), m_LensSerial);	
	DFX_Text(pFX, _T("[InternalSerialNumber]"), m_IntSerial);	
	DFX_Text(pFX, _T("[Aperture]"), m_Aperture);	
	DFX_Text(pFX, _T("[BlueBalance]"), m_BlueBalance);	
	DFX_Text(pFX, _T("[RedBalance]"), m_RedBalance);	
	DFX_Text(pFX, _T("[CircleOfConfusion]"), m_cOfC);	
	DFX_Text(pFX, _T("[DOF]"), m_DOF);	
	DFX_Text(pFX, _T("[Depth Of Field]"), m_Depth);	
	DFX_Text(pFX, _T("[FOV]"), m_FOV);	
	DFX_Text(pFX, _T("[Field Of View]"), m_fOfView);	
	DFX_Text(pFX, _T("[HyperfocalDistance]"), m_hfDistance);	
	DFX_Text(pFX, _T("[Lens35efl]"), m_Lens35);	
	DFX_Text(pFX, _T("[LightValue]"), m_LightValue);	
	DFX_Text(pFX, _T("[Megapixels]"), m_Megapixels);	
	DFX_LongBinary(pFX, _T("[Miniimage]"), m_miniImg);	
//	DFX_Text(pFX, _T("[HashStr]",32), m_hash);
	DFX_Text(pFX, _T("[HashStr]"), m_hash);
//}}AFX_FIELD_MAP
}
 CString CImgSet::GetDefaultSQL(){		// Default SQL for Recordset
		m_strSort = _T("[img].[Directory]");
	return _T("[img]");
}
#if 0
virtual CString CImgSet::GetDefaultSQL(){		// Default SQL for Recordset
	//	m_strSort = _T("[UsersTbl].[Email]");
	return _T("[FileName]");
}
virtual void CImgSet::DoFieldExchange(CDaoFieldExchange* pFX){  // RFX support
		//{{AFX_FIELD_MAP(VUserSet)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Long(pFX, _T("[Id]", m_id);
	DFX_Text(pFX, _T("[FileName]"), m_filename);
	DFX_Text(pFX, _T("[Directory"]), m_path);	
	DFX_Binary(pFX, _T("[exif]"), m_exif, 16000);	
	DFX_DateTime(pFX, _T("[FileCreateDate]"), m_fcDate);
	DFX_Text(pFX, _T("[FileType]"), m_fType);	
	DFX_Long(pFX, _T("ImageWidth]"), m_width);
	DFX_Long(pFX, _T("ImageHeight]"), m_height);	
	DFX_Text(pFX, _T("[ExposureTime]"), m_exposureTime);
	DFX_Text(pFX, _T("[FNumber]"), m_Fnumber);	
	DFX_Text(pFX, _T("[ExposureProgram]"), m_exProg);	
	DFX_Text(pFX, _T("[ISO]"), m_ISO);	
	DFX_DateTime(pFX, _T("[DateTimeOriginal]"), m_dtOrigin);
	DFX_Text(pFX, _T("[OwnerName]"), m_Owner);	
	DFX_Text(pFX, _T("[SerialNumber]"), m_serial);	
	DFX_Text(pFX, _T("[FacesDetected]"), m_facesDetect);	
	DFX_Text(pFX, _T("[LensInfo]"), m_LensInfo);	
	DFX_Text(pFX, _T("[Lens]"), m_Lens);	
	DFX_Text(pFX, _T("[LensModel]"), m_LensModel);	
	DFX_Text(pFX, _T("[LensSerialNumber]"), m_LensSerial);	
	DFX_Text(pFX, _T("[InternalSerialNumber]"), m_IntSerial);	
	DFX_Text(pFX, _T("[Aperture]"), m_Aperture);	
	DFX_Text(pFX, _T("[BlueBalance]"), m_BlueBalance);	
	DFX_Text(pFX, _T("[RedBalance]"), m_RedBalance);	
	DFX_Text(pFX, _T("[CircleOfConfusion]"), m_cOfC);	
	DFX_Text(pFX, _T("[DOF]"), m_DOF);	
	DFX_Text(pFX, _T("[Depth Of Field]"), m_Depth);	
	DFX_Text(pFX, _T("[FOV]"), m_FOV);	
	DFX_Text(pFX, _T("[Field Of View]"), m_fOfView);	
	DFX_Text(pFX, _T("[HyperfocalDistance]"), m_hfDistance);	
	DFX_Text(pFX, _T("[Lens35efl]"), m_Lens35);	
	DFX_Text(pFX, _T("[LightValue]"), m_LightValue);	
	DFX_Text(pFX, _T("[Megapixels]"), m_Megapixels);	
	DFX_Binary(pFX, _T("[Miniimage]"), m_miniImg);	
	DFX_Text(pFX, _T("[HashStr]",32), m_hash);
//}}AFX_FIELD_MAP
}
#endif


CImgrefSet::CImgrefSet(void)
{
	m_id = 0;
 	m_nFields = 6;

}

CImgrefSet::~CImgrefSet(void)
{
	if (this->IsOpen()) this->Close();
}
CString CImgrefSet::GetDefaultDBName(){
	return dbPath; // db name
}// Default database name

CString CImgrefSet::GetDefaultSQL(){		// Default SQL for Recordset
	//	m_strSort = _T("[UsersTbl].[Email]");
	return _T("[img1]");
}
void CImgrefSet::DoFieldExchange(CDaoFieldExchange* pFX){  // RFX support
		//{{AFX_FIELD_MAP(VUserSet)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Long(pFX, _T("[Id]"), m_id);
	DFX_Long(pFX, _T("[idMain]"), m_idmain);
	DFX_Text(pFX, _T("[FileName]"), m_filename);
	DFX_Text(pFX, _T("[Directory]"), m_dir);	
	DFX_Text(pFX, _T("[Computer]"), m_comp);	
	DFX_Text(pFX, _T("[DiskInfo]"), m_diskinfo);
//}}AFX_FIELD_MAP
}


CFaceSet::CFaceSet(void)
{
	m_id = 0;
 	m_nFields = 4;

}

CFaceSet::~CFaceSet(void)
{
	if (this->IsOpen()) this->Close();
};

CString CFaceSet::GetDefaultDBName(){
	return dbPath; // db name
}// Default database name

CString CFaceSet::GetDefaultSQL(){		// Default SQL for Recordset
	//	m_strSort = _T("[faces].[m_idmain]");
	return _T("[faces]");
}
void CFaceSet::DoFieldExchange(CDaoFieldExchange* pFX){  // RFX support
		//{{AFX_FIELD_MAP(VUserSet)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Long(pFX, _T("[Idf]"), m_id);
	DFX_LongBinary(pFX, _T("[image]"), m_image);	
	DFX_Long(pFX, _T("[imageId]"), m_idmain);
	DFX_Text(pFX, _T("[sHash]"), m_hash);
//}}AFX_FIELD_MAP
}