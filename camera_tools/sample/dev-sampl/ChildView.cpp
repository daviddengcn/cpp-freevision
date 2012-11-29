// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "camera.h"
#include "ChildView.h"
#include ".\childview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	BITMAPINFOHEADER hd;
	PBYTE pbuf;
	memset(&hd, 0, sizeof(BITMAPINFOHEADER));
	hd.biSize = sizeof(BITMAPINFOHEADER);
	hd.biWidth = 320;
	hd.biHeight = 240;
	hd.biPlanes = 1;
	hd.biBitCount = 24;
	hd.biCompression = BI_RGB;

	HBITMAP hbmp = ::CreateDIBSection(NULL, (LPBITMAPINFO)&hd, DIB_RGB_COLORS, (LPVOID*)&pbuf, NULL, 0);

//	memset(pbuf, 0, 320 * 240 * 3);

	m_bmp.Attach(hbmp);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_START, OnFileStart)
	ON_COMMAND(ID_FILE_STOP, OnFileStop)
	ON_WM_CREATE()
	ON_WM_DESTROY()
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

	CDC dc_map;
	dc_map.CreateCompatibleDC(&dc);
	CBitmap* bmp_save = dc_map.SelectObject(&m_bmp);
	dc.BitBlt(0, 0, 320, 240, &dc_map, 0, 0, SRCCOPY);
	dc_map.SelectObject(bmp_save);
}


void CChildView::OnFileStart()
{
	// TODO: Add your command handler code here
	if (!m_camera.Connect(0)) {
		::MessageBox(NULL, "Can't connect to camera!", "Error", MB_OK | MB_ICONERROR);
		return;
	} // if
	m_camera.SetLive(false);
	m_camera.SetOverlay(false);

	m_camera.DlgVideoSource();

	m_camera.SetVideoFormat(320, 240);	// color depth is set to 24-bits. use another form of SetVideoFormat for advacned settings.
	CAPTUREPARMS cp = m_camera.GetCaptureSetup();
	cp.fYield = true;
	m_camera.SetCaptureSetup(cp);
	m_camera.CaptureSequence(false);
}

void CChildView::OnStreamCallbackFunc(CObject* sender, BOOL& res, LPBYTE data, DWORD bytesused, DWORD timecaptured, DWORD flags)
{
	BITMAP bmp;
	m_bmp.GetBitmap(&bmp);
	
	memmove(bmp.bmBits, data, 320 * 240 * 3);

	Invalidate(false);
}

void CChildView::OnFileStop()
{
	m_camera.Disconnect();
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_camera.SetOnVideoStreamCallback(this, (TOnStreamCallbackFunc) OnStreamCallbackFunc);
	if (!m_camera.Create(m_hWnd)) {
		::MessageBox(NULL, "Can't create camera handle!", "Error", MB_OK | MB_ICONERROR);

		return -1;
	} // if

	::ShowWindow(m_camera.GetHandle(), SW_HIDE);
	return 0;
}

void CChildView::OnDestroy()
{
	m_camera.Stop();
	::Sleep(100);
	m_camera.Disconnect();
	CWnd::OnDestroy();
}
