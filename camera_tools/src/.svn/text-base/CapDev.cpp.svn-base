// CapDev.cpp: implementation of the CCapDev class.
// Author: David
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapDev.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCapDev::CCapDev()
{
	m_hWnd = NULL;

	memset(&m_Caps, 0, sizeof(m_Caps));
	memset(&m_Status, 0, sizeof(m_Status));

	memset(&m_OnVideoStreamCallback, 0, sizeof(m_OnVideoStreamCallback));
	memset(&m_OnWaveStreamCallback, 0, sizeof(m_OnWaveStreamCallback));
	memset(&m_OnStatusChanged, 0, sizeof(m_OnStatusChanged));
	memset(&m_OnErrorCallback, 0, sizeof(m_OnErrorCallback));
}

CCapDev::~CCapDev()
{

}

static LRESULT CALLBACK ccdVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	CCapDev* cd = (CCapDev*)capGetUserData(hWnd);
	if (cd != NULL) {
		if (ASSIGNED(cd->m_OnVideoStreamCallback)) {
			BOOL res = TRUE;
			PERFORMEVENT(cd->m_OnVideoStreamCallback)(cd, res, lpVHdr->lpData, lpVHdr->dwBytesUsed, lpVHdr->dwTimeCaptured, lpVHdr->dwFlags);
			return res;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK ccdWaveStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	CCapDev* cd = (CCapDev*)capGetUserData(hWnd);
	if (cd != NULL) {
		if (ASSIGNED(cd->m_OnWaveStreamCallback)) {
			BOOL res = TRUE;
			PERFORMEVENT(cd->m_OnWaveStreamCallback)(cd, res, lpVHdr->lpData, lpVHdr->dwBytesUsed, lpVHdr->dwTimeCaptured, lpVHdr->dwFlags);
			return res;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK ccdStatusCallback(HWND hWnd, int nID, LPCSTR lpsz)
{
	CCapDev* cd = (CCapDev*)capGetUserData(hWnd);
	if (cd != NULL) {
		capGetStatus(hWnd, &cd->m_Status, sizeof(cd->m_Status));
		if (ASSIGNED(cd->m_OnStatusChanged)) {
			BOOL res = TRUE;
			PERFORMEVENT(cd->m_OnStatusChanged)(cd, res, nID, lpsz);
			return res;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK ccdErrorCallback(HWND hWnd, int nID, LPCSTR lpsz)
{
	CCapDev* cd = (CCapDev*)capGetUserData(hWnd);
	if (cd != NULL) {
		if (ASSIGNED(cd->m_OnErrorCallback)) {
			BOOL res = TRUE;
			PERFORMEVENT(cd->m_OnErrorCallback)(cd, res, nID, lpsz);
			return res;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CCapDev::Create(HWND parent)
{
	m_hWnd = capCreateCaptureWindow(NULL, WS_CHILD, 0, 0, 160, 120, parent, 1);

	if (m_hWnd != NULL) {
		capSetUserData(m_hWnd, this);
		capSetCallbackOnVideoStream(m_hWnd, ccdVideoStreamCallback);
		capSetCallbackOnWaveStream(m_hWnd, ccdWaveStreamCallback);
		capSetCallbackOnStatus(m_hWnd, ccdStatusCallback);
		capSetCallbackOnError(m_hWnd, ccdErrorCallback);
	}

	return m_hWnd != NULL;
}

int CCapDev::EnumerateDrivers(CObject *self, TOnEnumDrvFunc callback)
{
	int cnt = 0;
	int i;
	char name[256], version[256];
	for (i=0; i<MAXVIDDRIVERS; i++) {
		if (capGetDriverDescription(i, name, 255, version, 255)) {
			(self->*(callback))(i, name, version);

			cnt++;
		}
	}

	return cnt;
}

bool CCapDev::Connect(int index)
{
	if (!capDriverConnect(m_hWnd, index))
		return false;

	capDriverGetCaps(m_hWnd, &m_Caps, sizeof(m_Caps));
	SetPreviewRate(30);

	return true;
}

void CCapDev::SetPreviewRate(int vl)
{
	capPreviewRate(m_hWnd, vl);
}

void CCapDev::SetOnVideoStreamCallback(CObject *self, TOnStreamCallbackFunc func)
{
	m_OnVideoStreamCallback.self = self;  m_OnVideoStreamCallback.func = func;
}
void CCapDev::SetOnWaveStreamCallback(CObject *self, TOnStreamCallbackFunc func)
{
	m_OnWaveStreamCallback.self = self;  m_OnWaveStreamCallback.func = func;
}

CAPSTATUS CCapDev::GetStatus()
{
	return m_Status;
}

CAPDRIVERCAPS CCapDev::GetDriverCaps()
{
	return m_Caps;
}

void CCapDev::SetOverlay(bool vl)
{
	if (vl)
		SetLive(false);
	capOverlay(m_hWnd, vl);
}

bool CCapDev::GetHasOverlay()
{
	return m_Caps.fHasOverlay != FALSE;
}

void CCapDev::SetOnStatusChanged(CObject *self, TOnStatusChangedFunc func)
{
	m_OnStatusChanged.self = self;  m_OnStatusChanged.func = func;
}

bool CCapDev::DlgVideoFormat()
{
	if (m_Caps.fHasDlgVideoFormat) {
		return capDlgVideoFormat(m_hWnd) != NULL;
	}

	return false;
}

bool CCapDev::DlgVideoSource()
{
	if (m_Caps.fHasDlgVideoSource) {
		return capDlgVideoSource(m_hWnd) != NULL;
	}

	return false;
}

bool CCapDev::DlgVideoDisplay()
{
	if (m_Caps.fHasDlgVideoDisplay) {
		return capDlgVideoDisplay(m_hWnd) != NULL;
	}

	return false;
}

HWND CCapDev::GetHandle()
{
	return m_hWnd;
}

void CCapDev::SetLive(bool vl)
{
	if (vl)
		SetOverlay(false);
	capPreview(m_hWnd, vl);
}

bool CCapDev::GetLive()
{
	return m_Status.fLiveWindow != NULL;
}

void CCapDev::SetPreviewScale(bool vl)
{
	capPreviewScale(m_hWnd, vl);
}

bool CCapDev::GetPreviewScale()
{
	return m_Status.fScale != NULL;
}

CAPTUREPARMS CCapDev::GetCaptureSetup()
{
	CAPTUREPARMS res;
	capCaptureGetSetup(m_hWnd, &res, sizeof(res));
	return res;
}

void CCapDev::SetCaptureSetup(const CAPTUREPARMS &vl)
{
	capCaptureSetSetup(m_hWnd, &vl, sizeof(vl));
}

bool CCapDev::CaptureSequence(bool tofile)
{
	SetLive(false);
	if (tofile)
		return capCaptureSequence(m_hWnd) != NULL;
	else
		return capCaptureSequenceNoFile(m_hWnd) != NULL;
}

bool CCapDev::DlgVideoCompression()
{
	return capDlgVideoCompression(m_hWnd) != NULL;
}

bool CCapDev::Disconnect()
{
	return capDriverDisconnect(m_hWnd) != NULL;
}

CString CCapDev::GetDriverName()
{
	char buf[80];
	capDriverGetName(m_hWnd, buf, sizeof(buf));
	return CString(buf);
}

CString CCapDev::GetDriverVersion()
{
	char buf[40];
	capDriverGetVersion(m_hWnd, buf, sizeof(buf));
	return CString(buf);
}

void CCapDev::SetCaptureFile(const CString &vl)
{
	char buf[256];
	strncpy(buf, vl, sizeof(buf));
	capFileSetCaptureFile(m_hWnd, buf);
}

CString CCapDev::GetCaptureFile()
{
	char buf[256];
	capFileGetCaptureFile(m_hWnd, buf, sizeof(buf));
	return CString(buf);
}

bool CCapDev::Stop()
{
	return capCaptureStop(m_hWnd) != NULL;
}

BITMAPINFOHEADER CCapDev::GetVideoFormat()
{
	BITMAPINFOHEADER res;
	int len = capGetVideoFormatSize(m_hWnd);
	if (len != sizeof(res)) {
		if (len<sizeof(res))
			len = sizeof(res);
		BITMAPINFOHEADER* p = (BITMAPINFOHEADER*)malloc(len);
		capGetVideoFormat(m_hWnd, p, len);
		res = *p;
		delete p;
	} else
		capGetVideoFormat(m_hWnd, &res, len);


	return res;
}

void CCapDev::SetVideoFormat(const BITMAPINFOHEADER &vl)
{
	capSetVideoFormat(m_hWnd, &vl, sizeof(vl));
}

WAVEFORMATEX CCapDev::GetAudioFormat()
{
	WAVEFORMATEX res;
	int len = capGetAudioFormatSize(m_hWnd);
	if (len != sizeof(res)) {
		if (len<sizeof(res))
			len = sizeof(res);
		WAVEFORMATEX* p = (WAVEFORMATEX*)malloc(len);
		capGetAudioFormat(m_hWnd, p, len);
		res = *p;
		delete p;
	} else
		capGetAudioFormat(m_hWnd, &res, len);

	return res;
}

void CCapDev::SetAudioFormat(const WAVEFORMATEX &vl)
{
	capSetAudioFormat(m_hWnd, &vl, sizeof(vl));
}

void CCapDev::SetVideoFormat(int width, int height)
{
	BITMAPINFOHEADER header;
	memset(&header, 0, sizeof(header));
	header.biSize = sizeof(header);

	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biCompression = BI_RGB;

	SetVideoFormat(header);
}

bool CCapDev::GetOverlay()
{
	return m_Status.fOverlayWindow != NULL;
}

void CCapDev::SetOnErrorCallback(CObject *self, TOnErrorCallbackFunc func)
{
	m_OnErrorCallback.self = self;  m_OnErrorCallback.func = func;
}

bool CCapDev::AllocCapFile(DWORD size)
{
	return capFileAlloc(m_hWnd, size) != NULL;
}

bool CCapDev::CopyCaptureFile(const char *newfn)
{
	return capFileSaveAs(m_hWnd, newfn) != NULL;
}
