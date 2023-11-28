// ImportToSl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "ImportToSl.h"
#include "slite.h"
#include <time.h>




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if defined _M_IX86
//#include "../dbflib/dbflib.h"
#include <afxdao.h>
#include "../dbflib/PhotoDB.h"
#include "../dbflib/PhotoDB.cpp"
//#ifdef DEBUG
//#pragma comment(lib, "..\\Debug\\dbflib.lib")
//#else
//#pragma comment(lib, "..\\Release\\dbflib.lib")
//#endif // DEBUG
#endif

// The one and only application object

//CWinApp theApp;

//using namespace std;

std::string GetTimeFromExif(CString dtstr) {
    CString str = dtstr;
    int nYear = 1111;
    int nMonth = 11;
    int nDay = 11;
    int nHour = 11;
    int nMin = 11;
    int nSec = 11;
    int start = 0;
    int end = 0;
    COleDateTime dt;
    if (dtstr.GetLength() > 4) {
        nYear = _wtoi(dtstr);
        start = 4;// dtstr.Find(':');
        str = dtstr.Mid(start + 1);
        if (str.GetLength() > 1) {
            nMonth = _wtoi(str);
            start = 2;//str.Find(':');
            str = str.Mid(start + 1);
            if (str.GetLength() > 1) {
                nDay = _wtoi(str);
                start = 2;//str.Find(' ');
                str = str.Mid(start + 1);
                if (str.GetLength() > 1) {
                    nHour = _wtoi(str);
                    start = 2;//str.Find(':');
                    str = str.Mid(start + 1);
                    if (str.GetLength() > 1) {
                        nMin = _wtoi(str);
                        start = 2;//str.Find(':');
                        str = str.Mid(start + 1);
                        if (str.GetLength() > 1)	nSec = _wtoi(str);
                    }//end check min
                }//end check hour
            }//end check day
        }//end check Month
    }//end check Year
    dt = COleDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
    if (dt.GetStatus()) return "0";

    return std::to_string(ConvertFromMSTime(dt));
}



void FillExifStruct(SLEXIFSTR& info, CImgSet &aset) {
    info.exif[0] = ConvertToUTF8(aset.m_filename);
    info.exif[1] = ConvertToUTF8(aset.m_dir);
    info.exif[2] = std::to_string(ConvertFromMSTime(aset.m_fcDate)); // convert to int64
    info.exif[3] = std::to_string(aset.m_width);
    info.exif[4] = std::to_string(aset.m_height);
    info.exif[5] = ConvertToUTF8(aset.m_fType);
    info.exif[6] = ConvertToUTF8(aset.m_make);
    info.exif[7] = ConvertToUTF8(aset.m_model);
    info.exif[8] = ConvertToUTF8(aset.m_exposureTime);
    info.exif[9] = ConvertToUTF8(aset.m_Fnumber);
    info.exif[10] = ConvertToUTF8(aset.m_exProg);
    info.exif[11] = ConvertToUTF8(aset.m_ISO);
    info.exif[12] = std::to_string(ConvertFromMSTime(aset.m_dtOrigin));
    info.exif[13] = ConvertToUTF8(aset.m_Owner);
    info.exif[14] = ConvertToUTF8(aset.m_serial);
    info.exif[15] = ConvertToUTF8(aset.m_facesDetect);
    info.exif[16] = ConvertToUTF8(aset.m_LensInfo);
    info.exif[17] = ConvertToUTF8(aset.m_Lens);
    info.exif[18] = ConvertToUTF8(aset.m_LensModel);
    info.exif[19] = ConvertToUTF8(aset.m_LensSerial);
    info.exif[20] = ConvertToUTF8(aset.m_IntSerial);
    info.exif[21] = ConvertToUTF8(aset.m_Aperture);
    info.exif[22] = ConvertToUTF8(aset.m_BlueBalance);
    info.exif[23] = ConvertToUTF8(aset.m_RedBalance);
    info.exif[24] = ConvertToUTF8(aset.m_cOfC);
    info.exif[25] = ConvertToUTF8(aset.m_DOF);
    info.exif[26] = ConvertToUTF8(aset.m_Depth);
    info.exif[27] = ConvertToUTF8(aset.m_FOV);
    info.exif[28] = ConvertToUTF8(aset.m_fOfView);
    info.exif[29] = "";
    info.exif[30] = "";
    info.exif[31] = ConvertToUTF8(aset.m_hfDistance);
    info.exif[32] = ConvertToUTF8(aset.m_Lens35);
    info.exif[33] = ConvertToUTF8(aset.m_LightValue);
    info.exif[34] = ConvertToUTF8(aset.m_Megapixels);
    LPCTSTR ptr  = (LPCTSTR )::GlobalLock(aset.m_exif.m_hData);
    if (ptr != nullptr) {
        CString stmp = ptr;
        info.exif[35] = ConvertToUTF8(stmp);
        ::GlobalUnlock(aset.m_exif.m_hData);
    }
    else {
        info.exif[35] = "";
    }
    info.exif[49] = ConvertToUTF8(aset.m_hash);
    info.m_ptr = (unsigned char*)::GlobalLock(aset.m_miniImg.m_hData);
    info.m_ptrLength = (info.m_ptr != nullptr) ? aset.m_miniImg.m_dwDataLength : 0;
};

