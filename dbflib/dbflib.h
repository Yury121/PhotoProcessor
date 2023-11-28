#pragma once
#ifndef __PHOTOPROC_DBFLIB_HEADER_
#define __PHOTOPROC_DBFLIB_HEADER_
#include "md5.h"
#if defined _M_IX86
#include "PhotoDB.h"
#endif
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DBFLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DBFLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DBFLIB_EXPORTS
#define DBFLIB_API __declspec(dllexport)
#else
#define DBFLIB_API __declspec(dllimport)
#endif

// This class is exported from the dbflib.dll
class DBFLIB_API Cdbflib {
public:
	Cdbflib(void) {};
	// TODO: add your methods here.
};
#if 0
extern DBFLIB_API int ndbflib;

DBFLIB_API int fndbflib(void);
#endif



extern CString  exifconst[];

#if defined _M_IX86
typedef struct _EXIFTXTSTRUCT {
	/*union{ */
#if 0
//		struct EXIF{
	CString FileName;
	CString Directory;
	CString FileCreateData;
	CString FileType;
	CString ImageWidth;
	CString ImageHeight;
	CString Make;
	CString Model;
	CString ExposureTime;
	CString FNumber;
	CString ExposureProgram;
	CString ISO;
	CString DateTimeOriginal;
	CString OwnerName;
	CString SerialNumber;
	CString FacesDetected;
	CString LensInfo;
	CString Lens;
	CString LensModel;
	CString LensSerialNumber;
	CString InternalSerialNumber;
	CString Aperture;
	CString BlueBalance;
	CString RedBalance;
	CString CircleOfConfusion;
	CString DOF;
	CString Depth_Of_Field;
	CString FOV;
	CString Field_Of_View;
	CString ShutterSpeed;
	CString FocalLength35efl;
	CString HyperfocalDistance;
	CString Lens35efl;
	CString LightValue;
	CString Megapixels;

	/*		CString ___str[35];
		};*/
		//
#endif
	CString exif[50];
	union {
		unsigned char chHash[16];
		unsigned __int32 dwHash[4];
	}Hash;
	_EXIFTXTSTRUCT() {
		memset(Hash.chHash, 0, sizeof(Hash));
		for (int i = 0; i < 50; i++)
			exif[i].Empty();

	};

	} EXIFSTR, * PEXIFSTR;
typedef struct _tagHASH {
	union {
		unsigned char chHash[16];
		unsigned __int32 dwHash[4];
	}Hash;
}HASH128, * PHASH128;

DBFLIB_API int GetFileHash(LPCTSTR fpath, unsigned char hash[16]);
DBFLIB_API int ParseExif(LPCTSTR src,int szsrc, LPCTSTR out, int szout, EXIFSTR &info);
//DBFLIB_API int ParseExif(LPCTSTR src,int szsrc, LPCTSTR & out,int szout, EXIFSTR &info);
DBFLIB_API int InitLocalDB(LPCTSTR fpath);
DBFLIB_API void ReleaseLocalDB();
DBFLIB_API void ConvertHashToString(CString &out, unsigned char buf[16]);
DBFLIB_API int AddFileToDb(CString & fname, CString & minname, CString &exif);
DBFLIB_API int GetIdInfo(int id, CString & fname, CString & minname, CString &exif, CString &path);
DBFLIB_API int GetIdCount(int * id_list, int  maxcnt);
DBFLIB_API int AddFaceToDb(int idMain, CString & path);
DBFLIB_API int GetDubId(int idMain, int * idBuf, int szBuf);
DBFLIB_API CString GetDubPath(int idDub, CString &diskinfo); // get dublicate path and disk information
DBFLIB_API int GetFaceIdInfo(int idMain, int * idBuf, int szBuf, CString & fname); // get list id to faces with idMain
DBFLIB_API int GetFaceIdImage(int id, CString & minimage); // get list id to faces with idMain

DBFLIB_API CString GetAlternatePath(int id_img, CString idPath = _T("")); // get alternate path to image id_img
DBFLIB_API void DeleteFaceImg(int id); //delete from database
DBFLIB_API void DeleteDubImg(int id); //delete from database
DBFLIB_API void DeleteImg(int id); //delete from database

#endif




#endif