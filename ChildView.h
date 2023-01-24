// ChildView.h : interface of the CChildView class
//


#pragma once
#include "tools.h"
#include <gdiplus.h>
#include <gdiplusBitmap.h>

//extern	Gdiplus::Bitmap * BMP1;
//extern	Gdiplus::Bitmap * BMP2;
//extern HBITMAP hbmp;
//extern BITMAP bimg;
extern CBitmap * cBmp;
extern CSize  src_sz;
//extern BITMAPINFO * bi;
//extern BITMAPINFOHEADER hbi;
extern CImage img1;
extern CImage img2;
extern CPChannel red;
extern CPChannel blue;
extern CPChannel green;
extern CPChannel gray;
#define FALSERECT (-10000)
//HCURSOR oldCursor;

extern CRect clnRect; // client rect
extern CSize isz;	// client size

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	CPoint m_cp; // corner;
	BITMAPINFO tbi;
	unsigned char * mem;
	CPChannel red1;
	CPChannel blue1;
	CPChannel green1;
	CSize sz;
	CPoint start;
	CPoint end;
	HCURSOR curs[5];
	CPoint m_size;
	CRect m_selr; // selection rectangle

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

