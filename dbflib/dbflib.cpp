// dbflib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dbflib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
static CPhotoDB * m_db =NULL;

using namespace std;


CString FindDiskSerial(CString sSerial){
	CString sDisk =_T("");
	DWORD dwVol = 0;
	TCHAR buf[256]={};
	CString str;
	DWORD dwCount = GetLogicalDriveStrings(256,buf);
	for(int i = 0; i<dwCount-1;){
		sDisk = &buf[i];
		i+= sDisk.GetLength()+1;
		::GetVolumeInformation(sDisk, NULL, 0, &dwVol, 0, 0, 0, 0);
		str.Format(_T("%X"), dwVol);
		if (sSerial.Find(str) >= 0){
			break;
		}
		sDisk =_T("");
	}

	return sDisk;
}

bool AddToDublicate(int id, CString &fname, CString &dir, CString &info);

CString  exifconst[] = {_T("FileName   "), _T("Directory "), _T("FileCreateDate "), /* 0, 1, 2 */
						_T("FileType "), _T("ImageWidth "), _T("ImageHeight "), /* 3, 4, 5 */
						_T("Make"), _T("Model"), _T("ExposureTime "), _T("FNumber "), /* 6, 7, 8, 9 */
						_T("ExposureProgram "), _T("ISO "), _T("DateTimeOriginal "),  /* 10, 11, 12 */
						_T("OwnerName "), _T("SerialNumber "), _T("FacesDetected "), /* 13, 14, 15 */
						_T("LensInfo"),  _T("Lens "),								/* 16, 17 */
						_T("LensModel"), _T("LensSerialNumber"), _T("InternalSerialNumber "),  /* 18, 19, 20 */
						_T("Aperture "), _T("BlueBalance "), _T("RedBalance "),				/* 21, 22, 23 */
						_T("CircleOfConfusion "),  	_T("DOF"), _T("Depth Of Field "),  /* 24, 25, 26 */
						_T("FOV "), _T("Field Of View "),  _T("ShutterSpeed"), _T("FocalLength35efl "), /* 27, 28, 29 30 */
						_T("HyperfocalDistance "), _T("Lens35efl "), _T("LightValue "), _T("Megapixels ") /*  31, 32, 33, 34 */
};

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
	}

	return nRetCode;
}
COleDateTime GetFromExif(CString dtstr){
	CString str = dtstr;
	int nYear =1111;
	int nMonth =11;
	int nDay=11;
	int nHour=11;
	int nMin=11;
	int nSec=11;
	int start = 0;
	int end = 0;
	COleDateTime dt;
	if (dtstr.GetLength() > 4){
		nYear = _wtoi(dtstr);
		start =4;// dtstr.Find(':');
		str = dtstr.Mid(start+1);
		if (str.GetLength() > 1){
			nMonth = _wtoi(str);
			start =2;//str.Find(':');
			str = str.Mid(start+1);
			if (str.GetLength() >1){
				nDay = _wtoi(str);
				start =2;//str.Find(' ');
				str = str.Mid(start+1);
				if (str.GetLength() >1){
					nHour = _wtoi(str);
					start =2;//str.Find(':');
					str = str.Mid(start+1);
					if (str.GetLength() > 1){
						nMin = _wtoi(str);
						start =2;//str.Find(':');
						str = str.Mid(start+1);
						if (str.GetLength() > 1)	nSec = _wtoi(str);
					}//end check min
				}//end check hour
			}//end check day
		}//end check Month
	}//end check Year
	dt = COleDateTime(nYear,nMonth,nDay,nHour,nMin,nSec);
	if (dt.GetStatus()) dt= COleDateTime(1111,11,11,11,11,11);
//	if (!dt.ParseDateTime(dtstr,0,0 )){
//		dt = COleDateTime(1000,11,11,11,11,11);
//	}
	
	return dt;
}

