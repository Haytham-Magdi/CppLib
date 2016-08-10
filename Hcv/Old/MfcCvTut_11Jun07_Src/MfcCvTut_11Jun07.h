// MfcCvTut_11Jun07.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMfcCvTut_11Jun07App:
// See MfcCvTut_11Jun07.cpp for the implementation of this class
//

class CMfcCvTut_11Jun07App : public CWinApp
{
public:
	CMfcCvTut_11Jun07App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMfcCvTut_11Jun07App theApp;