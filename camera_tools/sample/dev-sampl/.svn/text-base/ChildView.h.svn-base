// ChildView.h : interface of the CChildView class
//


#pragma once

#include <CapDev.h>


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
	CCapDev m_camera;

	void OnStreamCallbackFunc(CObject* sender, BOOL& res, LPBYTE data, DWORD bytesused, DWORD timecaptured, DWORD flags);
	afx_msg void OnFileStop();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};