DBFLIB_API int GetFileHash(LPCTSTR fpath, unsigned char hash[16])
{
	
	MD5_CTX mdhash;
	//TLDOM::PTR<IHash> pHash; // hash common interface
	//RESULT Result = E_FAILED;
	unsigned char Buf[4096] ={};
	int szSize = 0; // variable for sizeof Hash
	HANDLE hFile = INVALID_HANDLE_VALUE; // handle for  file
	DWORD dwRead = 0;

//	memset(hash, 0xff, 16);
//	Result = pHash.CreateInstance(UIID_IMd5);
//	if (Result <= 0)	{ 
//		return -1;
//	}
	MD5Init(&mdhash);
	hFile = CreateFile(fpath,GENERIC_READ, FILE_SHARE_READ,0, OPEN_EXISTING,0,0);	 
	// check result
	if (hFile == INVALID_HANDLE_VALUE){
		return -2;
	}
	
	//pHash->Clear();
	do{
		if (!ReadFile(hFile, Buf, sizeof(Buf), &dwRead,0) ){
			dwRead = 0; // error red file
			break; 
		}
		szSize += dwRead;
		MD5Update(&mdhash, Buf, dwRead);
		//pHash->Hash(Buf, dwRead); // calculate hash
	}while(dwRead > 0);
//	pHash->Final(hash, &mdhash); // calculate hash value
	MD5Final(hash, &mdhash);
	CloseHandle(hFile); // close file object
	//pHash.Release(); // fre hash interface

	return szSize;
}//end gethash
// parse unicode string
DBFLIB_API int ParseExif(LPCTSTR src,int szsrc, LPCTSTR out, int szout, EXIFSTR & info){
	if (out)	memset((char *) out, 0, szout*sizeof(TCHAR));
	CString srcstr = src;
	CString outstr = _T("");
	CString tmp = src;
	CString tmp1 = _T("");
	CString tmp3 = _T("");
	
//	CString exif[] = &info.FileName; 

	int size = 0;
	int cnt = sizeof( exifconst)/sizeof(CString *);
	int cs, ce, dtp;
	for (int i =0; i<cnt; i++){
		cs = tmp.Find(exifconst[i]);
		info.exif[i].Empty();
		if (cs == -1){
			tmp = srcstr;
			cs = tmp.Find(exifconst[i]);
			if (cs == -1)	continue;
		}
		tmp1 = tmp.Mid(cs);
		ce = tmp1.Find(_T("\n"));
		tmp = tmp1;
		tmp1 = tmp.Left(ce);
		cs = tmp1.Find(_T(":"));
		tmp1= tmp1.Mid(cs+2,ce);
		info.exif[i] = tmp1.Trim();
		outstr += exifconst[i];
		outstr += _T(": ");
#if 0
		if (i==12){
			tmp1 = tmp1.Trim();
			dtp = tmp1.Find(_T(" "));
			tmp3 = tmp1.Mid(dtp+1);
			tmp1 = tmp1.Left(dtp+1);
			tmp1.Replace(_T(":"),_T("."));
			tmp1 += tmp3;
		}
#endif
		outstr += tmp1.TrimRight();
		outstr += _T("\r\n");


		
	}
	size = min(outstr.GetLength(),szout);
	if (size){
		try{
			if (out)	{
				memcpy((char *) out, outstr.GetBuffer(), size*sizeof(TCHAR));
				outstr.ReleaseBuffer();
			};
		}
		catch(...){
			size = 0;
		}
		outstr.ReleaseBuffer();

	}
	return size;
}
DBFLIB_API int InitLocalDB(LPCTSTR fpath){
	if (!m_db) m_db = new CPhotoDB();
	return m_db->Init(fpath);
}
DBFLIB_API void ReleaseLocalDB(){
	if (m_db) {
		if (m_db->m_imgset.IsOpen()) m_db->m_imgset.Close();
		delete m_db;
		m_db = 0;
	}
	AfxDaoTerm();
}

