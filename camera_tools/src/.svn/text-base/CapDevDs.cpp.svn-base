// CapDevDs.cpp: implementation of the CCapDevDs class.
// Author: David
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapDevDs.h"

#include <vfwmsgs.h>
#include <Dmo.h>
#pragma comment(lib, "Msdmo.lib")
#pragma comment(lib, "strmiids.lib")

// Tear down everything downstream of a given filter
static void NukeDownstream(IBaseFilter *filter, IGraphBuilder* graph) 
{
	IPin *pP, *pTo;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;
	HRESULT hr = filter->EnumPins(&pins);
	if (hr == NOERROR) {
		pins->Reset();
		while (hr == NOERROR) {
			ULONG u;
			hr = pins->Next(1, &pP, &u);
			if(hr == S_OK && pP != NULL) {
				if (SUCCEEDED(pP->ConnectedTo(&pTo))) {
					if (pTo != NULL) {
						if (pTo->QueryPinInfo(&pininfo) == NOERROR) {
							if(pininfo.dir == PINDIR_INPUT) {
								NukeDownstream(pininfo.pFilter, graph);
								graph->Disconnect(pTo);
								graph->Disconnect(pP);
								graph->RemoveFilter(pininfo.pFilter);
							}
							pininfo.pFilter->Release();
						}
						pTo->Release();
					}
				}
				pP->Release();
			}
		}
		pins->Release();
	}
}

// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[256];
	HRESULT hr;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
			  GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) {
		hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();
	return hr;
}

// Removes a filter graph from the Running Object Table
void RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapDevDs::CCapDevDs()
{
	CoInitialize(NULL);

	m_Capture = NULL;
	m_Builder = NULL;
	m_Graph = NULL;
	m_VCapture = NULL;
	m_ComFilter = NULL;
	m_NullRenderer = NULL;
	m_SConfig = NULL;
	m_MediaControl = NULL;
	m_Dlgs = NULL;

	memset(m_FileName, 0, sizeof(m_FileName));
	m_OutputRender = NULL;
	m_GrabberFilter = NULL;

	memset(&m_OnStreamCallback, 0, sizeof(m_OnStreamCallback));
	memset(&m_OnCopyProgressDs, 0, sizeof(m_OnCopyProgressDs));

	m_ImageBitDepth = 24;
	m_ImageWidth = 320;
	m_ImageHeight= 240;

	m_Previewing = false;
	m_Capturing = false;
	m_Running = false;

	m_PreviewWindow = NULL;

	m_FileRender = NULL;
	m_FileSink = NULL;

	m_Parent = NULL;
	m_RenderType = CDD_OUT_FILE;
	m_FrameRate = 25;

	m_Compressor = NULL;
#ifdef REGISTER_FILTERGRAPH
	m_RotWord = 0;
#endif
}

CCapDevDs::~CCapDevDs()
{
	Disconnect();
	Destroy();
	SetRenderType(CDD_OUT_FILE);
	SetOutputRender(NULL);

	CoUninitialize();
}


int CCapDevDs::EnumerateDrivers(CObject *self, TOnEnumDrvDsFunc callback, REFCLSID category)
{
	USES_CONVERSION;
	HRESULT hr;
	int cnt;

	ICreateDevEnum *pCreateDevEnum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
		return 0;

	IEnumMoniker *pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(category, &pEm, 0);
	if(hr != NOERROR)
		return 0;

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	cnt = 0;
	while (hr = pEm->Next(1, &pM, &cFetched), hr==S_OK) {
		if (callback != NULL) {
			IPropertyBag *pBag;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if (SUCCEEDED(hr)) {
				VARIANT var;
				var.vt = VT_BSTR; 
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if(hr == NOERROR) {
					(self->*(callback))(cnt, W2A(var.bstrVal), pM);

					SysFreeString(var.bstrVal);
				} else
					(self->*(callback))(cnt, "Noname", pM);
				pBag->Release();
			} else 
				(self->*(callback))(cnt, "Noname", pM);
		}
		pM->Release();
		cnt++;
	}
	pEm->Release();
	pCreateDevEnum->Release();

	return cnt;
}


