#pragma once
#include <afxdao.h>
 #pragma warning(disable : 4995)


class CImgSet :	public CDaoRecordset
{
public:
	CImgSet(void);
	~CImgSet(void);

public:
	long m_id;
	CString m_filename;
	CString m_dir;
	CLongBinary m_exif;
	COleDateTime m_fcDate;
	CString m_fType;
	long m_width;
	long m_height;
	CString m_make;
	CString m_model;
	CString m_exposureTime; 
	CString m_Fnumber;
	CString m_exProg; 
	CString m_ISO;
	COleDateTime m_dtOrigin;
	CString m_Owner;
	CString m_serial;
	CString m_facesDetect;
	CString m_LensInfo;
	CString m_Lens;
	CString m_LensModel;
	CString m_LensSerial;
	CString m_IntSerial;
	CString m_Aperture;
	CString m_BlueBalance;
	CString m_RedBalance;
	CString m_cOfC; 
	CString m_DOF;
	CString m_Depth;
	CString m_FOV;
	CString m_fOfView;
	CString m_hfDistance;
	CString m_Lens35;
	CString m_LightValue;
	CString m_Megapixels;
	CLongBinary m_miniImg;
	CString m_hash;
	virtual CString GetDefaultDBName();		// Default database name
//	DECLARE_DYNAMIC(CImgSet);
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
	virtual CString GetDefaultSQL();		// Default SQL for Recordset
#if 0	
	

#endif
};
#if 0	
class CImgrefSet :	public CDaoRecordset
{
public:
	CImgrefSet(void);
	~CImgrefSet(void);

public:
	long m_id;
	long m_idmain; //idMain
	CString m_filename; //FileName
	CString m_dir;		// Directory
	CString m_comp;		// Computer
	CString m_diskinfo;  // DiskInfo
	virtual CString GetDefaultDBName();		// Default database name
//	DECLARE_DYNAMIC(CImgSet);
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
	virtual CString GetDefaultSQL();		// Default SQL for Recordset

	};

#endif


class CImgrefSet :	public CDaoRecordset
{
public:
	CImgrefSet(void);
	~CImgrefSet(void);

public:
	long m_id;
	long m_idmain; //idMain
	CString m_filename; //FileName
	CString m_dir;		// Directory
	CString m_comp;		// Computer
	CString m_diskinfo;  // DiskInfo
	virtual CString GetDefaultDBName();		// Default database name
//	DECLARE_DYNAMIC(CImgrefSet);
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
	virtual CString GetDefaultSQL();		// Default SQL for Recordset

};

//#if 0
class CFaceSet :	public CDaoRecordset
{
public:
	CFaceSet(void);
	~CFaceSet(void);

public:
	long m_id; //Idf
	CLongBinary m_image; //image
	long m_idmain; //imageId
	CString m_hash;	//sHash
	virtual CString GetDefaultDBName();		// Default database name
//	DECLARE_DYNAMIC(CFaceSet);
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
	virtual CString GetDefaultSQL();		// Default SQL for Recordset

};


class CPhotoDB :	public CDaoDatabase
{
public:
	CString m_path;
	CPhotoDB(void);
	~CPhotoDB(void);
	int Init(CString path);
	CImgSet m_imgset;
	CImgrefSet m_dubset;
	CFaceSet  m_faceset;
};


#if 0

#endif

//#endif

CString GetComputerInfo(CString &path);