int ParseExifSL(LPCTSTR src, int szsrc, LPCTSTR out, int szout, SLEXIFSTR& info)
{
    std::vector<CString> exifconst = { _T("FileName   "), _T("Directory "), _T("FileCreateDate "), /* 0, 1, 2 */
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
    if (out)	memset((char*)out, 0, szout * sizeof(TCHAR));
    CString srcstr = src;
    CString outstr = _T("");
    CString tmp = src;
    CString tmp1 = _T("");
    CString tmp3 = _T("");


    int size = 0;
    int cnt = exifconst.size();
    int cs, ce, dtp;
    for (int i = 0; i < cnt; i++) {
        cs = tmp.Find(exifconst[i]);
        info.exif[i] = "";
        if (cs == -1) {
            tmp = srcstr;
            cs = tmp.Find(exifconst[i]);
            if (cs == -1)	continue;
        }
        tmp1 = tmp.Mid(cs);
        ce = tmp1.Find(_T("\n"));
        tmp = tmp1;
        tmp1 = tmp.Left(ce);
        cs = tmp1.Find(_T(":"));
        tmp1 = tmp1.Mid(cs + 2, ce);
        if (i != 2 && i != 12) {
            info.exif[i] = ConvertToUTF8(tmp1.Trim());
        }
        else {
            info.exif[i] = GetTimeFromExif(tmp1.Trim());
        }
        outstr += exifconst[i];
        outstr += _T(": ");
        outstr += tmp1.TrimRight();
        outstr += _T("\r\n");
    }
    size = min(outstr.GetLength(), szout);
    if (size) {
        try {
            if (out) {
                memcpy((char*)out, outstr.GetBuffer(), size * sizeof(TCHAR));
                outstr.ReleaseBuffer();
            };
        }
        catch (...) {
            size = 0;
        }
        outstr.ReleaseBuffer();

    }
    // create date int64_t
    if (info.exif[12].empty()) {
        info.exif[12] = "0";
    }
    if (info.exif[2].empty()) {
        info.exif[2] = "0";
    }

    return size;
  
}
int AddToFaceRef(SLRecordset<FACESET> fset, CFaceSet& aset, int idmain = 0) {
    std::string sql = "INSERT INTO FACESET (IDMAIN, HASH, IMAGE) VALUES (  ?, ?, ?) ";
    int res = fset.Open(sql);
    std::string tstr;
    if (res == SQLITE_OK) {
        res = (idmain != 0) ? sqlite3_bind_int(fset.GetSmpt(), 1, idmain) : sqlite3_bind_int(fset.GetSmpt(), 1, aset.m_idmain);
        tstr = ConvertToUTF8(aset.m_hash);
        res = sqlite3_bind_text(fset.GetSmpt(), 2, tstr.c_str(), tstr.length(), 0);
        unsigned char * ptr = (unsigned char*)::GlobalLock(aset.m_image.m_hData);
        if (ptr != nullptr) {
            res = sqlite3_bind_blob(fset.GetSmpt(), 3, ptr, aset.m_image.m_dwDataLength, SQLITE_TRANSIENT);
            ::GlobalUnlock(aset.m_image.m_hData);
            ptr = nullptr;
        }
        else
            res = sqlite3_bind_null(fset.GetSmpt(), 3);
        sqlite3_step(fset.GetSmpt());
        return fset.Close();
    }
    return res;
}

int AddToImgRef(SLRecordset<IMGREF>& rset, CImgrefSet& aset, int idmain = 0) {
    std::string sql = "INSERT INTO IMGREF (IDMAIN, FNAME, DIR, COMP, DISKINFO) VALUES ( ?, ?, ?, ?, ?) ";
    int res = -1;
    res = rset.Open(sql);
    //std::string tstr ="";
    if (res == SQLITE_OK) {
        res = (idmain != 0) ? sqlite3_bind_int(rset.GetSmpt(), 1, idmain) : sqlite3_bind_int(rset.GetSmpt(), 1, aset.m_idmain);
        rset.m_set.m_filename = ConvertToUTF8(aset.m_filename);
        res = rset.m_set.m_filename.empty() ? sqlite3_bind_null(rset.GetSmpt(), 2) : sqlite3_bind_text(rset.GetSmpt(), 2, rset.m_set.m_filename.c_str(), rset.m_set.m_filename.length(), NULL);
        rset.m_set.m_dir = ConvertToUTF8(aset.m_dir);
        res = rset.m_set.m_dir.empty() ? sqlite3_bind_null(rset.GetSmpt(), 3) : sqlite3_bind_text(rset.GetSmpt(), 3, rset.m_set.m_dir.c_str(), rset.m_set.m_dir.length(), 0);
        rset.m_set.m_computer = ConvertToUTF8(aset.m_comp);
        res = rset.m_set.m_computer.empty()  ?  sqlite3_bind_null(rset.GetSmpt(), 4) : sqlite3_bind_text(rset.GetSmpt(), 4, rset.m_set.m_computer.c_str(), rset.m_set.m_computer.length(), 0);
        rset.m_set.m_diskinfo = ConvertToUTF8(aset.m_diskinfo);
        res = (rset.m_set.m_diskinfo.empty()) ? sqlite3_bind_null(rset.GetSmpt(), 5) : sqlite3_bind_text(rset.GetSmpt(), 5, rset.m_set.m_diskinfo.c_str(), rset.m_set.m_diskinfo.length(), 0);
        sqlite3_step(rset.GetSmpt());
        res = rset.Close();
    }
    return res;
}


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"Fatal Error: MFC initialization failed\n");
            return 1;
        }
    }
    else
    {
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        return 1;
    }

    SLDatabaes sl_db;
    CString workPath;
    bool ret = SHGetSpecialFolderPath(NULL, workPath.GetBuffer(MAX_PATH), CSIDL_LOCAL_APPDATA, 0);
    workPath.ReleaseBuffer();
    if (ret) {
        workPath += _T("\\VYVphoto");
        ::CreateDirectory(workPath, 0);
    }
    workPath.Replace(_T("\\"), _T("/"));
    if (sl_db.Open("./test.db3") != SQLITE_OK) {
        std::cout << "\nError create sql database\n";
    };

