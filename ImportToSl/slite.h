#pragma once
#include <cstdio>
#include <string>
#include <memory>
#include "sqlite3.h"
#include <Windows.h>
#include <afxwin.h>  
#include <vector>


#if defined _M_IX86
#pragma comment(lib, "..\\DLL\\sqlite3_w32.lib")
#elif defined _M_X64
#pragma comment(lib,"..\\DLL\\sqlite3_w64.lib")
#endif

typedef struct _tagSLTIME {
	uint64_t Year : 16;
	uint64_t Mounth : 4;
	uint64_t Day : 5;
	uint64_t Hour : 5;
	uint64_t Minute : 6;
	uint64_t Second : 6;
	uint64_t msSecond : 10;
	uint64_t DayWeek : 9;
}SLTIME;

union SYS_SLTIME {
	int64_t systime;
	SLTIME sltime;
};


inline int64_t ConvertFromMSTime(COleDateTime& dt) {
	//int64_t time = 0L;
	SYS_SLTIME tt;
	tt.systime = 0L;

	SYSTEMTIME stime;
	if (dt.GetStatus() == COleDateTime::valid) {
		if (dt.GetAsSystemTime(stime)) {
			tt.sltime.Year = stime.wYear;
			tt.sltime.Mounth = stime.wMonth;
			tt.sltime.Day = stime.wDay;
			tt.sltime.Hour = stime.wHour;
			tt.sltime.Minute = stime.wMinute;
			tt.sltime.Second = stime.wSecond;
			tt.sltime.msSecond = stime.wMilliseconds;
			tt.sltime.DayWeek = stime.wDayOfWeek;
		}
	}
	//    struct tm tt = { dt.GetSecond(), dt.GetMinute(), dt.GetHour(), dt.GetDay(), dt.GetMonth(), dt.GetYear(), dt.GetDayOfWeek(), dt.GetDayOfYear() };
	 //   int year = dt.GetYear();
	 //   timespec tm;
	 //   timespec_get(&tm, TIME_UTC);
	return tt.systime;
}

inline SYSTEMTIME ConvertIntToSysTime(int64_t sstime) {
	SYSTEMTIME stime;
	SYS_SLTIME tt;
	tt.systime = sstime;
	stime.wYear = tt.sltime.Year;
	stime.wDay = tt.sltime.Day;
	stime.wMonth = tt.sltime.Mounth;
	stime.wHour = tt.sltime.Hour;
	stime.wMinute = tt.sltime.Minute;
	stime.wSecond = tt.sltime.Second;
	stime.wMilliseconds = tt.sltime.msSecond;
	stime.wDayOfWeek = tt.sltime.DayWeek;
	return stime;
}

inline int64_t ConvertFromFileTime(FILETIME& ft) {
	SYS_SLTIME tt;
	SYSTEMTIME stime;
	tt.systime = 0L;
	if (!FileTimeToSystemTime(&ft, &stime)){
		tt.sltime.Year = stime.wYear;
		tt.sltime.Mounth = stime.wMonth;
		tt.sltime.Day = stime.wDay;
		tt.sltime.Hour = stime.wHour;
		tt.sltime.Minute = stime.wMinute;
		tt.sltime.Second = stime.wSecond;
		tt.sltime.msSecond = stime.wMilliseconds;
		tt.sltime.DayWeek = stime.wDayOfWeek;
	}
	return tt.systime;

}





typedef struct _tagSLHASH {
	union {
		uint32_t dwHash[4];
		uint8_t chHash[16];
	} Hash;
}SLHASH128, * SLPHASH128;

typedef struct _SLEXIFTXTSTRUCT {
	/*union{ */
	std::string exif[50];
	SLHASH128 Hash;
	unsigned char* m_ptr = nullptr;
	int m_ptrLength = 0;
	_SLEXIFTXTSTRUCT() {
		memset(Hash.Hash.chHash, 0, sizeof(Hash));
		for (int i = 0; i < 50; i++)   	exif[i].clear();
	};
} SLEXIFSTR, * PSLEXIFSTR;

//extern CString  exifconst[];


