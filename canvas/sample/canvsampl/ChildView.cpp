// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "canvsampl.h"
#include "ChildView.h"

#include "../../src/Canvas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
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

	// attach
	TCanvas canvas;
	canvas.SetHandle(dc.m_hDC);

	// draw a red line
	canvas.GetPen()->SetColor(clRed);
	canvas.MoveTo(0, 0);  canvas.LineTo(100, 100);

	// draw a grean circle
	canvas.GetBrush()->SetColor(clGreen);
	canvas.Ellipse(10, 10, 90, 90);

	// draw text
	canvas.GetBrush()->SetStyle(bsClear);
	canvas.GetFont()->SetSize(20);
	canvas.GetFont()->SetColor(clBlack);
	canvas.GetFont()->SetStyle(fsBold);
	canvas.TextOut(82, 12, "Canvas");
	canvas.GetFont()->SetColor(clBlue);
	canvas.TextOut(80, 10, "Canvas");

	// detach
	canvas.SetHandle(NULL);
}