bool CCapDevDs::Create(HWND parent)
{
	m_Parent = parent;

	if (FAILED(CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, (REFIID)IID_ICaptureGraphBuilder2, (void **)&m_Builder)))
		return false;

	if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&m_Graph)))
		return false;

	if (FAILED(m_Builder->SetFiltergraph(m_Graph)))
		return false;

	if (FAILED(CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&m_NullRenderer))))
		return false;

	m_Graph->QueryInterface(IID_IMediaControl, (void **)&m_MediaControl);

	return true;
}

bool CCapDevDs::Destroy()
{
	RELEASE_INTER(m_MediaControl);
	RELEASE_INTER(m_NullRenderer);
	RELEASE_INTER(m_Graph);
	RELEASE_INTER(m_Builder);

	return true;
}

bool CCapDevDs::Connect(IMoniker *dev)
{
	if (m_Capture != NULL)
		Disconnect();

	if (dev != NULL)
		dev->AddRef();
	m_Capture = dev;

	if (m_Capture == NULL)
		return true;

	if (FAILED(m_Capture->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_VCapture))) {
		RELEASE_INTER(m_Capture);
		return false;
	}

	m_Graph->AddFilter(m_VCapture, L"Video Capture");

	if (m_Builder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, m_VCapture, IID_IAMStreamConfig, (void **)&m_SConfig) != NOERROR) {
		m_Builder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_VCapture, IID_IAMStreamConfig, (void **)&m_SConfig);
	}

	m_Builder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_VCapture, IID_IAMVfwCaptureDialogs, (void **)&m_Dlgs);

	return true;
}

bool CCapDevDs::Disconnect()
{
	SetRunning(false);
	SetCapturing(false);
	SetPreviewing(false);
	TearDownGraph();

	RELEASE_INTER(m_Dlgs);
	RELEASE_INTER(m_SConfig);

	if (m_VCapture != NULL) {
		m_Graph->RemoveFilter(m_VCapture);
		RELEASE_INTER(m_VCapture);
	}

	RELEASE_INTER(m_Capture);

	return true;
}

IVideoWindow* CCapDevDs::GetPreviewWindow()
{
	return m_PreviewWindow;
}

bool CCapDevDs::FetchDevice(IMoniker* &res, int index, REFCLSID category)
{
	int cnt;
	res = NULL;

	ICreateDevEnum *pCreateDevEnum;
	if (CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum) != NOERROR)
		return false;

	IEnumMoniker *pEm;
	if (pCreateDevEnum->CreateClassEnumerator(category, &pEm, 0) != NOERROR) {
		pCreateDevEnum->Release();
		return false;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	cnt = 0;
	while (pEm->Next(1, &pM, &cFetched) == S_OK) {
		if (cnt == index) {
			res = pM;
			break;
		} else
			pM->Release();
		cnt++;
	}
	pEm->Release();
	pCreateDevEnum->Release();

	return res != NULL;
}

bool CCapDevDs::GetPreviewing()
{
	return m_PreviewWindow != NULL && m_Previewing;
}

void CCapDevDs::SetRenderType(int vl)
{
	if (m_RenderType != vl) {
		bool bsave = m_Capturing;
		SetCapturing(false);

		if (m_RenderType == CDD_OUT_FRAMES) {
			//Clear old grabber filer
			RELEASE_INTER(m_GrabberFilter);
		}

		if (vl == CDD_OUT_FRAMES) {
			//create and set new grabber filter
			if (SUCCEEDED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_GrabberFilter))) {
				ISampleGrabber *grabber;
				if (SUCCEEDED(m_GrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&grabber))) {
					grabber->SetBufferSamples(false);
					grabber->SetCallback(this, 0);
					grabber->Release();
				} else
					vl = m_RenderType;
			} else
				vl = m_RenderType;

			if (vl == m_RenderType) {
				RELEASE_INTER(m_GrabberFilter);
			}
		}

		m_RenderType = vl;

		SetCapturing(bsave);
	}
}

int CCapDevDs::GetRenderType()
{
	return m_RenderType;
}

void CCapDevDs::SetFrameRate(double vl)
{
	if (m_FrameRate != vl) {
		bool bsave = m_Running;
		if (m_Running)
			SetRunning(false);

		m_FrameRate = vl;

		if (!UpdateCaptureConfig()) {
			TearDownGraph();
			UpdateGraph();
		}

		if (bsave)
			SetRunning(true);
	}
}

