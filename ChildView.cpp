// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "PhotoProcessor.h"
#include "ChildView.h"

static bool  IsMoveDraw = true;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;
// CChildView
CPen bnrPen(PS_DOT, 0, RGB(255,128,128));

CChildView::CChildView()
{
	tbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	tbi.bmiHeader.biBitCount = 24;
	tbi.bmiHeader.biClrImportant =0;
	tbi.bmiHeader.biClrUsed = 0;
	tbi.bmiHeader.biPlanes = 1;
	tbi.bmiHeader.biCompression =   BI_RGB;
	tbi.bmiHeader.biXPelsPerMeter = 0;
	tbi.bmiHeader.biYPelsPerMeter = 0;
	tbi.bmiHeader.biWidth = 0;
	tbi.bmiHeader.biHeight = 0;
	tbi.bmiHeader.biSizeImage = 0;
	sz = CSize(0,0);
	mem = 0;
	start =CPoint(0,0);
	curs[0] = 	AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	curs[1] = AfxGetApp()->LoadStandardCursor(IDC_HAND);
	m_selr = CRect(FALSERECT,FALSERECT,FALSERECT,FALSERECT);
//	oldCursor = 0;

}

CChildView::~CChildView()
{
	if (mem) free(mem);
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	isz = CSize(0,0);
	CRect rect(0,0,0,0);
	CPen * oldPen = 0;
	if (mem){
		if (sz.cx <= m_size.x) rect.right = sz.cx;
		else rect.right = m_size.x;
		if (sz.cy <= m_size.y) rect.bottom = sz.cy;
		else rect.bottom = m_size.y;
//		::StretchDIBits(dc.m_hDC, m_cp.x, m_cp.y, sz.cx, sz.cy, m_cp.x, m_cp.y, sz.cx, sz.cy, mem, &tbi,  DIB_RGB_COLORS, SRCCOPY);
		::StretchDIBits(dc.m_hDC, rect.left, rect.top, rect.right, rect.bottom, -m_cp.x, -m_cp.y, rect.right, rect.bottom, mem, &tbi,  DIB_RGB_COLORS, SRCCOPY);
		isz = sz;
		if (m_selr.left != FALSERECT){ // draw selection region
			oldPen = dc.SelectObject(&bnrPen);
			dc.MoveTo(m_selr.left, m_selr.top);
			dc.LineTo(m_selr.right, m_selr.top);
			dc.LineTo(m_selr.right, m_selr.bottom);
			dc.LineTo(m_selr.left, m_selr.bottom);
			dc.LineTo(m_selr.left, m_selr.top);
			if (oldPen) dc.SelectObject(oldPen);

		}
	}
	else
		 dc.DeleteTempMap();

	GetParent()->PostMessage(WM_USER+22,0);
	// Do not call CWnd::OnPaint() for painting messages
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_size.x =cx;
	m_size.y = cy;
	m_selr = CRect(FALSERECT,FALSERECT,FALSERECT,FALSERECT);

//	GetClientRect(&clnRect);
//	sz = src_sz;
//	if ((clnRect.right-clnRect.left < sz.cx)||(clnRect.bottom-clnRect.top < sz.cy)){
//		while((clnRect.right-clnRect.left < sz.cx)||(clnRect.bottom-clnRect.top < sz.cy)){
//			sz.cx /= 2;
//			sz.cy /= 2;
//		}
//	};
	GetParent()->PostMessage(WM_USER+11,0);
	
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//if (oldCursor = 0){
	if (mem){
		CRect rect;
		this->GetWindowRect(&rect);
		if ((sz.cx > rect.Width())||(sz.cy > rect.Height())){
			start = point;
			SetCursor(curs[1]);
		}
	}
	//		(false);
//		HCURSOR cursor =AfxGetApp()->LoadCursor(IDC_WAIT);
//		SetCursor(cursor);
	//	ShowCursor(true);
	//	 HCURSOR hCursor;
   // Load the predefined WIndows "size all" cursor.
 //  hCursor = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
//	SetCursor(hCursor);	
	//}

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
//	if (oldCursor){
//	ShowCursor(false);
//		AfxGetApp()->LoadCursor(IDC_ARROW  );
//		ShowCursor(true);
//		oldCursor = 0;
//	}
//	SetCursor(curs[0]);
	CRect rect;
	this->GetWindowRect(&rect);
	int dtx = -(sz.cx- rect.Width());
	int dty = -(sz.cy -rect.Height());
	if ((sz.cx > rect.Width())||(sz.cy > rect.Height())){
		if (mem){
			if (start.x*start.y) {
				m_cp.x +=  point.x-start.x;
				m_cp.y -=  point.y-start.y;
			}
			m_cp.x = min(0, max(m_cp.x, dtx));
			m_cp.y = min(0, max(m_cp.y,dty));
	//		m_cp = m_cp +( point -start);

			Invalidate();
		};
	}
	start = CPoint(0,0);
	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags&MK_LBUTTON){
		if (mem){
			CRect rect;
			this->GetWindowRect(&rect);
			if ((sz.cx > rect.Width())||(sz.cy > rect.Height()))
				SetCursor(curs[1]);
			int dtx = -(sz.cx- rect.Width());
			int dty = -(sz.cy -rect.Height());
			if (start.x*start.y) {
				m_cp.x +=  point.x-start.x;
				m_cp.y -=  point.y-start.y;
			}
			m_cp.x = min(0, max(m_cp.x, dtx));
			m_cp.y = min(0, max(m_cp.y,dty));
			start = point;
			//		m_cp = m_cp +( point -start);
			if (IsMoveDraw){ 	Invalidate();  	}
			IsMoveDraw  = !IsMoveDraw ;
		};
	}
	if (nFlags&MK_RBUTTON){
		if (m_selr.left != FALSERECT){
#if 0
			CRect tmpr = m_selr;
			if (tmpr.left > point.x) tmpr.left = point.x;
			else tmpr.right = point.x;
			if (tmpr.top > point.y) tmpr.top = point.y;
			else tmpr.bottom = point.y;
			if (tmpr.right > sz.cx) tmpr.right = sz.cx;
			if (tmpr.bottom > sz.cy) tmpr.bottom = sz.cy;

			CDC * pdc = this->GetDC();
			CPen * oldPen = pdc->SelectObject(&bnrPen);
			pdc->MoveTo(tmpr.left, tmpr.top);
			pdc->LineTo(tmpr.right, tmpr.top);
			pdc->LineTo(tmpr.right, tmpr.bottom);
			pdc->LineTo(tmpr.left, tmpr.bottom);
			pdc->LineTo(tmpr.left, tmpr.top);
			if (oldPen) pdc->SelectObject(oldPen);
			this->ReleaseDC(pdc);
#endif
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_selr.left != FALSERECT){
		if (m_selr.left > point.x) m_selr.left = point.x;
		else m_selr.right = point.x;
		if (m_selr.top > point.y) m_selr.top = point.y;
		else m_selr.bottom = point.y;
		if ((m_selr.right == m_selr.left)||(m_selr.top == m_selr.bottom)){
			m_selr = CRect(FALSERECT,FALSERECT,FALSERECT,FALSERECT);
		}
		if (m_selr.right > sz.cx) m_selr.right = sz.cx;
		if (m_selr.bottom > sz.cy) m_selr.bottom = sz.cy;
		this->Invalidate();
	}

	CWnd::OnRButtonUp(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (mem){
		if (point.x > sz.cx) point.x = sz.cx;
		if (point.y > sz.cy) point.y = sz.cy;
		m_selr = CRect(point.x, point.y, point.x, point.y);
	};

	CWnd::OnRButtonDown(nFlags, point);
}