#if defined _M_IX86
    AfxDaoInit();
    CPhotoDB  a_db;
    if (a_db.Init(workPath) != 1) return 1;
    int count = 0;
    int res = 0;
    if (!a_db.m_imgset.IsOpen()) a_db.m_imgset.Open();
    SLRecordset< IMGSET> imgset(sl_db.GetDb());
    SLRecordset<FACESET> faceset(sl_db.GetDb());
    SLRecordset<IMGREF> imgref(sl_db.GetDb());
 /*   
    std::string sq_imgset = "INSERT INTO IMGSET (ID, DTORIGIN, DTFCREATE, WIDTH, HEIGHT, "
        " FILENAME, FDIR, TYPE, MAKE, MODEL, EXPTIME, FNUMBER, EXPOSPROG, ISO, OWNER, SERIAL, "
        "FASEDETECT, LENSINFO, LENS, LENSMODEL, LENSSERIAL, INTSERIAL, "
        "APERTURE, BLUEBALANCE, COFC, DOF, DEPTH, FOFVIEW, HFDISTANCE, LENS35, LIGHTVALUE, MEGAPIXELS, "
        "HASH, MINIIMG, EXIF ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        */
  //  std::vector<std::string> imgFileds = { "MAKE", "MODEL", "EXPTIME", "FNUMBER", "EXPOSPROG", "ISO",
  //      "OWNER", "SERIAL", "FASEDETECT", "LENSINFO", "LENS", "LENSMODEL", "LENSSERIAL", "INTSERIAL", 
  //      "APERTURE", "BLUEBALANCE", "COFC", "DOF", "DEPTH", "FOFVIEW", "HFDISTANCE", "LENS35",
  //      "LIGHTVALUE", "MEGAPIXELS", "HASH", "EXIF"   };



  //  std::string sq_imgset = "INSERT INTO IMGSET (ID, DTORIGIN, DTFCREATE, WIDTH, HEIGHT, "
  //      " FILENAME, FDIR, TYPE, TYPE, HASH, MINIIMG) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    SLEXIFSTR info;
    if (a_db.m_imgset.IsOpen()) {
        if (!(a_db.m_imgset.IsBOF() || a_db.m_imgset.IsEOF())) {
            std::string tstr = "";
            //imgset.Open(sq_imgset);
            unsigned char* ptr = nullptr;
            a_db.m_imgset.MoveFirst();
            int64_t ind = 0L, ind1 = 0L;
            while ((!a_db.m_imgset.IsBOF()) && (!a_db.m_imgset.IsEOF())) {
                count++;
                FillExifStruct(info, a_db.m_imgset);
                res = BindImgSet(imgset, info, tstr);
                if (info.m_ptr) ::GlobalUnlock(a_db.m_imgset.m_miniImg.m_hData);
                if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                else { //added
                    res = imgset.Open("SELECT ID,max(ID) FROM IMGSET ");// "SELECT * FROM IMGSET ORDER BY ID DESC");
                    if (res == SQLITE_OK) {
                       // sqlite3_reset(imgset.GetSmpt());
                        if (imgset.Next() == SQLITE_ROW){
//                            int id = sqlite3_column_count(imgset.GetSmpt());
//                            id = sqlite3_data_count(imgset.GetSmpt());
//                            id = sqlite3_column_type(imgset.GetSmpt(), 1);
                            int id = sqlite3_column_int(imgset.GetSmpt(), 1);
                            ;
                            CString s_sql;
                            s_sql.Format(_T("[idMain]=%d"), a_db.m_imgset.m_id);
                            if (!a_db.m_dubset.IsOpen()) a_db.m_dubset.Open();
                            if (a_db.m_dubset.IsOpen()) {
                                if (a_db.m_dubset.FindFirst(s_sql)) {
                                    while (!(a_db.m_dubset.IsBOF()) || (!a_db.m_dubset.IsEOF())) {
                                        AddToImgRef(imgref, a_db.m_dubset, id);
                                        if (!a_db.m_dubset.FindNext(s_sql)) break;
                                    }
                                }
                                a_db.m_dubset.Close();
                            }//end add ref
                            if (!a_db.m_faceset.IsOpen()) a_db.m_faceset.Open();
                            if (a_db.m_faceset.IsOpen()) {
                                s_sql.Format(_T("[imageId]=%d"), a_db.m_imgset.m_id);
                                if (a_db.m_faceset.FindFirst(s_sql)) {
                                    while (!(a_db.m_faceset.IsBOF()) || (!a_db.m_faceset.IsEOF())) {
                                        AddToFaceRef(faceset, a_db.m_faceset, id);
                                        if (!a_db.m_faceset.FindNext(s_sql)) break;
                                    }
                                }
                                a_db.m_faceset.Close();
                            }//end add ref

                            
                        }
                    }
                    imgset.Close();
                }
                std::cout <<  ".";
                
                a_db.m_imgset.MoveNext();

#if 0
                // insert record
                if (imgset.Open(sq_imgset) == SQLITE_OK) {
                    res = sqlite3_bind_int(imgset.GetSmpt(), 1, a_db.m_imgset.m_id);
                    if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    ind = ConvertFromMSTime(a_db.m_imgset.m_dtOrigin);
                    res = sqlite3_bind_int64(imgset.GetSmpt(), 2, ind);
                    if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    ind1 = ConvertFromMSTime(a_db.m_imgset.m_fcDate);
                    res = sqlite3_bind_int64(imgset.GetSmpt(), 3, ind1);
                    if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    res = sqlite3_bind_int(imgset.GetSmpt(), 4, a_db.m_imgset.m_width);
                    if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    res = sqlite3_bind_int(imgset.GetSmpt(), 5, a_db.m_imgset.m_height);
                    tstr = ConvertToUTF8(a_db.m_imgset.m_filename);
                    res = sqlite3_bind_text(imgset.GetSmpt(), 6, tstr.c_str(), tstr.length(), NULL);
                    if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    ptr = (unsigned char*)::GlobalLock(a_db.m_imgset.m_miniImg.m_hData);
                    if (ptr != nullptr) {
                       // ind = sqlite3_bind_parameter_index(imgset.GetSmpt(), iNames[6].c_str());
                        res = sqlite3_bind_blob(imgset.GetSmpt(), 7, ptr, a_db.m_imgset.m_miniImg.m_dwDataLength, SQLITE_TRANSIENT);
                        ::GlobalUnlock(a_db.m_imgset.m_miniImg.m_hData);
                        ptr = nullptr;
                        if (res != SQLITE_OK) { tstr = sqlite3_errmsg(sl_db.GetDb()); }
                    }
                    a_db.m_imgset.MoveNext();

                    sqlite3_step(imgset.GetSmpt());
                }
#endif
            }
        }
        imgset.Close();
    }
    std::cout << "\n";
    if (a_db.m_imgset.IsOpen()) a_db.m_imgset.Close();
    if (a_db.m_dubset.IsOpen()) a_db.m_dubset.Close();
    if (a_db.m_faceset.IsOpen()) a_db.m_faceset.Close();
    if (a_db.IsOpen()) a_db.Close();
    AfxDaoTerm();
#endif

 
    sl_db.Close();
    return nRetCode;
}