double CCapDevDs::GetFrameRate()
{
	return m_FrameRate;
}

void CCapDevDs::SetCaptureFile(const char *vl)
{
	strncpy(m_FileName, vl, sizeof(m_FileName));
}

bool CCapDevDs::UpdateGraph()
{
	bool res1 = UpdatePreviewGraph();
	bool res2 = UpdateCaptureGraph();

#ifdef REGISTER_FILTERGRAPH
	if (FAILED(AddGraphToRot(m_Graph, &m_RotWord))) {
		m_RotWord = 0;
	}
#endif

	return res1 && res2;
}

void CCapDevDs::TearDownGraph()
{
#ifdef REGISTER_FILTERGRAPH
	if (m_RotWord != 0) {
		RemoveGraphFromRot(m_RotWord);
		m_RotWord = 0;
	}
#endif

	if (m_PreviewWindow != NULL) {
		m_PreviewWindow->put_Visible(FALSE);
		m_PreviewWindow->put_Owner(NULL);
		RELEASE_INTER(m_PreviewWindow);
	}

	RELEASE_INTER(m_FileRender);
	RELEASE_INTER(m_FileSink);

	if (m_VCapture != NULL)
		NukeDownstream(m_VCapture, m_Graph);
}

void CCapDevDs::SetCapturing(bool vl)
{
	if (m_Capturing != vl) {
		bool wasrunning = m_Running;
		if (m_Running)
			SetRunning(false);

		m_Capturing = vl;
//		if (!m_Capturing) {
		TearDownGraph();
		UpdateGraph();
//		} else
//			UpdateCaptureGraph();

		if (wasrunning)
			SetRunning(true);
	}
}

void CCapDevDs::SetPreviewing(bool vl)
{
	if (vl != m_Previewing) {
		bool wasrunning = m_Running;
		if (m_Running)
			SetRunning(false);

			m_Previewing = vl;
//		if (!m_Previewing) {
			TearDownGraph();
			UpdateGraph();
//		} else
//			UpdatePreviewGraph();

		if (wasrunning)
			SetRunning(true);
	}
}

bool CCapDevDs::GetCapturing()
{
	return m_Capturing;
}

bool CCapDevDs::UpdatePreviewGraph()
{
	if (m_Previewing) {
		UpdateCaptureConfig();

		if (m_PreviewWindow == NULL) {
			HRESULT hr = m_Builder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, m_VCapture, NULL, NULL);
			if(hr == VFW_S_NOPREVIEWPIN) {
				// preview was faked up for us using the (only) capture pin
			} else if(hr != S_OK) {
				// maybe it's DV?
				hr = m_Builder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_VCapture, NULL, NULL);
				if(hr != S_OK && hr != VFW_S_NOPREVIEWPIN)
					return false;
			}

			if (m_Graph->QueryInterface(IID_IVideoWindow, (void **)&m_PreviewWindow) != NOERROR )
				return false;

			m_PreviewWindow->put_Owner((OAHWND)m_Parent);
			m_PreviewWindow->put_WindowStyle(WS_CHILD);


			RECT rt;
			::GetClientRect(m_Parent, &rt);

			m_PreviewWindow->SetWindowPosition(0, 0, rt.right, rt.bottom);
			m_PreviewWindow->put_Visible(TRUE);
		}
	}

	return true;
}

