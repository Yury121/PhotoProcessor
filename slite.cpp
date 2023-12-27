#include "stdafx.h"
#include "slite.h"
#include "md5.hpp"


static 	 std::shared_ptr<SLDatabaes> m_db;



CString FindDiskSerialSL(CString sSerial) {
    CString sDisk = _T("");
    DWORD dwVol = 0;
    TCHAR buf[256] = {};
    CString str;
    DWORD dwCount = GetLogicalDriveStrings(256, buf);
    for (int i = 0; i < dwCount - 1;) {
        sDisk = &buf[i];
        i += sDisk.GetLength() + 1;
        ::GetVolumeInformation(sDisk, NULL, 0, &dwVol, 0, 0, 0, 0);
        str.Format(_T("%X"), dwVol);
        if (sSerial.Find(str) >= 0) {
            break;
        }
        sDisk = _T("");
    }
    return sDisk;
}

int InitLocalDBSL(LPCTSTR fpath) {

    if (m_db.get() == nullptr) {
        m_db = std::make_shared<SLDatabaes>();
    }
    if (m_db.get() != nullptr) {
        if (m_db->GetDb() != nullptr)
            return m_db->Open(ConvertToUTF8(fpath));
    }
    else return -1;
    CString cpath = fpath;
    cpath += _T("/vb.db3");
    if (::PathFileExists(cpath)) {
        DWORD dwa = GetFileAttributes(cpath);
        SetFileAttributes(cpath, dwa|FILE_ATTRIBUTE_HIDDEN);
    }
    else {
        m_db->Init(ConvertToUTF8(fpath));
        m_db->Close();
        DWORD dwa = GetFileAttributes(cpath);
        SetFileAttributes(cpath, dwa | FILE_ATTRIBUTE_HIDDEN);
        return m_db->Open(ConvertToUTF8(fpath));
    }
    if (m_db->Init(ConvertToUTF8(fpath)) == SQLITE_OK) {
        if (!m_db->CheckColumnExist("COSIN")) {
            char* cerr = nullptr;
            int res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN X INTEGER", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN Y INTEGER", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN WIDTH INTEGER", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN HEIGHT INTEGER", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN NORM REAL", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN COSIN BLOB", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);

        }
        if (!m_db->CheckColumnExist("AGE")) {
            char* cerr = nullptr;
            int res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN AGE REAL", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN MALE REAL", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);
            res = sqlite3_exec(m_db->GetDb(), "ALTER TABLE FACESET ADD COLUMN FEMALE REAL", 0, 0, &cerr);
            if (res) sqlite3_free(cerr);

        }
        return sqlite3_exec(m_db->GetDb(), "VACUUM", 0, 0, 0);
    }
    return m_db->Open(ConvertToUTF8(fpath));
}
void ReleaseLocalDBSL() {
    if (m_db.get() != nullptr) {
        m_db->Close();
    }
	m_db.reset();
}

