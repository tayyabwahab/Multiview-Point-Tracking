// OBTracerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OBTracer.h"
#include "OBTracerDlg.h"
#include <time.h>
#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BYTE inputBuf[IMG_W*IMG_H*COLORBAND];
BYTE dispBuf[IMG_W*IMG_H*COLORBAND];
BYTE dispNewBuf[IMG_W*IMG_H*3];
BYTE fgBuf[IMG_W*IMG_H];	
BYTE fgBuf2[IMG_W*IMG_H];	
BYTE bgBuf[IMG_W*IMG_H*COLORBAND];
BYTE maskBuf[IMG_W*IMG_H];
BYTE StableBuf[IMG_W*IMG_H];
BYTE updateBuf[IMG_W*IMG_H];
BYTE maskBuf3D[IMG_W*IMG_H*COLORBAND];

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COBTracerDlg dialog

COBTracerDlg::COBTracerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COBTracerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COBTracerDlg)
	m_camNum = 0;
	m_live = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COBTracerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COBTracerDlg)
	DDX_Control(pDX, IDC_BUTTON_REC, m_recBtn);
	DDX_Control(pDX, IDC_STATIC_DISPWIN, m_dispWin);
	DDX_Control(pDX, IDC_SLIDER_DELAY, m_slider);
	DDX_Radio(pDX, IDC_RADIO_CAM1, m_camNum);
	DDX_Check(pDX, IDC_CHECK_LIVE, m_live);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COBTracerDlg, CDialog)
	//{{AFX_MSG_MAP(COBTracerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_DISP1, OnButtonDisp1)
	ON_BN_CLICKED(IDC_BUTTON_DISP4, OnButtonDisp4)
	ON_BN_CLICKED(IDC_BUTTON_DISP9, OnButtonDisp9)
	ON_BN_CLICKED(IDC_CHECK_LIVE, OnCheckLive)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COBTracerDlg message handlers

BOOL COBTracerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_isStop = FALSE;
	m_isPause = FALSE;
	m_isRecord = FALSE;
	m_isThreadRun = FALSE;
	m_dispMode = MODE4;
	
	ZeroMemory(m_fileName,256);

	//Set delay time 0 ~ 1000 ms
	m_slider.SetRange(0,1000,TRUE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COBTracerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void COBTracerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);		
	}
	else
	{
		CDialog::OnPaint();
	}
	IniDispWin();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COBTracerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COBTracerDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	if (!m_isThreadRun) 
	{
		UpdateData(TRUE);
		
		m_isPause = FALSE;
		m_isThreadRun = TRUE;
		m_thread = AfxBeginThread((AFX_THREADPROC)MainTread,this);
	}
}

void IMGProTransBMPToBuffer(char *FilePath, char *filename,  BYTE *Buffer, int bitcount, int img_w, int img_h, int colorband)
{
	FILE *in_file;
	BITMAPFILEHEADER hf;			
	BITMAPINFOHEADER hInfo;	
	RGBQUAD hRGB[256];
	
	char file_name[512];
	ZeroMemory(file_name, sizeof(char)*512);
	sprintf(file_name, "%s\\%s.bmp", FilePath, filename);
	in_file = fopen(file_name, "rb");
	if(in_file == NULL)
	{
		return;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER),1, in_file);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, in_file);
	if(hInfo.biBitCount != bitcount)
		return;
	fread(Buffer, sizeof(char), img_h*img_w*colorband, in_file);
	fclose(in_file);
}
UINT COBTracerDlg::MainTread(LPVOID pParam)
{
	COBTracerDlg *pMainDlg = (COBTracerDlg *)pParam;
	int					iRunningFrame = 6;
	BlobInfo			*m_blob;
	
	CAvi				m_avi;
	CDisplay			m_disp;
	CMog				mog1(IMG_W,IMG_H,COLORBAND);
	CTracker			m_track(IMG_W,IMG_H);
	CLabeling			o_Labeling(IMG_W,IMG_H);	
	int					blobNum=0;	
	int					iCnt=0;
	int					iframe = 7;
	char				cPrint[15];
	BlobPast			*m_blobs;

	
	m_blob = new BlobInfo[MAX_TRACKING_NUM];
	

	m_disp.SetDisplay(pMainDlg->m_dispWin.m_hWnd,pMainDlg->m_dispMode,IMG_W,IMG_H);
	m_avi.IniReadAvi(pMainDlg->m_fileName);
	IMGProTransBMPToBuffer("C:\\Users\\Tayyab Wahab\\Documents\\Visual Studio 2010\\Projects\\Tracking New Video\\SVTrackingTestData\\", "masksingle",  maskBuf3D, 24, IMG_W,IMG_H, COLORBAND);

	for(int j = 0 ; j < IMG_H ; j++)
		for(int i = 0 ; i < IMG_W ; i++)
			maskBuf[j * IMG_W +i] = maskBuf3D[j * 3 * IMG_W + i * 3];


	m_disp.SetDisplayMode(pMainDlg->m_dispMode);


	////////////////for trace object/////////////
	BlobPast			*m_blobpast;
	m_blobpast = new BlobPast[MAX_TRACKING_NUM];
	CTracker oTrace(IMG_W, IMG_H);

	while(!pMainDlg->m_isStop) 
	{
		sprintf(cPrint, "%010d", iframe);
		IMGProTransBMPToBuffer("C:\\Users\\Tayyab Wahab\\Documents\\Visual Studio 2010\\Projects\\Tracking New Video\\SVTrackingTestData\\", cPrint,  inputBuf, 24, IMG_W,IMG_H, COLORBAND);
		
		CString count_frame;
		count_frame.Format("%d",iCnt);

		char *p = (char*)(LPCTSTR)count_frame;

		CopyMemory(dispBuf,inputBuf,IMG_W*IMG_H*COLORBAND);
		CopyMemory(dispNewBuf,inputBuf,IMG_W*IMG_H*COLORBAND);
		ZeroMemory(fgBuf2,IMG_W*IMG_H);
		int m_totalNum = 0;
		
		
		if(iframe > iRunningFrame)
		{
			////////////clock begins//////////////
			clock_t begin, end;
			double time_spent;
			begin = clock();
			////////////clock begins//////////////
			
			mog1.RunMog(inputBuf,maskBuf,NULL,fgBuf2);
		
			blobNum = o_Labeling.Labeling(inputBuf,fgBuf2,80,50000,iframe, m_blob);
			//insert new tracking function//////////////////
			//oTrace.InsertKFBlobInformation(m_blob, m_blobpast, blobNum,inputBuf,fgBuf2);

			m_track.InsertKFBlobInformation(m_blob, m_blobpast, blobNum,inputBuf,fgBuf2);
			
			//insert getblobs function 
			m_blobs = m_track.getBlobs(iCnt,  inputBuf,  fgBuf2);
			m_totalNum = m_track.getTotalBlobNum();
			iCnt++;
			
			
			///////////////clock ends///////////////
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			///////////////clock ends///////////////

			int k = 0;
				while (k < m_totalNum)
				{
					if (m_blobs[k].isInFrame)
					{
						//kalman(k);
						FILE *fp;
						char cLog1filePath[256];
						sprintf(cLog1filePath, "PF data file with time.txt");
						fp = fopen(cLog1filePath, "a+");
						fprintf(fp, "Frame Number :%d\tBlob number:%d\tcurrentPos( %0.1f , %0.1f )\tpreviousPos(%0.1f, %.1f)\texpectedPos(%.1f, %.1f)\t%f\n ",iframe,k,m_blobs[k].xPosition[0],m_blobs[k].yPosition[0],m_blobs[k].xPosition[1],m_blobs[k].yPosition[1],m_blobs[k].predictedX,m_blobs[k].predictedY,time_spent); 
						fclose(fp);
				
					}
					k++;
				}
			
			//////////////////////////////////////////////
		}
		m_disp.DrawImage(inputBuf,1,3,cPrint,10,NULL,0);
		m_disp.DrawImage(dispBuf,2,3,"Neighbour Detection",10,m_blob,blobNum);
		m_disp.DrawImageNew(dispNewBuf,3,3,"Particle Filter",10,m_blobs,m_totalNum,-1,p);
		m_disp.DrawImage(fgBuf2,4,1,"Foreground",10,NULL,0);
		iframe++;
		
	}
	pMainDlg->m_isThreadRun = FALSE;
	pMainDlg->IniDispWin();
	delete []m_blob;
	delete []m_blobpast;
	return 0;
}
void COBTracerDlg::OnButtonFile() 
{
}
void COBTracerDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	m_isStop = TRUE;
}

