// ChildView.h : interface of the CChildView class
//


#pragma once

#include <CapDevDs.h>
#include "afxwin.h"


// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

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
	afx_msg void OnFileStart();

public:
	CBitmap m_bmp;
	CCapDevDs m_camera;

	void SetOnStreamCallback(CObject* sender, const void* buffer, DWORD bufferlen, double SampleTime, IMediaSample *pSample);
	afx_msg void OnFileStop();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};

#pragma once


// CChooseCamDlg dialog

class CChooseCamDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseCamDlg)

public:
	CChooseCamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChooseCamDlg();

// Dialog Data
	enum { IDD = IDD_CHOOSECAM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	void OnEnumDrvDsFunc(int index, const char* name, IMoniker* mon);
	CComboBox m_cb_drivers;
	int m_driver_index;
};