#if 0
inline int GetFileHash(LPCTSTR fpath, unsigned char hash[16]) {
	MD5_CTX mdhash;
	unsigned char Buf[4096] = {};
	int szSize = 0; // variable for sizeof Hash
	HANDLE hFile = INVALID_HANDLE_VALUE; // handle for  file
	DWORD dwRead = 0;
	MD5Init(&mdhash);
	hFile = CreateFile(fpath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) {	// check result
		return -2;
	}
	do {
		if (!ReadFile(hFile, Buf, sizeof(Buf), &dwRead, 0)) {
			dwRead = 0; // error red file
			break;
		}
		szSize += dwRead;
		MD5Update(&mdhash, Buf, dwRead);
		//pHash->Hash(Buf, dwRead); // calculate hash
	} while (dwRead > 0);
	MD5Final(hash, &mdhash);
	CloseHandle(hFile); // close file object
	return szSize;
}//end gethash

int ParseExif(LPCTSTR src, int szsrc, LPCTSTR out, int szout, EXIFSTR& info);
//DBFLIB_API int ParseExif(LPCTSTR src,int szsrc, LPCTSTR & out,int szout, EXIFSTR &info);
int InitLocalDB(LPCTSTR fpath);
void ReleaseLocalDB();
void ConvertHashToString(CString& out, unsigned char buf[16]);
int AddFileToDb(CString& fname, CString& minname, CString& exif);
int GetIdInfo(int id, CString& fname, CString& minname, CString& exif, CString& path);
int GetIdCount(int* id_list, int  maxcnt);
int AddFaceToDb(int idMain, CString& path);
int GetDubId(int idMain, int* idBuf, int szBuf);
CString GetDubPath(int idDub, CString& diskinfo); // get dublicate path and disk information
int GetFaceIdInfo(int idMain, int* idBuf, int szBuf, CString& fname); // get list id to faces with idMain
int GetFaceIdImage(int id, CString& minimage); // get list id to faces with idMain

//CString GetAlternatePath(int id_img, CString idPath = _T("")); // get alternate path to image id_img
void DeleteFaceImg(int id); //delete from database
void DeleteDubImg(int id); //delete from database
void DeleteImg(int id); //delete from database
#endif

static inline std::string ConvertToUTF8(CString wstr) {
	std::string str = "";
	int sz = wstr.GetLength() * 3;
	std::unique_ptr<uint8_t[]> zStr(new uint8_t[sz+5]);
	//char zPathBuf[MAX_PATH + 1];
	memset(zStr.get(), 0, sz+5);
	
#if _NATIVE_WCHAR_T_DEFINED
	WideCharToMultiByte(CP_UTF8, 0, wstr.GetBuffer(), -1, (LPSTR) zStr.get(), sz, NULL, NULL);
	str = (char*)zStr.get();
#endif
	return str;
};

inline CString ConvertFromUTF8(std::string ustr) {
	int cchReq = ::MultiByteToWideChar(CP_UTF8, 0, ustr.c_str(), -1, nullptr, 0);
	std::wstring w(cchReq, L'\0');
	int cchWritten = ::MultiByteToWideChar(CP_UTF8, 0, ustr.c_str(), -1, &w[0], cchReq);
	//ATL::CW2A(w.c_str(), CP_ACP);
	CString str= _T("");
#if _NATIVE_WCHAR_T_DEFINED
	str = w.c_str();
#else
	str = ATL::CW2A(w.c_str(), CP_ACP);
#endif
	return str;
};


typedef struct _tagIMGREF{
	int m_Id = -1;
	int m_IdMain = -1;
	std::string m_filename = "";
	std::string m_dir = "";
	std::string m_computer = "";
	std::string m_diskinfo = "";
}IMGREF;


static const char* sql_imgref = "CREATE TABLE IF NOT EXISTS  IMGREF(" \
" ID INTEGER PRIMARY KEY AUTOINCREMENT, IDMAIN INTEGER, " \
"FNAME TEXT, DIR TEXT, COMP TEXT, DISKINFO TEXT)";
static const char* sql_indMAIN = "CREATE INDEX IF NOT EXISTS IDMAININD ON IMGSET(IDMAIN)";


typedef struct _tagSBLOB {
	void* pointer = nullptr;
	sqlite3_blob* m_blob = nullptr;
	int64_t size = 0L;
	unsigned char type = 0;
	inline int allocate_sblob(int64_t sz) {
		if (pointer != nullptr) {
			free_pointer();
		}
		pointer = (sz > 0) ? _aligned_malloc(sz, 256) : nullptr; 
		return (pointer == nullptr) ? false : true;
	};
	inline void free_pointer() {
		if (pointer != nullptr) _aligned_free(pointer);

	};
}SBLOB;

