// PhotoProcessor.h : main header file for the PhotoProcessor application
//
#pragma once
#include <Gdiplus.h>
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPhotoProcessorApp:
// See PhotoProcessor.cpp for the implementation of this class
//

class CPhotoProcessorApp : public CWinApp
{
public:
	CPhotoProcessorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPhotoProcessorApp theApp;
extern ULONG_PTR gdiplusToken;
extern Gdiplus::GdiplusStartupInput gdiplusStartupInput;