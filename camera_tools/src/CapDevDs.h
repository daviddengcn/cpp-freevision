// CapDevDs.h: interface for the CCapDevDs class.
// Author: David
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPDEVDS_H__943CACC4_B738_45FD_AB63_7831C1C81890__INCLUDED_)
#define AFX_CAPDEVDS_H__943CACC4_B738_45FD_AB63_7831C1C81890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <strmif.h>     // Generated IDL header file for streams interfaces
#include <control.h>    // generated from control.odl
#include <amvideo.h>    // ActiveMovie video interfaces and definitions
#include <uuids.h>      // declaration of type GUIDs and well-known clsids
#include <qedit.h>

//Denfinitions of event types
typedef void (CObject::*TOnEnumDrvDsFunc)(int index, const char* name, IMoniker* mon);

typedef void (CObject::*TOnStreamCallbackDsFunc)(CObject* sender, const void* buffer, DWORD bufferlen, double SampleTime, IMediaSample *pSample);
typedef struct {
	CObject* self;
	TOnStreamCallbackDsFunc func;
}TOnStreamCallbackDsEvent;

typedef void (CObject::*TOnCopyProgressDsFunc)(CObject* sender, int progress, bool& abort);
typedef struct {
	CObject* self;
	TOnCopyProgressDsFunc func;
}TOnCopyProgressDsEvent;


#ifndef PERFORMEVENT
#define PERFORMEVENT(e) ((e).self->*((e).func))
#endif PERFORMEVENT

#ifndef ASSIGNED
#define ASSIGNED(e) ((e).func!=NULL)
#endif


#ifndef RELEASE_INTER
#define	RELEASE_INTER(p)	if (p != NULL) { p->Release();  p = NULL; }
#endif

#define CDD_OUT_FILE	0
#define CDD_OUT_FRAMES	1
#define CDD_OUT_FILTER	2

#define CCD_CAT_CAPTURE	CLSID_VideoInputDeviceCategory
#define CCD_CAT_VIDCOM	CLSID_VideoCompressorCategory

#define WM_CAP_NOTIFY	(WM_USER+1)

#ifdef _DEBUG
//#define REGISTER_FILTERGRAPH
#endif

class CCapDevDs : public CObject, private ISampleGrabberCB, IAMCopyCaptureFileProgress
{
// for ISampleGrabberCB interface
private:
    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
	// Another callback function. I am not using it now.
    STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
	{return E_NOTIMPL;}

// for IAMCopyCaptureFileProgress interface
	STDMETHODIMP Progress(int iProgress);

// comman methods for all interfaces
private:
	STDMETHODIMP_(ULONG) AddRef() {
		return 2;
	}

	STDMETHODIMP_(ULONG) Release() {
		return 1;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
		if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
			*ppv = (void *) static_cast<ISampleGrabberCB*>(this);
			return NOERROR;
		}
		if (riid == IID_IAMCopyCaptureFileProgress) {
			*ppv = (void *) static_cast<IAMCopyCaptureFileProgress*>(this);
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

private:
	bool UpdateCaptureConfig();
	void TearDownGraph();
	bool UpdateCaptureGraph();
	bool UpdatePreviewGraph();
	bool UpdateGraph();

	IMoniker* m_Capture;
	ICaptureGraphBuilder2* m_Builder;
	IGraphBuilder* m_Graph;
	IBaseFilter* m_VCapture;
	IBaseFilter* m_ComFilter;
	IBaseFilter* m_NullRenderer;
	IAMStreamConfig* m_SConfig;
	IMediaControl *m_MediaControl;
	IAMVfwCaptureDialogs *m_Dlgs;

	char m_FileName[256];
	IBaseFilter* m_OutputRender;
	IBaseFilter* m_GrabberFilter;

	TOnStreamCallbackDsEvent m_OnStreamCallback;
	TOnCopyProgressDsEvent m_OnCopyProgressDs;

	int m_ImageBitDepth;
	int m_ImageWidth;
	int m_ImageHeight;

	bool m_Previewing;
	bool m_Capturing;
	bool m_Running;

	IVideoWindow* m_PreviewWindow;

	IBaseFilter *m_FileRender;
	IFileSinkFilter *m_FileSink;

	HWND m_Parent;
	int m_RenderType;
	double m_FrameRate;

	IMoniker* m_Compressor;
#ifdef REGISTER_FILTERGRAPH
	DWORD m_RotWord;
#endif

//public methods, properties and events
public:	//methods
	bool DlgVideoDisplay(bool test = false);
	bool DlgVideoSource(bool test = false);
	bool DlgVideoFormat(bool test = false);

	bool Create(HWND parent);
	bool Destroy();
	bool Connect(IMoniker* dev);
	bool Disconnect();

	bool AllocCapFile(DWORDLONG size);
	bool CopyCaptureFile(const char* newfn);

	static int EnumerateDrivers(CObject *self, TOnEnumDrvDsFunc callback, REFCLSID category = CCD_CAT_CAPTURE);
	static bool FetchDevice(IMoniker* &res, int index, REFCLSID category = CCD_CAT_CAPTURE);
	static bool CompressorDlg(IMoniker* com, HWND parent = NULL);

	CCapDevDs();
	virtual ~CCapDevDs();
public:	//properties and events
	//ImageBitDepth property
	int GetImageBitDepth();
	void SetImageBitDepth(int vl);

	//ImageHeight property
	int GetImageHeight();
	void SetImageHeight(int vl);

	//ImageWidth property
	int GetImageWidth();
	void SetImageWidth(int vl);

	//OutputRender property
	IBaseFilter* GetOutputRender();
	void SetOutputRender(IBaseFilter* vl);

	//Compressor property
	IMoniker* GetCompressor();
	void SetCompressor(IMoniker* vl);

	//Running property
	bool GetRunning();
	void SetRunning(bool vl);

	//Capturing property
	bool GetCapturing();
	void SetCapturing(bool vl);

	//CaptureFile property
	const char* GetCaptureFile();
	void SetCaptureFile(const char* vl);

	//FrameRate property
	double GetFrameRate();
	void SetFrameRate(double vl);

	//RenderType property
	int GetRenderType();
	void SetRenderType(int vl);

	//PreviewWindow property
	IVideoWindow* GetPreviewWindow();

	//Previewing property
	bool GetPreviewing();
	void SetPreviewing(bool vl);

	//OnCopyProgressDs event
	void SetOnCopyProgressDs(CObject* self, TOnCopyProgressDsFunc func);

	//OnStreamCallback event
	void SetOnStreamCallback(CObject* self, TOnStreamCallbackDsFunc func);
};

#endif // !defined(AFX_CAPDEVDS_H__943CACC4_B738_45FD_AB63_7831C1C81890__INCLUDED_)