DBFLIB_API void ConvertHashToString(CString &out, unsigned char buf[16]){
	CString sym[]= {_T("0"),_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),
			_T("8"),_T("9"),_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F")
	};
	out = _T(""); int a,b;
	for (int i = 0; i <16; i++){
		a = b = buf[i];
		a = a>>4;
		out += sym[a];
		out += sym[(b&0x0F)];
	}//end main loop
}
DBFLIB_API int AddFileToDb(CString & fname, CString & minname, CString &exif){
	//fname - path to source file
	if (!m_db) return 0;
	int id = -2;
//	
	CString volBuf= GetComputerInfo(fname); //_T("");	
	CString disk=_T("");
	DWORD serNum =0; 
	DWORD fileSysFlag = 0;
	DWORD maxCompLen = 0;
	DWORD volSerNum = 0;
	CString FileSysNameBuf =_T("");
	DWORD nFileSysFlag =0;
	int ret= GetVolumeInformation(NULL/*fname.GetBuffer()*/, 
		volBuf.GetBuffer(256), 256,		&volSerNum, 
		&maxCompLen, 
		&nFileSysFlag, 
		FileSysNameBuf.GetBuffer(1024),1024);
	if (ret == 0) ret = GetLastError();
	fname.ReleaseBuffer();
	FileSysNameBuf.ReleaseBuffer();
	volBuf.ReleaseBuffer();
	disk.Format(_T("%X :: %s :: %s"), volSerNum, volBuf,FileSysNameBuf);
  
	
	COleDateTime dt1,dt2;
	EXIFSTR est; // bufer to save data
	CString str;
	unsigned char * hData;
	int stSize = exif.GetLength()*sizeof(TCHAR);
	GetFileHash(fname, est.Hash.chHash);
	ParseExif(exif, exif.GetLength(),0,0,est);
	ConvertHashToString(est.exif[49],&est.Hash.chHash[0]);
	if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
	if (m_db->m_imgset.IsOpen()){
		str.Format(_T("[HashStr]='%s'"),est.exif[49]);
		if (!m_db->m_imgset.FindFirst(str)){
			if (m_db->m_imgset.CanAppend()){
				m_db->m_imgset.AddNew();
			// create new record
				m_db->m_imgset.m_filename= est.exif[0];
				m_db->m_imgset.m_dir = est.exif[1];
				//		m_db->m_imgset.m_exif.m_hData = exif.GetBuffer();
				dt1 = GetFromExif(est.exif[2]);
				m_db->m_imgset.m_fcDate= dt1;//GetFromExif(est.exif[2]);
				m_db->m_imgset.m_fType = est.exif[3];
				m_db->m_imgset.m_width = _wtoi(est.exif[4]);
				m_db->m_imgset.m_height = _wtoi(est.exif[5]);
				m_db->m_imgset.m_make = est.exif[6];
				m_db->m_imgset.m_model = est.exif[7];
				m_db->m_imgset.m_exposureTime = est.exif[8];
				m_db->m_imgset.m_Fnumber = est.exif[9];
				m_db->m_imgset.m_exProg = est.exif[10];
				m_db->m_imgset.m_ISO = est.exif[11];
				dt2 =  GetFromExif(est.exif[12]);
				if (dt2 > COleDateTime(1800,1,1,0,0,1))
					m_db->m_imgset.m_dtOrigin = dt2;
				else 
					m_db->m_imgset.m_dtOrigin = dt1;
				m_db->m_imgset.m_Owner = est.exif[13];
				m_db->m_imgset.m_serial = est.exif[14];
				m_db->m_imgset.m_facesDetect = est.exif[15];
				m_db->m_imgset.m_LensInfo = est.exif[16];
				m_db->m_imgset.m_Lens = est.exif[17];
				m_db->m_imgset.m_LensModel = est.exif[18];
				m_db->m_imgset.m_LensSerial = est.exif[19];
				m_db->m_imgset.m_IntSerial = est.exif[20];
				m_db->m_imgset.m_Aperture = est.exif[21];
				m_db->m_imgset.m_BlueBalance = est.exif[22];
				m_db->m_imgset.m_RedBalance = est.exif[23];
				m_db->m_imgset.m_cOfC = est.exif[24];
				m_db->m_imgset.m_DOF = est.exif[25];
				m_db->m_imgset.m_Depth = est.exif[26];
				m_db->m_imgset.m_FOV = est.exif[27];
				m_db->m_imgset.m_fOfView = est.exif[28];
				m_db->m_imgset.m_hfDistance = est.exif[31];
				m_db->m_imgset.m_Lens35 = est.exif[30];
				m_db->m_imgset.m_LightValue = est.exif[33];
				m_db->m_imgset.m_Megapixels = est.exif[34];
				m_db->m_imgset.m_hash=est.exif[49];
				// add exif string
#if 0
				m_db->m_imgset.m_exif.m_hData= GlobalReAlloc(m_db->m_imgset.m_exif.m_hData,stSize,GMEM_ZEROINIT);
				hData = (unsigned char *) GlobalLock(m_db->m_imgset.m_exif.m_hData);
				if (hData){
					memcpy( hData, exif.GetBuffer(), stSize);
					exif.ReleaseBuffer();
					GlobalUnlock(m_db->m_imgset.m_exif.m_hData);
					m_db->m_imgset.m_exif.m_dwDataLength =stSize;
					m_db->m_imgset.SetFieldValue(_T("exif"), m_db->m_imgset.m_exif);
				}
				else{
					GlobalUnlock(m_db->m_imgset.m_exif.m_hData);
					m_db->m_imgset.m_exif.m_dwDataLength = 0;
				};
#endif
				m_db->m_imgset.SetFieldValue(_T("exif"), exif.GetBuffer());
				exif.ReleaseBuffer();
				HANDLE hFile = ::CreateFile(minname,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
				if (hFile != INVALID_HANDLE_VALUE){
					DWORD size = ::GetFileSize(hFile, 0);
					m_db->m_imgset.m_miniImg.m_hData =  GlobalReAlloc(m_db->m_imgset.m_miniImg.m_hData, size,GMEM_ZEROINIT);
					hData = (unsigned char *) GlobalLock(m_db->m_imgset.m_miniImg.m_hData);
					if (hData){
						ReadFile(hFile, hData, size, &size, 0);
						GlobalUnlock(m_db->m_imgset.m_miniImg.m_hData);
						m_db->m_imgset.m_miniImg.m_dwDataLength = size;
						m_db->m_imgset.SetFieldValue(_T("Miniimage"), m_db->m_imgset.m_miniImg);
						m_db->m_imgset.Update();
					}
					CloseHandle(hFile);
				}
				else{ // not add to db
					m_db->m_imgset.m_miniImg.m_dwDataLength =0;
				}
//				id = m_db->m_imgset.m_id;
			}//end can append
		}
		if (m_db->m_imgset.FindFirst(str)){
			id = m_db->m_imgset.m_id;
			AddToDublicate(id,  est.exif[0],  est.exif[1], disk);
		}
	}

	// minname - miniature path
	// exif - path to exif file
	// return 1 if file new and  add to db, n>0 if file new, but it copy exist in db n times <0 if file not add to db 0 file exist in db
	return id;
}
#define FULL_EXIF_TO_DB
DBFLIB_API int GetIdInfo(int id, CString & fname, CString & minname, CString &exif, CString &path){
	CString str;
	unsigned char * ptr = 0;
	DWORD dwWrite =0;
	CString tmp = _T("");
	int wb_size = 16000;
	TCHAR * wbuf= (TCHAR *) malloc(wb_size*sizeof(TCHAR));
	if (!wbuf) 
		return -1; // ERROR
	memset((unsigned char *) wbuf,0, wb_size*sizeof(TCHAR));
	if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
	if (m_db->m_imgset.IsOpen()){
		str.Format(_T("[Id]=%d"),id);
		if (m_db->m_imgset.FindFirst(str)){
			fname = m_db->m_imgset.m_filename;
			if (m_db->m_imgset.m_dir.Right(1).Compare(_T("/") ))
				path.Format(_T("%s/%s"),m_db->m_imgset.m_dir, m_db->m_imgset.m_filename);
			else
				path.Format(_T("%s%s"),m_db->m_imgset.m_dir, m_db->m_imgset.m_filename);
			//fname.Format(_T("%s\\%s"),m_db->m_imgset.m_dir,m_db->m_imgset.m_filename);
			ptr = (unsigned char *)::GlobalLock(m_db->m_imgset.m_miniImg.m_hData);
			if (ptr){
				HANDLE hFile = ::CreateFile(minname.GetBuffer(), GENERIC_READ|GENERIC_WRITE, 0, 0, CREATE_ALWAYS,0,0);
				minname.ReleaseBuffer();
				if (hFile != INVALID_HANDLE_VALUE){
					WriteFile(hFile, ptr, m_db->m_imgset.m_miniImg.m_dwDataLength, &dwWrite,0);
					CloseHandle(hFile);
				}
				::GlobalUnlock(m_db->m_imgset.m_miniImg.m_hData);
				ptr = 0;
			}
			exif = _T("");
#ifndef FULL_EXIF_TO_DB
			if (m_db->m_imgset.m_dtOrigin.GetStatus() != COleDateTime::invalid){
				exif.Format(_T("Path	: %s/%s\r\n"),m_db->m_imgset.m_dir,m_db->m_imgset.m_filename);
				//tmp.Format( _T("Created : %s\r\n"), m_db->m_imgset.m_dtOrigin.Format(_T("%#c")));
				tmp.Format( _T("Created : %s\r\n"), m_db->m_imgset.m_dtOrigin.Format(_T("%d/%m/%Y %H:%M:%S")));
				exif += tmp;
			}
			if (!m_db->m_imgset.m_make.IsEmpty()){
				tmp.Format( _T("Make   : %s\r\n"), m_db->m_imgset.m_make);
				exif += tmp;
			}
			if (!m_db->m_imgset.m_model.IsEmpty()){
				tmp.Format( _T("Model  : %s\r\n"), m_db->m_imgset.m_model);
				exif += tmp;
			}
			tmp.Format(     _T("Size   :  %d * %d\r\n"), m_db->m_imgset.m_width, m_db->m_imgset.m_height);
			exif += tmp;
			if (!m_db->m_imgset.m_Lens.IsEmpty()){
				tmp.Format( _T("Lens  : %s\r\n"), m_db->m_imgset.m_Lens);
				exif += tmp;
			}
			if (!m_db->m_imgset.m_exposureTime.IsEmpty()){
				tmp.Format( _T("Exposure  : %s\r\n"), m_db->m_imgset.m_exposureTime);
				exif += tmp;
			}
			if (!m_db->m_imgset.m_Aperture.IsEmpty()){
				tmp.Format( _T("Aprture  : %s\r\n"), m_db->m_imgset.m_Aperture);
				exif += tmp;
			}

#else

			if (m_db->m_imgset.m_exif.m_dwDataLength){
				if (wb_size< m_db->m_imgset.m_exif.m_dwDataLength){
					free(wbuf);
					wb_size = m_db->m_imgset.m_exif.m_dwDataLength+10;
					wbuf = (TCHAR *) malloc(wb_size*sizeof(TCHAR));
					memset((unsigned char *) wbuf,0, wb_size*sizeof(TCHAR));
				}
				//dwWrite = min(16000,m_db->m_imgset.m_exif.m_dwDataLength);
				if (wbuf){
					memcpy((unsigned char *)wbuf, GlobalLock(m_db->m_imgset.m_exif.m_hData), m_db->m_imgset.m_exif.m_dwDataLength);
					exif = wbuf;
				}
				else
					exif = (TCHAR *) GlobalLock(m_db->m_imgset.m_exif.m_hData);
				GlobalUnlock(m_db->m_imgset.m_exif.m_hData);
			}
			else{ 
				if (m_db->m_imgset.m_dtOrigin.GetStatus() != COleDateTime::invalid){
					exif.Format(_T("Path	: %s/%s\r\n"),m_db->m_imgset.m_dir,m_db->m_imgset.m_filename);
					//tmp.Format( _T("Created : %s\r\n"), m_db->m_imgset.m_dtOrigin.Format(_T("%#c")));
					tmp.Format( _T("Created : %s\r\n"), m_db->m_imgset.m_dtOrigin.Format(_T("%d/%m/%Y %H:%M:%S")));
					exif += tmp;
				}
			}
#endif
		}//end find
	}//end isOpen

	if (wbuf) free(wbuf);
	return 0;
}
DBFLIB_API int GetIdCount(int * id_list, int  maxcnt){
	int count = 0;
	if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
	if (m_db->m_imgset.IsOpen()){
		if(!(m_db->m_imgset.IsBOF()||m_db->m_imgset.IsEOF())){
			m_db->m_imgset.MoveFirst();
			while( (!m_db->m_imgset.IsBOF())&&(!m_db->m_imgset.IsEOF())){
				if (count<maxcnt) id_list[count] = m_db->m_imgset.m_id;
				count++;
				m_db->m_imgset.MoveNext();
			}
		}

//		m_db->m_imgset.Close();
	}
	
	return count;
}


CString GetComputerInfo(CString &path){
	CString ret = _T("");
	CString comp = _T("");
	CString volume = _T("");
	CString sFS = _T("");
	DWORD dwVol = 256;
	DWORD dwComp = 256;
	DWORD dwSerial = -1;
	DWORD dwFlag = 0;
	GetComputerName(comp.GetBuffer(256), &dwComp);
	comp.ReleaseBuffer();
//	ret = path.Left(path.Find('/'));
	GetVolumeInformation(path.Left(3),volume.GetBuffer(1024), 1024,&dwSerial,&dwVol,&dwFlag,sFS.GetBuffer(100),100);
	volume.ReleaseBuffer();
	sFS.ReleaseBuffer();
	ret.Format(_T("%X :: %s :: %s :: %X :: %s" ),dwSerial,volume, sFS, GetDriveType(path.Left(3)),  comp); 
	return ret;

}

bool AddToDublicate(int id, CString &fname, CString &dir, CString &info){
	CString str;
	str.Format(_T("[idMain]=%d AND [DiskInfo]=\"%s\" AND [FileName] = \"%s\" AND [Directory]=\"%s\" "),id, info, fname, dir);
	if (!m_db->m_dubset.IsOpen()) m_db->m_dubset.Open();
	if (m_db->m_dubset.IsOpen()){
		if (m_db->m_dubset.FindFirst(str)){
			//if ((fname.CompareNoCase(m_db->m_dubset.m_filename) == 0)&&(dir.CompareNoCase(m_db->m_dubset.m_dir) == 0))
			return true;
		}
		//append record
		if (m_db->m_dubset.CanAppend()){
			m_db->m_dubset.AddNew();
			m_db->m_dubset.m_idmain = id;
			m_db->m_dubset.m_filename = fname;
			m_db->m_dubset.m_dir = dir;
			m_db->m_dubset.m_diskinfo = info;
			m_db->m_dubset.Update();
			return true;
		}
		 
	}

	return false;

}

DBFLIB_API int GetDubId(int idMain, int * idBuf, int szBuf){
	int count = 0;
	CString str = _T("");
	if (!m_db->m_dubset.IsOpen()) m_db->m_dubset.Open();
	if (m_db->m_dubset.IsOpen()){
		str.Format(_T("[idMain]=%d"), idMain);
		if (m_db->m_dubset.FindFirst(str)){
			while(count<szBuf){
				idBuf[count++]= m_db->m_dubset.m_id;
//				m_db->m_dubset.MoveNext();
				if((!m_db->m_dubset.IsBOF())||(!m_db->m_dubset.IsEOF())){
					if (m_db->m_dubset.FindNext(str)){
						continue;
					}
					else
						break;
				}
				else
					break;
			}
		}
	}
	return count;
}
DBFLIB_API CString GetDubPath(int idDub, CString &diskinfo){
	CString path =_T("");
	diskinfo =_T("");
	CString str;
	if (!m_db->m_dubset.IsOpen()) m_db->m_dubset.Open();
	if (m_db->m_dubset.IsOpen()){
		str.Format(_T("[Id] = %d"), idDub);
		if (m_db->m_dubset.FindFirst(str)){
			if (m_db->m_dubset.m_dir.Right(1).Compare(_T("/") ))
				path.Format(_T("%s/%s"),m_db->m_dubset.m_dir, m_db->m_dubset.m_filename);
			else
				path.Format(_T("%s%s"),m_db->m_dubset.m_dir, m_db->m_dubset.m_filename);
			diskinfo = m_db->m_dubset.m_diskinfo;
		}
	}
	return path;



}

DBFLIB_API int AddFaceToDb(int idMain, CString & path){
	CString sHash = _T("");
	CString str = _T("");
	unsigned char hash[16]={};
	int sz = GetFileHash(path, hash);
	unsigned char * hData = 0;
	if (sz < 100)
		return -1;
	ConvertHashToString(sHash,hash);
	str.Format(_T("[imageId]=%d AND [sHash] =\"%s\""), idMain, sHash);
	if (!m_db->m_faceset.IsOpen()) m_db->m_faceset.IsOpen();
	if (m_db->m_faceset.IsOpen()){
		//if ((m_db->m_faceset.IsEOF() && m_db->m_faceset.IsBOF()))
		if (!m_db->m_faceset.FindFirst(str)){
			if (m_db->m_faceset.CanAppend()){
				m_db->m_faceset.AddNew();

				m_db->m_faceset.m_idmain = idMain;
				m_db->m_faceset.m_hash = sHash;

				HANDLE hFile = ::CreateFile(path,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
				if (hFile != INVALID_HANDLE_VALUE){
					DWORD size = ::GetFileSize(hFile, 0);
					m_db->m_faceset.m_image.m_hData =  GlobalReAlloc(m_db->m_faceset.m_image.m_hData, size,GMEM_ZEROINIT);
					hData = (unsigned char *) GlobalLock(m_db->m_faceset.m_image.m_hData);
					if (hData){
						ReadFile(hFile, hData, size, &size, 0);
						GlobalUnlock(m_db->m_faceset.m_image.m_hData);
						m_db->m_faceset.m_image.m_dwDataLength = size;
						m_db->m_faceset.SetFieldValue(_T("image"), m_db->m_faceset.m_image);
						m_db->m_faceset.Update();
					}
					CloseHandle(hFile);
				}
				else{ // not add to db
					m_db->m_faceset.m_image.m_dwDataLength =0;
				}
//				m_db->m_faceset.Update();
			}
		}
	}
	return 0;
}//end AddFileToDb

// face info
// get list id to faces with idMain
DBFLIB_API int GetFaceIdInfo(int idMain, int * idBuf, int szBuf, CString & fname){
	int count = 0;
	fname =_T("");
	CString str;
	if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
	if (m_db->m_imgset.IsOpen()){
		str.Format(_T("[Id]=%d"), idMain);
		if (m_db->m_imgset.FindFirst(str)){
			fname = m_db->m_imgset.m_filename;
		}
	}
	if (!m_db->m_faceset.IsOpen()) m_db->m_faceset.Open();
	if (m_db->m_faceset.IsOpen()){
		str.Format(_T("[imageId]=%d"), idMain);
		if(!(m_db->m_faceset.IsBOF()&&m_db->m_faceset.IsEOF())){
			m_db->m_faceset.MoveFirst();
			if (m_db->m_faceset.FindFirst(str)){
				while(count <szBuf){
					idBuf[count++] = m_db->m_faceset.m_id;
	//				m_db->m_faceset.MoveNext();
					if (!m_db->m_faceset.FindNext(str)){
						break;
					}
				}
			}//end find
		}
	}
	return count; // return number of idfaces into idBuf
}//end GetFaceIdInfo

// get list id to faces with idMain
DBFLIB_API int GetFaceIdImage(int id, CString & minimage){
	int sz = 0;
	CString str;
	unsigned char * ptr = 0;
	if (!m_db->m_faceset.IsOpen()) m_db->m_faceset.Open();
	if (m_db->m_faceset.IsOpen()){
		str.Format(_T("[Idf] = %d"), id);
		if (m_db->m_faceset.FindFirst(str)){
			ptr = (unsigned char *)::GlobalLock(m_db->m_faceset.m_image.m_hData);
			if (ptr){
				HANDLE hFile = ::CreateFile(minimage.GetBuffer(), GENERIC_READ|GENERIC_WRITE, 0, 0, CREATE_ALWAYS,0,0);
				minimage.ReleaseBuffer();
				if (hFile != INVALID_HANDLE_VALUE){
					WriteFile(hFile, ptr, m_db->m_faceset.m_image.m_dwDataLength,(DWORD *) &sz,0);
					CloseHandle(hFile);
				}
				::GlobalUnlock(m_db->m_faceset.m_image.m_hData);
				ptr = 0;
			}
		}
	}
	return sz; // return sizeof image file
}//end GetFaceIdImage


DBFLIB_API CString GetAlternatePath(int id_img, CString idPath ){
	CString path = idPath;
//	CString path = _T("");
	CString str = _T("");
	if (path.IsEmpty()){
		if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
		str.Format(_T("[Id]=%d"), id_img);
		if (m_db->m_imgset.FindFirst(str)){
			if (m_db->m_imgset.m_dir.Right(1).Compare(_T("/") ))
				path.Format(_T("%s/%s"),m_db->m_imgset.m_dir, m_db->m_imgset.m_filename);
			else
				path.Format(_T("%s%s"),m_db->m_imgset.m_dir, m_db->m_imgset.m_filename);
		}	
		if (path.IsEmpty()){
			return _T(""); //id_img not found
		}
	}

	HANDLE hFile = CreateFile(path,FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (hFile == INVALID_HANDLE_VALUE){
		CString newpath = _T("");
		CString disk = _T("");
		int idBuf[100] ={};
		int count = GetDubId(id_img,idBuf, 100);
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
					break;
				}
				else path = _T("");
			}
				
		}
	}
	else{
		CloseHandle(hFile);
	}


	return path;// GetAlternatePath
}

