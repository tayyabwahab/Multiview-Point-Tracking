// OBTracerDlg.h : header file
//

#if !defined(AFX_OBTRACERDLG_H__C18D2AE0_A0DA_4499_B3B7_96DCA77A817B__INCLUDED_)
#define AFX_OBTRACERDLG_H__C18D2AE0_A0DA_4499_B3B7_96DCA77A817B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// COBTracerDlg dialog

#include "Display.h"
#include "Imgproc.h"
#include "MOG.h"
#include "Mframedif.h"
#include "Avi.h"
#include "ReadFile.h"
#include "Labeling.h"
#include "Tracker.h"

#define IMG_W	320
#define IMG_H	240

#define COLORBAND 3
#define MAX_TRACKING_NUM 1000



class COBTracerDlg : public CDialog
{
// Construction
public:
	COBTracerDlg(CWnd* pParent = NULL);	// standard constructor

	BOOL			m_isStop;
	BOOL			m_isPause;
	BOOL			m_isRecord;
	BOOL			m_isThreadRun;
	int				m_dispMode;
	char			m_fileName[256];
	int				m_moduler;
	double			m_alpa;
	int				m_Threshold;
	BYTE*			m_curBackGroundImg;
	PixelData*		m_pixelParam;
	bool			isMatched;
	CWinThread		*m_thread;	

	static UINT MainTread(LPVOID pParam);
	void IniDispWin();
	
// Dialog Data
	//{{AFX_DATA(COBTracerDlg)
	enum { IDD = IDD_OBTRACER_DIALOG };
	CButton	m_recBtn;
	CStatic	m_dispWin;
	CSliderCtrl	m_slider;
	int		m_camNum;
	BOOL	m_live;
	
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COBTracerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COBTracerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonFile();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonPause();
	afx_msg void OnButtonDisp1();
	afx_msg void OnButtonDisp4();
	afx_msg void OnButtonDisp9();
	afx_msg void OnCheckLive();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBTRACERDLG_H__C18D2AE0_A0DA_4499_B3B7_96DCA77A817B__INCLUDED_)