bool CCapDevDs::UpdateCaptureGraph()
{
	if (m_Capturing) {
		UpdateCaptureConfig();

		switch (m_RenderType) {
			case CDD_OUT_FILE : {
				if (m_FileName[0] == 0)
					return false;

				if (m_FileRender == NULL) {
					USES_CONVERSION;

					m_Builder->SetOutputFileName(&MEDIASUBTYPE_Avi, A2OLE(m_FileName), &m_FileRender, &m_FileSink);
					IConfigAviMux *config;
					if (m_FileRender->QueryInterface(IID_IConfigAviMux, (void**)&config) == NOERROR) {
						if (config != NULL) {
							config->SetOutputCompatibilityIndex(true);
							config->Release();
						}
					}

					if (m_ComFilter != NULL)
						m_Graph->AddFilter(m_ComFilter, L"Compressor");

					if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, m_VCapture, m_ComFilter, m_FileRender) != NOERROR) {
						if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_VCapture, m_ComFilter, m_FileRender) != NOERROR ) {
							if (m_ComFilter != NULL)
								m_Graph->RemoveFilter(m_ComFilter);

							RELEASE_INTER(m_FileRender);
							RELEASE_INTER(m_FileSink);

							return false;
						} // if
					} // if
				} // if
				break;
			}

			case CDD_OUT_FRAMES : {
				m_Graph->AddFilter(m_GrabberFilter, L"Grabber");

				if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, m_VCapture, m_GrabberFilter, m_NullRenderer) != NOERROR) {
					if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_VCapture, m_GrabberFilter, m_NullRenderer) != NOERROR ) {
						if (m_ComFilter != NULL)
							m_Graph->RemoveFilter(m_GrabberFilter);
						return false;
					} // if
				} // if
				break;
			}

			case CDD_OUT_FILTER : {
				if (m_OutputRender == NULL)
					return false;

				m_Graph->AddFilter(m_OutputRender, L"Customer Render");

				if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, m_VCapture, m_ComFilter, m_OutputRender) != NOERROR) {
					if (m_Builder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_VCapture, m_ComFilter, m_OutputRender) != NOERROR ) {
						if (m_ComFilter != NULL)
							m_Graph->RemoveFilter(m_OutputRender);

						return false;
					}
				}
				break;
			}
		}
	}
	return true;
}

void CCapDevDs::SetRunning(bool vl)
{
	if (m_Graph != NULL && m_MediaControl != NULL) {
		if (vl) {
			if (SUCCEEDED(m_MediaControl->Run())) {
				m_Running = vl;
			} else
				m_MediaControl->Stop();
		} else {
			if (SUCCEEDED(m_MediaControl->Stop())) {
				m_Running = vl;
			}
		}
	}
}

bool CCapDevDs::GetRunning()
{
	return m_Running;
}

void CCapDevDs::SetCompressor(IMoniker *vl)
{
	if (m_Compressor == vl)
		return;

	bool bsave = m_Capturing;
	SetCapturing(false);

	RELEASE_INTER(m_Compressor);
	RELEASE_INTER(m_ComFilter);

	if (vl != NULL)
		vl->AddRef();
	m_Compressor = vl;

	if (m_Compressor != NULL) {
		if (m_Compressor->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_ComFilter) != S_OK) {
			RELEASE_INTER(m_Compressor);
		}
	}

	SetCapturing(bsave);
}

IMoniker* CCapDevDs::GetCompressor()
{
	return m_Compressor;
}

const char* CCapDevDs::GetCaptureFile()
{
	return m_FileName;
}

bool CCapDevDs::CompressorDlg(IMoniker *com, HWND parent)
{
	if (com == NULL)
		return false;

	bool res = false;

	IBaseFilter* fcom;
	if (com->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&fcom)==S_OK) {
		IAMVfwCompressDialogs* dlgs;

		HRESULT hr = fcom->QueryInterface(IID_IAMVfwCompressDialogs, (void**)&dlgs);
		if (hr == S_OK) {
			res = SUCCEEDED(dlgs->ShowDialog(VfwCompressDialog_Config, NULL));

			dlgs->Release();
		}

		fcom->Release();
	}

	return res;
}

bool CCapDevDs::DlgVideoDisplay(bool test)
{
	if (m_Dlgs == NULL)
		return false;
	else if (test)
		return m_Dlgs->HasDialog(VfwCaptureDialog_Display) == S_OK;
	else
		return SUCCEEDED(m_Dlgs->ShowDialog(VfwCaptureDialog_Display, m_Parent));
}

bool CCapDevDs::DlgVideoSource(bool test)
{
	if (m_Dlgs == NULL)
		return false;
	else if (test)
		return m_Dlgs->HasDialog(VfwCaptureDialog_Source) == S_OK;
	else
		return SUCCEEDED(m_Dlgs->ShowDialog(VfwCaptureDialog_Source, m_Parent));
}

bool CCapDevDs::DlgVideoFormat(bool test)
{
	if (m_Dlgs == NULL)
		return false;
	else if (test)
		return m_Dlgs->HasDialog(VfwCaptureDialog_Format) == S_OK;
	else
		return SUCCEEDED(m_Dlgs->ShowDialog(VfwCaptureDialog_Format, m_Parent));
}

