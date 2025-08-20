// OBTracer.h : main header file for the OBTRACER application
//

#if !defined(AFX_OBTRACER_H__F0B4318C_2EC8_4DC5_8678_BFF6B7F2DA4F__INCLUDED_)
#define AFX_OBTRACER_H__F0B4318C_2EC8_4DC5_8678_BFF6B7F2DA4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COBTracerApp:
// See OBTracer.cpp for the implementation of this class
//

class COBTracerApp : public CWinApp
{
public:
	COBTracerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COBTracerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COBTracerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBTRACER_H__F0B4318C_2EC8_4DC5_8678_BFF6B7F2DA4F__INCLUDED_)