void COBTracerDlg::OnButtonPause() 
{
	// TODO: Add your control notification handler code here
	if (m_isPause) 
	{
		m_isPause = FALSE;
	}
	else
	{
		m_isPause = TRUE;
	}
}
void COBTracerDlg::OnButtonDisp1() 
{
	// TODO: Add your control notification handler code here
	m_dispMode = MODE1;
	IniDispWin();
}

void COBTracerDlg::OnButtonDisp4() 
{
	// TODO: Add your control notification handler code here
	m_dispMode = MODE4;
	IniDispWin();
}

void COBTracerDlg::OnButtonDisp9() 
{
	// TODO: Add your control notification handler code here
	m_dispMode = MODE9;
	IniDispWin();
}
void COBTracerDlg::IniDispWin()
{
	CClientDC dc(this);
	CRect rc;
	CPen Pen;
	CPen *pOldPen;
	Pen.CreatePen(PS_SOLID, 2, RGB(200,200,200));
	pOldPen = (CPen *)dc.SelectObject(&Pen);
	
	m_dispWin.GetWindowRect(rc);
	ScreenToClient(rc);

	dc.FillRect(rc,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

	switch(m_dispMode) {
	case MODE4:
		dc.MoveTo(rc.left, rc.top+(rc.Height()/2));
		dc.LineTo(rc.right, rc.top+(rc.Height()/2));

		dc.MoveTo(rc.left+(rc.Width()/2), rc.top);
		dc.LineTo(rc.left+(rc.Width()/2), rc.bottom);
		break;
	case MODE9:
		dc.MoveTo(rc.left, rc.top+(rc.Height()/3));
		dc.LineTo(rc.right, rc.top+(rc.Height()/3));

		dc.MoveTo(rc.left, rc.top+(rc.Height()/3)*2);
		dc.LineTo(rc.right, rc.top+(rc.Height()/3)*2);

		dc.MoveTo(rc.left+(rc.Width()/3), rc.top);
		dc.LineTo(rc.left+(rc.Width()/3), rc.bottom);

		dc.MoveTo(rc.left+(rc.Width()/3)*2, rc.top);
		dc.LineTo(rc.left+(rc.Width()/3)*2, rc.bottom);
		break;
	default:
		break;
	}
}
void COBTracerDlg::OnCheckLive() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CWnd *pWnd;

	if (m_live) 
	{
		pWnd = (CWnd *)GetDlgItem(IDC_BUTTON_FILE);
		pWnd->EnableWindow(FALSE);
	}
	else
	{
		pWnd = (CWnd *)GetDlgItem(IDC_BUTTON_FILE);
		pWnd->EnableWindow(TRUE);
	}
}
void COBTracerDlg::OnClose()
{
	m_isStop = TRUE;
	Sleep(100);
	CDialog::OnClose();
}
