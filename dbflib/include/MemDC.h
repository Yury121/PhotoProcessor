#ifndef _MEMDC_H_
#define _MEMDC_H_
#include <smart.h>

//////////////////////////////////////////////////
// Canvas - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-2002, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support. - KR
//
//           11/3/99 Fixed most common complaint. Added
//                   background color fill. - KR
//
//           11/3/99 Added support for mapping modes other than
//                   MM_TEXT as suggested by Lee Sang Hun. - KR
//
//           02/11/02 Added support for CScrollView as supplied
//                    by Gary Kirkham. - KR
//
// This class implements a memory Device Context which allows
// flicker free drawing.
#pragma warning (disable:4200)
class Canvas : public CDC {
private:	
	CBitmap		m_bitmap;		// Offscreen bitmap
	CBitmap*	m_oldBitmap;	// bitmap originally found in Canvas
	CDC*		m_pDC;			// Saves CDC passed in constructor
	CRect		m_rect;			// Rectangle of drawing area.
	BOOL		m_bMemDC;		// TRUE if CDC really is a Memory DC.
	CPen		m_Pen,*m_pOPen;
	CBrush		m_Brush,*m_pOBrush;
	CFont		m_Font,*m_pOFont;
public:
	
	Canvas(CDC* pDC, const CRect* pRect = NULL) : CDC()
	{
		ASSERT(pDC != NULL); 

		m_pOPen = NULL;
		m_pOBrush = NULL;
		m_pOFont = NULL;

		// Some initialization
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the rectangle to draw
		if (pRect == NULL) {
			pDC->GetClipBox(&m_rect);
		} else {
			m_rect = *pRect;
		}

		if (m_bMemDC) {
			// Create a Memory DC
			CreateCompatibleDC(pDC);
			pDC->LPtoDP(&m_rect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);

			SetMapMode(pDC->GetMapMode());

			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_rect.left, m_rect.top);
		} else {
			// Make a copy of the relevent parts of the current DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		// Fill background 
		FillSolidRect(m_rect, pDC->GetBkColor());
	}
	
	~Canvas()	
	{		
		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);			
			
			//Swap back the original bitmap.
			if(m_pOPen != NULL)
				SelectObject(m_pOPen);
			if(m_pOBrush != NULL)
				SelectObject(m_pOBrush);
			if(m_pOFont != NULL)
				SelectObject(m_pOFont);
			SelectObject(m_oldBitmap);		
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.			
			m_hDC = m_hAttribDC = NULL;
		}	
	}
	void selectFont(bool bBold = false,bool bUnderline = false,bool bItalic = false)
	{
		m_Font.DeleteObject();
		if(bUnderline || bBold || bItalic) {
			CFont	fontGui; fontGui.CreateStockObject(DEFAULT_GUI_FONT);
			LOGFONT logfontGui; fontGui.GetLogFont(&logfontGui); 
			if(bBold)
				logfontGui.lfWeight = FW_BOLD;
			if(bUnderline)
				logfontGui.lfUnderline = TRUE;
			if(bItalic)
				logfontGui.lfItalic = TRUE;
			m_Font.CreateFontIndirect(&logfontGui);
		}
		else {
			m_Font.CreateStockObject(DEFAULT_GUI_FONT);
		}
		if(m_pOFont == NULL)
			m_pOFont = SelectObject(&m_Font);
		else 
			SelectObject(&m_Font);
	}
	void selectPen(COLORREF clr)
	{
		m_Pen.DeleteObject();
		m_Pen.CreatePen(PS_SOLID,1,clr);
		if(m_pOPen == NULL)
			m_pOPen = SelectObject(&m_Pen);
		else
			SelectObject(&m_Pen);
	}

	void selectBrush(COLORREF clr)
	{
		m_Brush.DeleteObject();
		m_Brush.CreateSolidBrush(clr);
		if(m_pOBrush == NULL)
			m_pOBrush = SelectObject(&m_Brush);
		else
			SelectObject(&m_Brush);
	}

	// Allow usage as a pointer	
	Canvas* operator->() 
	{
		return this;
	}	

	// Allow usage as a pointer	
	operator Canvas*() 
	{
		return this;
	}
};

