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
	CChooseCamDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	IMoniker* mon;
	CCapDevDs::FetchDevice(mon, dlg.m_driver_index);
	if (!m_camera.Connect(mon)) {
		::MessageBox(NULL, "Can't connect to camera!", "Error", MB_OK | MB_ICONERROR);
		mon->Release();
		return;
	} // if
	mon->Release();

	m_camera.SetImageWidth(320);  m_camera.SetImageHeight(240);  m_camera.SetImageBitDepth(24);
	m_camera.SetFrameRate(25);
	m_camera.SetRenderType(CDD_OUT_FRAMES);
	m_camera.SetCapturing(true);
	m_camera.SetRunning(true);
}

void CChildView::SetOnStreamCallback(CObject* sender, const void* buffer, DWORD bufferlen, double SampleTime, IMediaSample *pSample)
{
	BITMAP bmp;
	m_bmp.GetBitmap(&bmp);
	
	memmove(bmp.bmBits, buffer, 320 * 240 * 3);

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

	m_camera.SetOnStreamCallback(this, (TOnStreamCallbackDsFunc) SetOnStreamCallback);
	if (!m_camera.Create(NULL)) {
		::MessageBox(NULL, "Can't create camera handle!", "Error", MB_OK | MB_ICONERROR);

		return -1;
	} // if

	return 0;
}

void CChildView::OnDestroy()
{
	m_camera.SetCapturing(false);
	::Sleep(100);
	m_camera.Disconnect();
	CWnd::OnDestroy();
}
// E:\MyProgram\freevision\camera_tools\src\sample\ds-sampl\ChildView.cpp : implementation file
//

#include "stdafx.h"
#include "camera.h"
#include "E:\MyProgram\freevision\camera_tools\src\sample\ds-sampl\ChildView.h"


// CChooseCamDlg dialog

IMPLEMENT_DYNAMIC(CChooseCamDlg, CDialog)
CChooseCamDlg::CChooseCamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseCamDlg::IDD, pParent)
	, m_driver_index(0)
{
}

CChooseCamDlg::~CChooseCamDlg()
{
}

void CChooseCamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_DRIVERS, m_cb_drivers);
	DDX_CBIndex(pDX, IDC_CB_DRIVERS, m_driver_index);
}


BEGIN_MESSAGE_MAP(CChooseCamDlg, CDialog)
END_MESSAGE_MAP()


// CChooseCamDlg message handlers

BOOL CChooseCamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cb_drivers.Clear();
	CCapDevDs::EnumerateDrivers(this, (TOnEnumDrvDsFunc) OnEnumDrvDsFunc);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseCamDlg::OnEnumDrvDsFunc(int index, const char* name, IMoniker* mon)
{
	CString str;
	str.Format("%d: %s", index, name);
	m_cb_drivers.AddString(str);
}