typedef struct _tagIMGSET {
	int m_id = -1;
	int64_t m_dtOrigin = 0L;
	int64_t m_fcDate = 0L;
	int m_width = 0;
	int m_height = 0;;
	std::string m_filename = "";
	std::string m_dir = "";
	std::string m_fType = "";
	std::string m_make = "";
	std::string m_model = "";	 //indexing 
	std::string m_exposureTime = "";
	std::string m_Fnumber = "";
	std::string m_exProg = "";
	std::string m_ISO = "";
	std::string m_Owner = "";
	std::string m_serial = "";
	std::string m_facesDetect = "";
	std::string m_LensInfo = "";
	std::string m_Lens = "";
	std::string m_LensModel = "";
	std::string m_LensSerial = "";
	std::string m_IntSerial = "";
	std::string m_Aperture = "";
	std::string m_BlueBalance = "";
	std::string m_RedBalance = "";
	std::string m_cOfC = "";
	std::string m_DOF = "";
	std::string m_Depth = "";
	std::string m_FOV = "";
	std::string m_fOfView = "";
	std::string m_hfDistance = "";
	std::string m_Lens35 = "";
	std::string m_LightValue = "";
	std::string m_Megapixels = "";
	std::string m_hash = "";
	std::string m_exif;
	SBLOB m_miniImg;
	
}IMGSET;


static const char* sql_imgset = "CREATE TABLE IF NOT EXISTS  IMGSET(" \
" ID INTEGER PRIMARY KEY AUTOINCREMENT, DTORIGIN INTEGER, DTFCREATE INTEGER, WIDTH INTEGER, HEIGHT INTEGER, " \
"FILENAME TEXT, FDIR TEXT, TYPE TEXT, MAKE TEXT, MODEL TEXT, " \
"EXPTIME TEXT, FNUMBER TEXT, EXPOSPROG TEXT, ISO TEXT, OWNER TEXT, SERIAL TEXT, "\
"FASEDETECT TEXT, LENSINFO TEXT, LENS TEXT, LENSMODEL TEXT, LENSSERIAL TEXT, INTSERIAL TEXT, " \
"APERTURE TEXT, BLUEBALANCE TEXT, REDBALANCE TEXT, COFC TEXT, DOF TEXT, DEPTH TEXT, FOV TEXT, "\
"FOFVIEW TEXT, SHSPEED TEXT, FOCALLENS35EFL TEXT, HFDISTANCE TEXT, LENS35 TEXT, LIGHTVALUE TEXT, MEGAPIXELS TEXT, " \
"HASH TEXT, MINIIMG BLOB, EXIF TEXT)";
static const char* sql_indHashMain = "CREATE INDEX IF NOT EXISTS HASHMAIN ON IMGSET(HASH)";
static const char* sql_indModel = "CREATE INDEX IF NOT EXISTS MODEL ON IMGSET(MODEL)";


typedef struct _tagFACESET{
	int m_id = 0;
	int m_idMain = 0;
	std::string m_hash = "00000000000000000000000000000000";
	SBLOB m_image;
}FACESET;

static const char* sql_faces = "CREATE TABLE IF NOT EXISTS  FACESET(" \
" ID INTEGER PRIMARY KEY AUTOINCREMENT, IDMAIN INTEGER, " \
"HASH TEXT, IMAGE BLOB)";
static const char* sql_indHash = "CREATE INDEX IF NOT EXISTS IDMAIN ON FACESET(IDMAIN, HASH)";