void CCapDevDs::SetOutputRender(IBaseFilter *vl)
{
	if (m_OutputRender != NULL)
		m_OutputRender->Release();

	if (vl != NULL)
		vl->AddRef();
	m_OutputRender = vl;
}

IBaseFilter* CCapDevDs::GetOutputRender()
{
	return m_OutputRender;
}

void CCapDevDs::SetImageWidth(int vl)
{
	m_ImageWidth = vl;
}

int CCapDevDs::GetImageWidth()
{
	return m_ImageWidth;
}

void CCapDevDs::SetImageHeight(int vl)
{
	m_ImageHeight = vl;
}

int CCapDevDs::GetImageHeight()
{
	return m_ImageHeight;
}

void CCapDevDs::SetImageBitDepth(int vl)
{
	m_ImageBitDepth = vl;
}

int CCapDevDs::GetImageBitDepth()
{
	return m_ImageBitDepth;
}

bool CCapDevDs::UpdateCaptureConfig()
{
	bool res = false;
	if (m_SConfig != NULL) {
		AM_MEDIA_TYPE *pmt;
		if (m_SConfig->GetFormat(&pmt) == NOERROR) {
			if (pmt->formattype == FORMAT_VideoInfo) {
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				pvi->AvgTimePerFrame = (LONGLONG)(1.0e7 / m_FrameRate);

				pvi->bmiHeader.biWidth = m_ImageWidth;
				pvi->bmiHeader.biHeight = m_ImageHeight;
				pvi->bmiHeader.biBitCount = m_ImageBitDepth;

				HRESULT hr = m_SConfig->SetFormat(pmt);
				if (hr == S_OK)
					res = true;
			}
			MoDeleteMediaType(pmt);
		}

		if (m_SConfig->GetFormat(&pmt) == NOERROR) {
			if (pmt->formattype == FORMAT_VideoInfo) {
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				if (pvi->AvgTimePerFrame>0)
					m_FrameRate = 1.0e7 / pvi->AvgTimePerFrame;

				m_ImageWidth = pvi->bmiHeader.biWidth;
				m_ImageHeight = pvi->bmiHeader.biHeight;
				m_ImageBitDepth = pvi->bmiHeader.biBitCount;
				
			}
			MoDeleteMediaType(pmt);
			return true;
		}

	}
	return res;
}

STDMETHODIMP CCapDevDs::SampleCB(double SampleTime, IMediaSample *pSample)
{
	if (ASSIGNED(m_OnStreamCallback)) {
		void*  buffer;
		pSample->GetPointer((BYTE**)&buffer);
		PERFORMEVENT(m_OnStreamCallback)(this, buffer, pSample->GetActualDataLength(), SampleTime, pSample);
	}
	return E_NOTIMPL;
}

void CCapDevDs::SetOnStreamCallback(CObject *self, TOnStreamCallbackDsFunc func)
{
	m_OnStreamCallback.self = self;  m_OnStreamCallback.func = func;
}

bool CCapDevDs::AllocCapFile(DWORDLONG size)
{
	bool res = false;
	if (m_Builder != NULL) {
		USES_CONVERSION;
		res = SUCCEEDED(m_Builder->AllocCapFile(A2OLE(m_FileName), size));
	}

	return res;
}

bool CCapDevDs::CopyCaptureFile(const char *newfn)
{
	bool res = false;
	if (m_Builder != NULL) {
		USES_CONVERSION;

		if ASSIGNED(m_OnCopyProgressDs)
			res = SUCCEEDED(m_Builder->CopyCaptureFile(A2OLE(m_FileName), A2OLE(newfn), false, this));
		else
			res = SUCCEEDED(m_Builder->CopyCaptureFile(A2OLE(m_FileName), A2OLE(newfn), false, NULL));
	}

	return res;
}

STDMETHODIMP CCapDevDs::Progress(int iProgress)
{
	HRESULT res = S_OK;
	if (ASSIGNED(m_OnCopyProgressDs)) {
		bool abort = false;
		PERFORMEVENT(m_OnCopyProgressDs)(this, iProgress, abort);
		if (abort)
			res = S_FALSE;
	}
	return res;
}

void CCapDevDs::SetOnCopyProgressDs(CObject* self, TOnCopyProgressDsFunc func)
{
	m_OnCopyProgressDs.self = self;  m_OnCopyProgressDs.func = func;
}