int GetIdCountSL(int* id_list, int  maxcnt) {
    SLRecordset< IMGSET> imgset(m_db->GetDb());
    int count = 0;
    memset((uint8_t*)id_list, 0, sizeof(int) * maxcnt);
    int res = imgset.Open("SELECT ID FROM IMGSET ORDER BY FDIR, FILENAME, TYPE ");// "SELECT * FROM IMGSET ORDER BY ID DESC");
    if (res == SQLITE_OK) {
        while (imgset.Next() == SQLITE_ROW && count < maxcnt) {
            id_list[count++] = sqlite3_column_int(imgset.GetSmpt(), 0);
            
        }
    }
    imgset.Close();
	return count;
}
CString GetAlternatePathSL(int id_img, CString idPath) {
    std::string spath = ConvertToUTF8(idPath);
    std::string str = "SELECT FILENAME, FDIR FROM IMGSET WHERE ID = '" + std::to_string(id_img) + "'";
    SLRecordset< IMGSET> imgset(m_db->GetDb());
    if (spath.empty()) {
        int res = imgset.Open(str);
        if (res == SQLITE_OK) {
            if (imgset.Next() == SQLITE_ROW) {
                if (sqlite3_data_count > 0) {
                    imgset.m_set.m_filename = (char*)sqlite3_column_text(imgset.GetSmpt(), 0);
                    imgset.m_set.m_dir = (char*)sqlite3_column_text(imgset.GetSmpt(), 1);
                    if (!imgset.m_set.m_dir.empty()) {
                        if (*imgset.m_set.m_dir.rbegin() == '/') {
                            spath = imgset.m_set.m_dir + imgset.m_set.m_filename;
                        }
                        else {
                            spath = imgset.m_set.m_dir + "/" + imgset.m_set.m_filename;
                        }
                    }
                }
            }
        }
    }        
    if (spath.empty()) {
        imgset.Close();
        return _T("");
    }
    CString path = ConvertFromUTF8(spath);
    HANDLE hFile = CreateFile(path, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        CString newpath = _T("");
        CString disk = _T("");
        path = _T("");
        int idBuf[100] = {};
        int count = GetDubIdSL(id_img, idBuf, 100);
        for (int i = 0; i < count; i++) {
            path = _T("");
            newpath = GetDubPathSL(idBuf[i], disk);
            disk = FindDiskSerialSL(disk);
            if (disk.IsEmpty()) continue;
            else {
                path.Format(_T("%s%s"), disk.Left(2), newpath.Mid(2));
                hFile = CreateFile(path, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
                if (hFile != INVALID_HANDLE_VALUE) {
                    CloseHandle(hFile);
                    break;
                }
                else path = _T("");
            }
        }
    }
    else {
        CloseHandle(hFile);
    }
    imgset.Close();
    return path;// GetAlternatePath
}

int GetDubIdSL(int idMain, int* idBuf, int szBuf)
{
    int count = 0;
    SLRecordset<IMGREF> imgref(m_db->GetDb());
    //"SELECT ID, FNAME, DIR, DISKINFO  FROM IMGREF WHERE IDMAIN = '37'"
    std::string sql = "SELECT ID FROM IMGREF WHERE IDMAIN = '" + std::to_string(idMain) + "'";
    int res = imgref.Open(sql);
    memset(idBuf, 0, sizeof(int) * szBuf);
    if (res == SQLITE_OK) {
        while (imgref.Next() == SQLITE_ROW && count < szBuf) {
            idBuf[count++] = sqlite3_column_int(imgref.GetSmpt(), 0);
        }
    }
   imgref.Close();
    return count;
}

CString GetDubPathSL(int idDub, CString& diskinfo) {
    CString path = _T("");
    std::string spath = "";
    SLRecordset<IMGREF> imgref(m_db->GetDb());
    diskinfo = _T("");
    //"SELECT ID, FNAME, DIR, DISKINFO  FROM IMGREF WHERE IDMAIN = '37'"
    std::string sql = "SELECT FNAME, DIR, DISKINFO FROM IMGREF WHERE ID = '" + std::to_string(idDub) + "'";
    int res = imgref.Open(sql);
    if (res == SQLITE_OK) {
        if (imgref.Next() == SQLITE_ROW) {
            if (sqlite3_data_count > 0) {
                imgref.m_set.m_filename = (char*)sqlite3_column_text(imgref.GetSmpt(), 0);
                imgref.m_set.m_dir = (char*)sqlite3_column_text(imgref.GetSmpt(), 1);
                imgref.m_set.m_diskinfo = (char*)sqlite3_column_text(imgref.GetSmpt(), 2);
                if (!imgref.m_set.m_dir.empty()) {
                    if (*imgref.m_set.m_dir.rbegin() == '/') {
                        spath = imgref.m_set.m_dir + imgref.m_set.m_filename;
                    }
                    else {
                        spath = imgref.m_set.m_dir + "/" + imgref.m_set.m_filename;
                    }
                }
                diskinfo = ConvertFromUTF8(imgref.m_set.m_diskinfo);
                path = ConvertFromUTF8(spath);
            }
        }
    }
    imgref.Close();
    return path;

}
//#ifdef _M_X64
int ParseExifSTR(LPCTSTR src, int szsrc, LPCTSTR out, int szout, EXIFSTR& info)
{
    SLEXIFSTR slinfo;
    int res = ParseExifSL(src, szsrc, out, szout, slinfo);
    for (int i = 0; i < 50; i++) {
        info.exif[i] = (slinfo.exif[i].empty()) ? _T("") : ConvertFromUTF8(slinfo.exif[i]);
    }
    memcpy(info.Hash.chHash, slinfo.Hash.chHash, 16);

    return 0;
}
//#endif

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

int GetFaceIdInfoSL(int idMain, int* idBuf, int szBuf, CString& fname)
{
    fname = _T("");
    int count = 0;
    SLRecordset<IMGSET>  imgset(m_db->GetDb());
    SLRecordset<FACESET> fset(m_db->GetDb());
    std:: string sql = "SELECT FILENAME FROM IMGSET WHERE ID = '" + std::to_string(idMain) + "'";
    int res = imgset.Open(sql);
    if (res == SQLITE_OK) {
        if (imgset.Next() == SQLITE_ROW) {
            imgset.m_set.m_filename = (char*) sqlite3_column_text(imgset.GetSmpt(), 0);
            fname = ConvertFromUTF8(imgset.m_set.m_filename);
        }
    }
    imgset.Close();

    sql = "SELECT ID FROM FACESET WHERE IDMAIN = '" + std::to_string(idMain) + "'";
    res = fset.Open(sql);
    if (res == SQLITE_OK) {
        while (fset.Next() == SQLITE_ROW && count < szBuf) {
            idBuf[count++] = sqlite3_column_int(fset.GetSmpt(), 0);
        }
    }
    fset.Close();

    return count;
}

int GetFaceIdImageSL(int id, CString& minimage)
{
    int sz = 0;
    SLRecordset<FACESET> fset(m_db->GetDb());
    std::string sql = "SELECT IMAGE FROM FACESET WHERE ID = '" + std::to_string(id) + "'";
    if (fset.Open(sql) == SQLITE_OK) {
        if (fset.Next() == SQLITE_ROW) {
            uint8_t * ptr =(uint8_t*) sqlite3_column_blob(fset.GetSmpt(), 0);
            DWORD blob_bytes = sqlite3_column_bytes(fset.GetSmpt(), 0);
            if (ptr) {
                HANDLE hFile = ::CreateFile(minimage.GetBuffer(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
                minimage.ReleaseBuffer();
                if (hFile != INVALID_HANDLE_VALUE) {
                    WriteFile(hFile, ptr, blob_bytes, (DWORD*)&sz, 0);
                    CloseHandle(hFile);
                }
            }
        }
    }
    fset.Close();
    return sz;
}

int GetIdInfoSL(int id, CString& fname, CString& minname, CString& exif, CString& path)
{
    int count = 0;
    fname = _T(""); exif = _T(""); path = _T("");
    SLRecordset<IMGSET>  imgset(m_db->GetDb());
    std::string sql = "SELECT FILENAME, FDIR, DTORIGIN, DTFCREATE, MINIIMG, EXIF FROM IMGSET WHERE ID = '" + std::to_string(id) + "'";
    if (imgset.Open(sql) == SQLITE_OK) {
        if (imgset.Next() == SQLITE_ROW) {
            imgset.m_set.m_filename = (char*)sqlite3_column_text(imgset.GetSmpt(), 0);
            imgset.m_set.m_dir = (char*)sqlite3_column_text(imgset.GetSmpt(), 1);
            imgset.m_set.m_dtOrigin = sqlite3_column_int64(imgset.GetSmpt(), 2);
            imgset.m_set.m_fcDate = sqlite3_column_int64(imgset.GetSmpt(), 3);
            imgset.m_set.m_exif = (char*) sqlite3_column_text(imgset.GetSmpt(), 5);
            uint8_t* ptr = (uint8_t*)sqlite3_column_blob(imgset.GetSmpt(), 4);
            DWORD blob_bytes = sqlite3_column_bytes(imgset.GetSmpt(), 4);
            if (ptr) {
                HANDLE hFile = ::CreateFile(minname.GetBuffer(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
                minname.ReleaseBuffer();
                if (hFile != INVALID_HANDLE_VALUE) {
                    WriteFile(hFile, ptr, blob_bytes, (DWORD*)&blob_bytes, 0);
                    CloseHandle(hFile);
                }
            }
            if (imgset.m_set.m_exif.empty()) {
                SYS_SLTIME ss;
                ss.systime = imgset.m_set.m_fcDate;
                imgset.m_set.m_exif = "Path	    : " + imgset.m_set.m_dir + "/" + imgset.m_set.m_filename + "\r\n";
                imgset.m_set.m_exif += "Created : " + std::to_string(ss.sltime.Day) + "/" + std::to_string(ss.sltime.Mounth);
                imgset.m_set.m_exif += "/" + std::to_string(ss.sltime.Year) + " " + std::to_string(ss.sltime.Hour);
                imgset.m_set.m_exif += ":" + std::to_string(ss.sltime.Minute) + ":" + std::to_string(ss.sltime.Second) + "\r\n";
            }
            std::string spath = "";
            if (!imgset.m_set.m_dir.empty()) {
                if (*imgset.m_set.m_dir.rbegin() == '/') {
                    spath = imgset.m_set.m_dir + imgset.m_set.m_filename;
                }
                else {
                    spath = imgset.m_set.m_dir + "/" + imgset.m_set.m_filename;
                }
            }
            path = ConvertFromUTF8(spath);
            exif = ConvertFromUTF8(imgset.m_set.m_exif);
            fname = ConvertFromUTF8(imgset.m_set.m_filename);
        }
    }
    imgset.Close();
    return 0;
}

int ParseExifSL(LPCTSTR src, int szsrc, LPCTSTR out, int szout, SLEXIFSTR& info)
{
    std::vector<CString> exifconst = { _T("FileName   "), _T("Directory "), _T("FileCreateDate "), /* 0, 1, 2 */
                        _T("ImageWidth "), _T("ImageHeight "),  _T("FileType "), /* 3, 4, 5 */
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

int GetFileHashSL(LPCTSTR fpath, unsigned char hash[16])
{

    MD5_CTX mdhash;
    //TLDOM::PTR<IHash> pHash; // hash common interface
    //RESULT Result = E_FAILED;
    unsigned char Buf[4096] = {};
    int szSize = 0; // variable for sizeof Hash
    HANDLE hFile = INVALID_HANDLE_VALUE; // handle for  file
    DWORD dwRead = 0;

    MD5Init(&mdhash);
    hFile = CreateFile(fpath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    // check result
    if (hFile == INVALID_HANDLE_VALUE) {
        return -2;
    }

    //pHash->Clear();
    do {
        if (!ReadFile(hFile, Buf, sizeof(Buf), &dwRead, 0)) {
            dwRead = 0; // error red file
            break;
        }
        szSize += dwRead;
        MD5Update(&mdhash, Buf, dwRead);
        //pHash->Hash(Buf, dwRead); // calculate hash
    } while (dwRead > 0);
    //	pHash->Final(hash, &mdhash); // calculate hash value
    MD5Final(hash, &mdhash);
    CloseHandle(hFile); // close file object
    //pHash.Release(); // fre hash interface

    return szSize;
    return 0;
}

void ConvertHashToStringSL(CString& out, unsigned char buf[16])
{
    CString sym[] = { _T("0"),_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),
            _T("8"),_T("9"),_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F")
    };
    out = _T(""); int a, b;
    for (int i = 0; i < 16; i++) {
        a = b = buf[i];
        a = a >> 4;
        out += sym[a];
        out += sym[(b & 0x0F)];
    }//end main loop
}

void ConvertHashToStringSTR(std::string& out, unsigned char buf[16])
{
    std::string sym[] = { "0","1", "2","3","4","5","6","7","8","9","A","B","C","D","E","F"  };
    out = ""; int a, b;
    for (int i = 0; i < 16; i++) {
        a = b = buf[i];
        a = a >> 4;
        out += sym[a];
        out += sym[(b & 0x0F)];
    }//end main loop
}


CString GetComputerInfoSL(CString& path) {
    CString ret = _T("");
    CString comp = _T("");
    CString volume = _T("");
    CString sFS = _T("");
    DWORD dwVol = 256 * sizeof(TCHAR);
    DWORD dwComp = 256 * sizeof(TCHAR);
    DWORD dwSerial = -1;
    DWORD dwFlag = 0;
    GetComputerName(comp.GetBuffer(dwComp), &dwComp);
    comp.ReleaseBuffer();
    //	ret = path.Left(path.Find('/'));
    GetVolumeInformation(path.Left(3), volume.GetBuffer(1024 * sizeof(TCHAR)), 1024 * sizeof(TCHAR), &dwSerial, &dwVol, &dwFlag, sFS.GetBuffer(100), 100);
    volume.ReleaseBuffer();
    sFS.ReleaseBuffer();
    ret.Format(_T("%X :: %s :: %s :: %X :: %s"), dwSerial, volume, sFS, GetDriveType(path.Left(3)), comp);
    return ret;
 }

bool AddToDublicateSL(int id, std::string& fname, std::string& dir, CString& info)
{
    SLRecordset<IMGREF> rset(m_db->GetDb());
    //SELECT ID FROM IMGREF WHERE FNAME = 'IMG_0248.JPG' AND DIR = 'D:/Pictures/Canon EOS 2000D' AND DISKINFO = '34E09837 :: Worker :: NTFS'
    std::string sql = "SELECT ID FROM IMGREF WHERE FNAME = '" + fname + "' AND DIR = '";
    sql += dir + "' AND DISKINFO = '" + ConvertToUTF8(info) + "'";
    if (rset.Open(sql) == SQLITE_OK) {
        if (rset.Next() == SQLITE_ROW) {
            if (sqlite3_data_count(rset.GetSmpt()) > 0) {
                rset.Close();
                return true; 
            }
        }
    }
    rset.Close();
    sql = "INSERT INTO IMGREF (IDMAIN, FNAME, DIR, DISKINFO) VALUES (?,?,?,?)";
    int bRet = 0;
    int res;
    if (rset.Open(sql) == SQLITE_OK) {
        bRet += (sqlite3_bind_int(rset.GetSmpt(), 1, id) == SQLITE_OK) ? 1 : 0;
        bRet += (sqlite3_bind_text(rset.GetSmpt(), 2, fname.c_str(), fname.length(), 0) == SQLITE_OK) ? 1 : 0;
        bRet += (sqlite3_bind_text(rset.GetSmpt(), 3, dir.c_str(), dir.length(), 0) == SQLITE_OK) ? 1 : 0;
        sql = ConvertToUTF8(info);
        bRet += (sqlite3_bind_text(rset.GetSmpt(), 4, sql.c_str(), sql.length(), 0) == SQLITE_OK) ? 1 : 0;
        if (bRet == 4){
            sqlite3_step(rset.GetSmpt());
        }

    }
    rset.Close();
    return (bRet == 4);
}

inline int BindImgSet(SLRecordset<IMGSET>& img, SLEXIFSTR& info, std::string& errstr) {
    std::vector<std::string> imgFileds = { "TYPE", "MAKE", "MODEL", "EXPTIME", "FNUMBER", "EXPOSPROG", "ISO",
           "OWNER", "SERIAL", "FASEDETECT", "LENSINFO", "LENS", "LENSMODEL", "LENSSERIAL", "INTSERIAL",	 //6- 19
           "APERTURE", "BLUEBALANCE", "REDBALANCE", "COFC", "DOF", "DEPTH", "FOV","FOFVIEW", "SHSPEED", "FOCALLENS35EFL",
         "HFDISTANCE", "LENS35", "LIGHTVALUE", "MEGAPIXELS", "DISKINFO", "EXIF" };



    std::string sq_imgset = "INSERT INTO IMGSET (DTORIGIN, DTFCREATE, WIDTH, HEIGHT, FILENAME, FDIR, HASH, ";
    // MINIIMG";

    std::string sq_tmp = ") VALUES ( '";
    sq_tmp += info.exif[12] + "', '" + info.exif[2] + "', '" + info.exif[3] + "', '" + info.exif[4] + "', ?, ?, '";
    if (info.exif[49].empty()) { info.exif[49] = "exif not found"; }
    sq_tmp += info.exif[49] + "', ?";
    // bind: Filename, Dir, type,  

    int res = -1;
    int count = 0;
    int id = 0;
    std::vector<int> sNum = { 0, 1 };
    for (int i = 0; i < 7; i++) {
        if (info.exif[5 + i].empty()) continue;
        sNum.push_back(i + 5);
        sq_imgset += imgFileds[i] + ", ";
        sq_tmp += ", ?";

    }
    for (int i = 7; i < 31; i++) {
        if (info.exif[6 + i].empty()) continue;
        sNum.push_back(i + 6);
        sq_imgset += imgFileds[i] + ", ";
        sq_tmp += ", ?";
    }
    sq_imgset += "MINIIMG" + sq_tmp + ")";
    res = img.Open(sq_imgset);// == SQLITE_OK)
    // bind textt fields
    int i = 0;
    while (res == SQLITE_OK && i < sNum.size()) {
        res = sqlite3_bind_text(img.GetSmpt(), i + 1, info.exif[sNum[i]].c_str(), -1/*info.exif[sNum[i]].length()*/, NULL);
        i++;
    }
    if (res == SQLITE_OK) {
        res = sqlite3_bind_blob(img.GetSmpt(), i + 1, info.m_ptr, info.m_ptrLength, SQLITE_TRANSIENT);
    }
    if (res == SQLITE_OK) {
        sqlite3_step(img.GetSmpt());
    }
    img.Close();
    if (img.Open("SELECT ID,max(ID) FROM IMGSET") == SQLITE_OK) {
        if (img.Next() == SQLITE_ROW) {
            id = sqlite3_column_int(img.GetSmpt(), 1);
        }
    }
    img.Close();

    return id;
}


int AddFileToDbSL(CString& fname, CString& minname, CString& exif)
{
    if (m_db.get() == nullptr) return 0;
    int64_t id = -2;
    CString volBuf = GetComputerInfoSL(fname); //_T("");	
    CString disk = _T("");
    DWORD serNum = 0;
    DWORD fileSysFlag = 0;
    DWORD maxCompLen = 0;
    DWORD volSerNum = 0;
    CString FileSysNameBuf = _T("");
    DWORD nFileSysFlag = 0;
    int ret = GetVolumeInformation(fname.Left(3)/*fname.GetBuffer()*/,
        volBuf.GetBuffer(256), 256, &volSerNum,
        &maxCompLen,
        &nFileSysFlag,
        FileSysNameBuf.GetBuffer(1024), 1024);
    if (ret == 0) ret = GetLastError();
    fname.ReleaseBuffer();
    FileSysNameBuf.ReleaseBuffer();
    volBuf.ReleaseBuffer();
    disk.Format(_T("%X :: %s :: %s"), volSerNum, volBuf, FileSysNameBuf);


    COleDateTime dt1, dt2;
    SLEXIFSTR est; // bufer to save data
    CString str;
    unsigned char* hData=nullptr;
    int stSize = exif.GetLength() * sizeof(TCHAR);
    GetFileHash(fname, est.Hash.chHash);
    ParseExifSL(exif, exif.GetLength(), 0, 0, est);

    ConvertHashToStringSTR(est.exif[49], &est.Hash.chHash[0]);
    
   
    SLRecordset<IMGSET> imgset(m_db->GetDb());
    std::string sql = "SELECT ID FROM IMGSET WHERE HASH = '" + est.exif[49] + "'";
    if (imgset.Open(sql) != SQLITE_OK) {
        imgset.Close();
        return id;
    }
    if (imgset.Next() == SQLITE_ROW) {
        id = sqlite3_column_int64(imgset.GetSmpt(), 0);
    }
    imgset.Close();
    if (id > 0) {
        AddToDublicateSL(id, est.exif[0], est.exif[1], disk);
        return id;
    }
    
      
    HANDLE hFile = ::CreateFile(minname, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD size = ::GetFileSize(hFile, 0);
        hData = (uint8_t * ) malloc(size);
        est.m_ptr = hData;
        est.m_ptrLength = (hData == nullptr) ? 0 : size;
        if (hData) {
            ReadFile(hFile, hData, size, &size, 0);
        }
        CloseHandle(hFile);
    }
    est.exif[35] = ConvertToUTF8(disk);
    est.exif[36] = ConvertToUTF8(exif);
    id = BindImgSet(imgset, est, sql);
    if (hData != nullptr) free(hData);
    return int(id);
}

int AddFaceToDbSL(int idMain, CString& path, FRECT& rect) {
   int id = 0;
   std::string  sHash = "";
   std::string sql = "SELECT ID FROM FACESET WHERE IDMAIN='" +std::to_string(idMain) += "' AND HASH = '";
   unsigned char hash[16] = {};
   uint32_t sz = GetFileHashSL(path, hash);
   unsigned char* hData = nullptr;
   if (sz < 100)  return -1;
   ConvertHashToStringSTR(sHash, hash);
   sql += sHash + "'";
   SLRecordset<FACESET> fset(m_db->GetDb());
   if (fset.Open(sql) == SQLITE_OK) {
       if (fset.Next() == SQLITE_ROW) {
           if (id != sqlite3_data_count(fset.GetSmpt())) {
               fset.Close();
               return 0;
           }
       }
   }
   fset.Close();
   HANDLE hFile = ::CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
   if (hFile != INVALID_HANDLE_VALUE) {
       DWORD size = ::GetFileSize(hFile, 0);
       hData = (unsigned char*)malloc(sz);
       if (hData) {
           ReadFile(hFile, hData, sz, &size, 0);
       }
       CloseHandle(hFile);
   }
   sql = "INSERT INTO FACESET (IDMAIN, HASH, IMAGE, X, Y, WIDTH, HEIGHT) VALUES (?,?,?, ?,?,?,?)";
   if (fset.Open(sql) == SQLITE_OK) {
       id += sqlite3_bind_int(fset.GetSmpt(), 1, idMain);
       id += sqlite3_bind_text(fset.GetSmpt(), 2, sHash.c_str(), sHash.length(), 0);
       id += sqlite3_bind_int(fset.GetSmpt(), 4, rect.x);
       id += sqlite3_bind_int(fset.GetSmpt(), 5, rect.y);
       id += sqlite3_bind_int(fset.GetSmpt(), 6, rect.width);
       id += sqlite3_bind_int(fset.GetSmpt(), 7, rect.height);
       if (hData != nullptr) {
           id += sqlite3_bind_blob(fset.GetSmpt(), 3, hData, sz, SQLITE_TRANSIENT);
       }
       else {
           id += sqlite3_bind_null(fset.GetSmpt(), 3);
       }
       if (id == 0) {
           sqlite3_step(fset.GetSmpt());
       }
   }
   fset.Close();
   if (hData != nullptr) free(hData);
    return id;
}
//int UpdateFacePosition(int id, int x, int y, int width, int height) {
    
//}

void DeleteFaceImgSL(int id)
{
    std::string sql = "DELETE FROM FACESET WHERE ID ='" + std::to_string(id) +"'";
    if (m_db.get() == nullptr) return;
    int res = sqlite3_exec(m_db->GetDb(), sql.c_str(), NULL, NULL, NULL);
    if (res != SQLITE_OK) {
        AfxGetApp()->DoMessageBox(_T("Can't delete record from FACESET"), MB_ICONSTOP, MB_OK);
    }
}

void DeleteDubImgSL(int id) {
    std::string sql = "DELETE FROM IMGREF WHERE ID ='" + std::to_string(id) +"'";
    if (m_db.get() == nullptr) return;
    int res = sqlite3_exec(m_db->GetDb(), sql.c_str(), NULL, NULL, NULL);
    if (res != SQLITE_OK) {
        AfxGetApp()->DoMessageBox(_T("Can't delete record from IMGREF"), MB_ICONSTOP, MB_OK);
    }
}

void DeleteImgSL(int id)
{
    CString str = _T("");
    int idBuf[100] = {};
    int sz = 100;
    // deleting faces
    while (sz > 0) {
        sz = GetFaceIdInfoSL(id, idBuf, 100, str);
        for (int i = 0; i < sz; i++) {
            DeleteFaceImgSL(idBuf[i]);
            idBuf[i] = 0;
        }
    }//end deleting faces
    sz = 100;
    //deleting dublicates
    while (sz > 0) {
        sz = GetDubIdSL(id, idBuf, 100);
        for (int i = 0; i < sz; i++) {
            DeleteDubImgSL(idBuf[i]);
            idBuf[i] = 0;
        }
    }

    std::string sql = "DELETE FROM IMGSET WHERE ID ='" + std::to_string(id) +"'";
    if (m_db.get() == nullptr) return;
    int res = sqlite3_exec(m_db->GetDb(), sql.c_str(), NULL, NULL, NULL);
    if (res != SQLITE_OK) {
        AfxGetApp()->DoMessageBox(_T("Can't delete record from IMGSET"), MB_ICONSTOP, MB_OK);
    }

}