static VOID GradientH(CDC *pDC, CRect rect,COLORREF clrStart,COLORREF clrEnd)
{
	int r = (GetRValue(clrEnd) - GetRValue(clrStart));
	int g = (GetGValue(clrEnd) - GetGValue(clrStart));
	int b = (GetBValue(clrEnd) - GetBValue(clrStart));
	int nSteps = max(abs(r), max(abs(g), abs(b)));

	int nPixels = rect.Height();
	nSteps = min(nPixels, nSteps);
	if(nSteps == 0) nSteps = 1;

	float rStep = (float)r/nSteps;
	float gStep = (float)g/nSteps;
	float bStep = (float)b/nSteps;

	r = GetRValue(clrStart);
	g = GetGValue(clrStart);
	b = GetBValue(clrStart);

	BOOL fLowColor = pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE;
	if(!fLowColor && nSteps > 1)
		if(pDC->GetDeviceCaps(BITSPIXEL)*pDC->GetDeviceCaps(PLANES) < 8)
			nSteps = 1; // for 16 colors no gradient

	float nHeightPerStep = (float)rect.Height() / nSteps;
	CRect rcFill(rect);
	CBrush br;
	// Start filling
	for (int i = 0; i < nSteps; i++) 
	{
		rcFill.top = rect.top + (int)(nHeightPerStep * i);
		rcFill.bottom = rect.top + (int)(nHeightPerStep * (i+1));
		if(i == nSteps-1)	//last step (because of problems with float)
			rcFill.bottom = rect.bottom;

		COLORREF clrFill = RGB(r + (int)(i * rStep),
			g + (int)(i * gStep),
			b + (int)(i * bStep));
		if(fLowColor)
		{
			br.CreateSolidBrush(clrFill);
			// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
			pDC->FillRect(rcFill, &br);
			br.DeleteObject();
		}
		else
			pDC->FillSolidRect(rcFill, clrFill);
	}
}

static VOID GradientV(CDC *pDC, CRect rect,COLORREF clrStart,COLORREF clrEnd)
{
	int r = (GetRValue(clrEnd) - GetRValue(clrStart));
	int g = (GetGValue(clrEnd) - GetGValue(clrStart));
	int b = (GetBValue(clrEnd) - GetBValue(clrStart));
	int nSteps = max(abs(r), max(abs(g), abs(b)));

	int nPixels = rect.Width();
	nSteps = min(nPixels, nSteps);
	if(nSteps == 0) nSteps = 1;

	float rStep = (float)r/nSteps;
	float gStep = (float)g/nSteps;
	float bStep = (float)b/nSteps;

	r = GetRValue(clrStart);
	g = GetGValue(clrStart);
	b = GetBValue(clrStart);

	BOOL fLowColor = pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE;
	if(!fLowColor && nSteps > 1)
		if(pDC->GetDeviceCaps(BITSPIXEL)*pDC->GetDeviceCaps(PLANES) < 8)
			nSteps = 1; // for 16 colors no gradient

	float nWidthPerStep = (float)rect.Width() / nSteps;
	CRect rcFill(rect);
	CBrush br;
	// Start filling
	for (int i = 0; i < nSteps; i++) 
	{
		rcFill.left = rect.left + (int)(nWidthPerStep * i);
		rcFill.right = rect.left + (int)(nWidthPerStep * (i+1));
		if(i == nSteps-1)	//last step (because of problems with float)
			rcFill.right = rect.right;

		COLORREF clrFill = RGB(r + (int)(i * rStep),
			g + (int)(i * gStep),
			b + (int)(i * bStep));
		if(fLowColor)
		{
			br.CreateSolidBrush(clrFill);
			// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
			pDC->FillRect(rcFill, &br);
			br.DeleteObject();
		}
		else
			pDC->FillSolidRect(rcFill, clrFill);
	}
}

