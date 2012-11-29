// CapDev.h: interface for the CCapDev class.
// Author: David
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPDEV_H__0BBABF8F_84CC_4213_B2B0_E1F2FAE7E098__INCLUDED_)
#define AFX_CAPDEV_H__0BBABF8F_84CC_4213_B2B0_E1F2FAE7E098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vfw.h>

#pragma comment(lib, "vfw32.lib")

#define	MAXVIDDRIVERS	10

//Definition of event types
#ifndef TNotifyFunc
typedef void(CObject::*TNotifyFunc)(CObject* sender);

typedef struct{
	CObject* self;
	TNotifyFunc func;
}TNotifyEvent;
#endif

typedef void (CObject::*TOnEnumDrvFunc)(int index, const char* name, const char* version);

typedef void (CObject::*TOnStreamCallbackFunc)(CObject* sender, BOOL& res, LPBYTE data, DWORD bytesused, DWORD timecaptured, DWORD flags);
typedef struct {
	CObject* self;
	TOnStreamCallbackFunc func;
}TOnStreamCallbackEvent;

typedef void (CObject::*TOnStatusChangedFunc)(CObject* sender, BOOL& res, int nID, LPCSTR lpsz);
typedef struct {
	CObject* self;
	TOnStatusChangedFunc func;
}TOnStatusChangedEvent;

typedef void (CObject::*TOnErrorCallbackFunc)(CObject* sender, BOOL& res, int nID, LPCSTR lpsz);
typedef struct {
	CObject* self;
	TOnErrorCallbackFunc func;
}TOnErrorCallbackEvent;


#ifndef PERFORMEVENT
#define PERFORMEVENT(e) ((e).self->*((e).func))
#endif PERFORMEVENT

#ifndef ASSIGNED
#define ASSIGNED(e) ((e).func!=NULL)
#endif

class CCapDev : public CObject
//class for CAPture DEVice.
{
	friend LRESULT CALLBACK ccdVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr);
	friend LRESULT CALLBACK ccdWaveStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr);
	friend LRESULT CALLBACK ccdStatusCallback(HWND hWnd, int nID, LPCSTR lpsz);
	friend LRESULT CALLBACK ccdErrorCallback(HWND hWnd, int nID, LPCSTR lpsz);

private:
	HWND m_hWnd;

	CAPDRIVERCAPS m_Caps;
	CAPSTATUS m_Status;

	TOnStreamCallbackEvent m_OnVideoStreamCallback;
	TOnStreamCallbackEvent m_OnWaveStreamCallback;
	TOnStatusChangedEvent m_OnStatusChanged;
	TOnErrorCallbackEvent m_OnErrorCallback;
public:
	bool DlgVideoDisplay();
	bool DlgVideoSource();
	bool DlgVideoFormat();
	bool DlgVideoCompression();

	bool Create(HWND parent);
	bool Connect(int index);
	bool Disconnect();
	bool Stop();
	bool CaptureSequence(bool tofile);

	static int EnumerateDrivers(CObject *self, TOnEnumDrvFunc callback);

	CCapDev();
	virtual ~CCapDev();
public:	//properties
	bool CopyCaptureFile(const char* newfn);
	bool AllocCapFile(DWORD size);
	//AudioFormat property
	WAVEFORMATEX GetAudioFormat();
	void SetAudioFormat(const WAVEFORMATEX& vl);

	//VideoFormat property
	BITMAPINFOHEADER GetVideoFormat();
	void SetVideoFormat(const BITMAPINFOHEADER& vl);
	void SetVideoFormat(int width, int height);

	//CaptureFile property
	CString GetCaptureFile();
	void SetCaptureFile(const CString& vl);

	//DriverVersion property
	CString GetDriverVersion();

	//DriverName property
	CString GetDriverName();

	//CaptureSetup property
	CAPTUREPARMS GetCaptureSetup();
	void SetCaptureSetup(const CAPTUREPARMS& vl);

	//PreviewScale property
	bool GetPreviewScale();
	void SetPreviewScale(bool vl);

	//Live property
	bool GetLive();
	void SetLive(bool vl);

	//Handle property
	HWND GetHandle();

	//HasOverlay proerty
	bool GetHasOverlay();

	//Overlay property
	bool GetOverlay();
	void SetOverlay(bool vl);

	//DriverCaps property
	CAPDRIVERCAPS GetDriverCaps();

	//Status property
	CAPSTATUS GetStatus();

	//PreviewRate property
	void SetPreviewRate(int vl);

	//OnErrorCallback event
	void SetOnErrorCallback(CObject* self, TOnErrorCallbackFunc func);

	//OnStatusChanged event.
	void SetOnStatusChanged(CObject* self, TOnStatusChangedFunc func);

	//OnVideoStreamCallback event
	void SetOnVideoStreamCallback(CObject* self, TOnStreamCallbackFunc func);

	//OnWaveStreamCallback event
	void SetOnWaveStreamCallback(CObject* self, TOnStreamCallbackFunc func);
};

#endif // !defined(AFX_CAPDEV_H__0BBABF8F_84CC_4213_B2B0_E1F2FAE7E098__INCLUDED_)