DBFLIB_API void DeleteFaceImg(int id){
	if (!m_db) return;
	if (!m_db->m_faceset.IsOpen()) m_db->m_faceset.Open();
	if (m_db->m_faceset.IsOpen()){
		CString str;
		str.Format(_T("[Idf] = %d"), id);
		if (m_db->m_faceset.FindFirst(str)){
			if (!m_db->m_faceset.IsDeleted()){
				m_db->m_faceset.Delete();
			}

		}
	}

}
DBFLIB_API void DeleteDubImg(int id){
//delete image reference from database
	if (!m_db) return;
	if (!m_db->m_dubset.IsOpen()) m_db->m_dubset.Open();
	if (m_db->m_dubset.IsOpen()){
		CString str;
		str.Format(_T("[Id] = %d"), id);
		if (m_db->m_dubset.FindFirst(str)){
			if (!m_db->m_dubset.IsDeleted()){
				m_db->m_dubset.Delete();
			}
		}
	}


};//

DBFLIB_API void DeleteImg(int id){
//delete image from database
	if (!m_db) return;

	CString str =_T("");
	int idBuf[100]={};
	int sz = 100;
	// deleting faces
	while(sz > 0){
		sz = GetFaceIdInfo(id, idBuf,100, str);
		for(int i = 0; i<sz; i++){
			DeleteFaceImg(idBuf[i]);
			idBuf[i] = 0;
		}
	}//end deleting faces
	sz  = 100;
	//deleting dublicates
	while(sz >0){
		sz = GetDubId(id, idBuf,100);
		for(int i=0; i<sz; i++){
			DeleteDubImg(idBuf[i]);
			idBuf[i] = 0;
		}
	}
	if (!m_db->m_imgset.IsOpen()) m_db->m_imgset.Open();
	if (m_db->m_imgset.IsOpen()){
		CString str;
		str.Format(_T("[Id] = %d"), id);
		if (m_db->m_imgset.FindFirst(str)){
			if (!m_db->m_imgset.IsDeleted()){
				m_db->m_imgset.Delete();
			}
		}
	}

};