static HICON LoadIconEx(HINSTANCE hResourceHandle,UINT nIdIcon,int cxyIcon)
{
	typedef struct ICONIMAGE
	{
		BITMAPINFOHEADER   icHeader;      // DIB header
		BYTE				icImage[0];
	} ICONIMAGE;


	typedef struct GRPICONDIRENTRY
	{
		BYTE   bWidth;               // Width, in pixels, of the image
		BYTE   bHeight;              // Height, in pixels, of the image
		BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
		BYTE   bReserved;            // Reserved
		WORD   wPlanes;              // Color Planes
		WORD   wBitCount;            // Bits per pixel
		DWORD   dwBytesInRes;         // how many bytes in this resource?
		WORD   nID;                  // the ID
	} GRPICONDIRENTRY;

#pragma pack( push )
#pragma pack( 2 )

	typedef struct GRPICONDIR
	{
		WORD            idReserved;   // Reserved (must be 0)
		WORD            idType;       // Resource type (1 for icons)
		WORD            idCount;      // How many images?
		GRPICONDIRENTRY   idEntries[0]; // The entries for each image
	} GRPICONDIR;

#pragma pack( pop )

	HRSRC	Resource = NULL;
	DWORD	ResSize = 0;
	HGLOBAL hResource = NULL;
	LPBYTE	lpResource = NULL;

	// Find in standalone Icon group
	if(FindResource(hResourceHandle,MAKEINTRESOURCE(nIdIcon),RT_ICON) == NULL && 
		FindResource(hResourceHandle,MAKEINTRESOURCE(nIdIcon),RT_GROUP_ICON) == NULL)
		Resource = FindResource(hResourceHandle,MAKEINTRESOURCE(nIdIcon),"RT_ICON32");
	else {
		return LoadIcon(hResourceHandle,MAKEINTRESOURCE(nIdIcon));
	}

	if(Resource == NULL)
		return NULL;


	ResSize = SizeofResource(hResourceHandle,Resource);
	hResource = LoadResource(hResourceHandle,Resource);
	lpResource = (LPBYTE)LockResource(hResource); 

	GRPICONDIR* pIconGroup = (GRPICONDIR*)lpResource;
	ICONIMAGE*	icon = (ICONIMAGE*)(lpResource + sizeof(GRPICONDIR) + sizeof(GRPICONDIRENTRY) * pIconGroup->idCount);

	if(pIconGroup->idType != 1)
		return NULL;

	for(int i=0;i<pIconGroup->idCount;i++){

		if(pIconGroup->idEntries[i].bWidth == cxyIcon || cxyIcon == 0){

			LPBYTE pImage = new BYTE [icon->icHeader.biSizeImage];
			int cXY = pIconGroup->idEntries[i].bWidth;

			for(int r=0;r<cXY;r++)
				memcpy(pImage + (cXY - 1 - r)*cXY*4,icon->icImage + r*cXY*4,cXY*4);

			BITMAP bp;
			bp.bmBits = pImage;
			bp.bmWidthBytes = cXY*4;
			bp.bmBitsPixel = 32;
			bp.bmHeight = cXY;
			bp.bmWidth = cXY;
			bp.bmPlanes = 1;
			bp.bmType = 0;

			HBITMAP bmp = CreateBitmapIndirect(&bp);
			HICON hIcon = NULL;

			if(bmp != NULL) {
				ICONINFO ii;
				ii.fIcon = TRUE;
				ii.hbmColor = bmp;
				ii.hbmMask = bmp;
				ii.xHotspot = 0;
				ii.yHotspot = 0;
				hIcon = CreateIconIndirect(&ii);
			}
			DeleteObject(bmp);
			delete pImage;
			return hIcon;
		}

		icon = (ICONIMAGE*)((LPBYTE)icon + pIconGroup->idEntries[i].dwBytesInRes);
	}

	return NULL;
}

static HICON LoadIconEx(const char* IclName,int iIconIndex)
{
	HICON iconLarge,iconSmall;
	::ExtractIconEx(IclName,iIconIndex,&iconLarge,&iconSmall,1);
	if(iconLarge != NULL){
		if(iconSmall != NULL)
			DestroyIcon(iconSmall);
		return iconLarge;
	}
	if(iconSmall != NULL){
		return iconSmall;
	}
	return NULL;
}

#include <gdiplus.h>
using namespace Gdiplus;

class CGdiPlus : public dom::singleton<CGdiPlus>
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR			gdiplusToken;
public:
	void Startup(){GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);}
	void Shutdwon(){GdiplusShutdown(gdiplusToken);}
};

#endif