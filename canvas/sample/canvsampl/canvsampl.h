// canvsampl.h : main header file for the canvsampl application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CCanvSamplApp:
// See canvsampl.cpp for the implementation of this class
//

class CCanvSamplApp : public CWinApp
{
public:
	CCanvSamplApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCanvSamplApp theApp;