template <typename T>
class SLRecordset {
public: 
	SLRecordset() {};
	SLRecordset(sqlite3* _db) { m_db = _db; };
	SLRecordset(sqlite3 * _db, std::string _sql) {
		if (_db != nullptr) {
			m_db = _db;
			m_sqlstr = _sql;
			m_res = sqlite3_prepare_v2(m_db, _sql.c_str(), -1, &m_smpt, 0);
			if (m_res != SQLITE_OK)	m_smpt = nullptr;
		}
	};
	inline int Open(std::string _sql) {
		m_res = -1;
		if (m_smpt != nullptr) { sqlite3_finalize(m_smpt); m_smpt = nullptr;	}
		if (m_db != nullptr) {
			m_sqlstr = _sql;
			m_res = sqlite3_prepare_v2(m_db, _sql.c_str(), -1, &m_smpt, 0);
			if (m_res != SQLITE_OK)	m_smpt = nullptr;
		}
		return m_res;
	};
	inline bool IsOpen() { return (m_smpt == nullptr ? false : true); };
	inline int Reset() { m_res = (m_smpt != nullptr) ? sqlite3_reset(m_smpt) : -1; return m_res;};
	int Next() { m_res = (m_smpt != nullptr) ? sqlite3_step(m_smpt) : -1; return m_res; };
	inline int Close() {
		m_res = (m_smpt != nullptr) ? sqlite3_finalize(m_smpt) : -1;
		m_smpt = nullptr;
		return m_res;
	};
	~SLRecordset() {
		if (m_smpt != nullptr) {
			int res = sqlite3_finalize(m_smpt);
			m_smpt = nullptr;
		}
	};
	inline std::string GetDefaultSQL() { return m_sqlstr; };
	virtual int FillValue() { return 0; };
	inline sqlite3_stmt* GetSmpt() { return m_smpt; };
	T m_set;
	int m_res = -1;
	inline sqlite3* GetParentDb() { return m_db; };

private:
	sqlite3_stmt* m_smpt = nullptr;
	sqlite3* m_db = nullptr;
	std::string m_sqlstr;
};

class SLImageRef : SLRecordset<IMGREF> {
public :
	SLImageRef() {};
	~SLImageRef() { Close(); };
	void OpenWS(int id) {
		std::string sql = "SELECT * FROM IMGREF WHERE IDMAIN = " + std::to_string(id);
		Open(sql);
	};
	inline int FillValue() {
		int count = 0;
		Next();
		if (m_res == SQLITE_ROW) {
			m_set.m_Id = sqlite3_column_int(GetSmpt(), 0);
			m_set.m_IdMain = sqlite3_column_int(GetSmpt(), 1);
			m_set.m_filename = (char *) sqlite3_column_text(GetSmpt(), 2);
			m_set.m_computer = (char*)sqlite3_column_text(GetSmpt(), 3);
			m_set.m_diskinfo = (char*)sqlite3_column_text(GetSmpt(), 4);
			count = 5;
			
		}
		return count;
	};

};
/*
LPCWSTR zPath = Windows::Storage::ApplicationData::Current-> TemporaryFolder->Path->Data();
char zPathBuf[MAX_PATH + 1];
memset(zPathBuf, 0, sizeof(zPathBuf));
WideCharToMultiByte(CP_UTF8, 0, zPath, -1, zPathBuf, sizeof(zPathBuf), NULL, NULL);
sqlite3_temp_directory = sqlite3_mprintf("%s", zPathBuf);
*/

class SLDatabaes {
public:
	SLDatabaes() {};
	~SLDatabaes() {
		if (m_db != nullptr) {
			sqlite3_close(m_db);
		}
	};					 
	inline sqlite3* GetDb()  {
		return m_db;
	};
	int Close() { int res = (m_db != nullptr) ? sqlite3_close(m_db) : SQLITE_OK; m_db = nullptr; return res; };
	int Open(std::string path) {
		CString zPath;
		//LPCWSTR zPath[MAX_PATH + 1] = {}
		GetTempPath(MAX_PATH, zPath.GetBuffer(MAX_PATH + 1));
		zPath.ReleaseBuffer();
		//LPCWSTR zPath = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path->Data();
		char zPathBuf[MAX_PATH + 1];
		memset(zPathBuf, 0, sizeof(zPathBuf));
		WideCharToMultiByte(CP_UTF8, 0, zPath, -1, zPathBuf, sizeof(zPathBuf), NULL, NULL);
#ifdef _M_IX86
		sqlite3_mprintf("%s", zPathBuf);
		sqlite3_win32_set_directory8(SQLITE_WIN32_TEMP_DIRECTORY_TYPE, zPathBuf);

#else 
		sqlite3_temp_directory = sqlite3_mprintf("%s", zPathBuf);

#endif // _M_X32_

		

		int res = sqlite3_open(path.c_str(), &m_db);
		if (res != 0) return res; //error
		char* cerr = 0;
		res = sqlite3_exec(m_db, sql_imgref, 0, 0, &cerr);
		if (res) {
			// cout << cerr
			sqlite3_free(cerr);
			return Close();
		}
		res = sqlite3_exec(m_db, sql_imgset, 0, 0, &cerr);
		if (res) {
			// cout << cerr
			sqlite3_free(cerr);
			return Close();
		}
		res = sqlite3_exec(m_db, sql_indModel, 0, 0, &cerr);
		if (res) {
			sqlite3_free(cerr);
			return Close();
		}
		res = sqlite3_exec(m_db, sql_indHashMain, 0, 0, &cerr);
		if (res) {
			sqlite3_free(cerr);
			return Close();
		}
		res = sqlite3_exec(m_db, sql_faces, 0, 0, &cerr);
		if (res) {
			sqlite3_free(cerr);
			return Close();
		}
		res = sqlite3_exec(m_db, sql_indHash, 0, 0, &cerr);
		if (res) {
			sqlite3_free(cerr);
			return Close();
		}
		return res;
	};
	bool AddToDublicate(int id, std::string& fname, std::string& dir, std::string& info) {
		return false;
	};
	int GetDubId(int idMain, int* idBuf, int szBuf) {
		int count = 0;
		return count;
	};
	std::string GetDubPath(int idDub, std::string& diskinfo) {
		return "";
	}

	


private:
	std::string dbPath;
	SLImageRef m_imgref;
	sqlite3* m_db = nullptr;

};



inline int BindImgSet(SLRecordset<IMGSET>& img, SLEXIFSTR& info, std::string& errstr) {
	std::vector<std::string> imgFileds = { "TYPE", "MAKE", "MODEL", "EXPTIME", "FNUMBER", "EXPOSPROG", "ISO",
		   "OWNER", "SERIAL", "FASEDETECT", "LENSINFO", "LENS", "LENSMODEL", "LENSSERIAL", "INTSERIAL",	 //6- 19
		   "APERTURE", "BLUEBALANCE", "REDBALANCE", "COFC", "DOF", "DEPTH", "FOV","FOFVIEW", "SHSPEED", "FOCALLENS35EFL",
		 "HFDISTANCE", "LENS35", "LIGHTVALUE", "MEGAPIXELS", "EXIF" };



	std::string sq_imgset = "INSERT INTO IMGSET (DTORIGIN, DTFCREATE, WIDTH, HEIGHT, FILENAME, FDIR, HASH, ";
	// MINIIMG";

	std::string sq_tmp = ") VALUES ( '";
	sq_tmp += info.exif[12] + "', '" + info.exif[2] + "', '" + info.exif[3] + "', '" + info.exif[4] +"', ?, ?, '";
	sq_tmp += info.exif[49] + "', ?";  
	// bind: Filename, Dir, type,  

	int res = -1;
	int count = 0;
	std::vector<int> sNum = {0, 1};
	for (int i = 0; i < 7; i++) {
		if (info.exif[5 + i].empty()) continue;
		sNum.push_back(i+5);
		sq_imgset += imgFileds[i] + ", ";
		sq_tmp += ", ?";

	}
	for (int i = 7; i < 30; i++) {
		if (info.exif[6 + i].empty()) continue;
		sNum.push_back(i+6);
		sq_imgset += imgFileds[i] + ", ";
		sq_tmp += ", ?";
	}
	sq_imgset += "MINIIMG" + sq_tmp + ")";
	res = img.Open(sq_imgset);// == SQLITE_OK)
	// bind textt fields
	int i = 0;
	while (res == SQLITE_OK && i < sNum.size()) {
		res = sqlite3_bind_text(img.GetSmpt(), i+1, info.exif[sNum[i]].c_str(), info.exif[sNum[i]].length(), NULL);
		i++;
	}
	if (res == SQLITE_OK) {
		res = sqlite3_bind_blob(img.GetSmpt(), i+1, info.m_ptr, info.m_ptrLength, SQLITE_TRANSIENT);
	}
	if (res == SQLITE_OK) {
		sqlite3_step(img.GetSmpt());
	}
	img.Close();

	return res;
}

int ParseExifSL(LPCTSTR src, int szsrc, LPCTSTR out, int szout, SLEXIFSTR& info);