// Display.cpp: implementation of the CDisplay class.
//
//////////////////////////////////////////////////////////////////////
//	GDI객체를 이용하여 영상을 화면에 출력하는 객체  //////////////////
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OBTracer.h"
#include "Display.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisplay::CDisplay()
{
	int i;

	m_color[0] = RGB(0, 255, 0);
	m_color[1] = RGB(0, 0, 255);
	m_color[2] = RGB(255, 255, 0);
	m_color[3] = RGB(0, 255, 255);
	m_color[4] = RGB(255, 0, 255);
	m_color[5] = RGB(255, 255, 255);
	m_color[6] = RGB(100, 100, 0);
	m_color[7] = RGB(0, 100, 100);
	m_color[8] = RGB(200, 100, 100);
	m_color[9] = RGB(100, 0, 100);

	for(i = 0; i < 10; i++)
		m_pen[i] = CreatePen(PS_SOLID, 1, m_color[i]);

	m_isFullMode = FALSE;
	m_selectedCH = 0;
	m_blobNum = 0;
	m_blob = NULL;

}

CDisplay::~CDisplay()
{
	int i;

	for(i = 0; i < 10; i++)
	{
		DeleteObject(m_pen[i]);
	}
	delete []m_dispBuf;
	delete []m_pbDibData;
}

void CDisplay::SetDisplay(HWND dispWin,int dispMode,int imgWidth, int imgHeight)
{
	m_dispWin = dispWin;
	m_dispMode = dispMode;
	m_dispBuf = new BYTE[imgWidth*imgHeight*3];
	m_pbDibData = new BYTE[imgWidth*imgHeight*3];

	m_bmi.biSize = sizeof(BITMAPINFOHEADER);
	m_bmi.biWidth = imgWidth;
	m_bmi.biHeight = imgHeight;
	m_bmi.biPlanes = 1;
	m_bmi.biBitCount = 24;
	m_bmi.biCompression = BI_RGB;
	m_bmi.biSizeImage = WIDTHBYTE(imgWidth*3)*imgHeight;
	m_bmi.biXPelsPerMeter = 0;
	m_bmi.biYPelsPerMeter = 0;
	m_bmi.biClrUsed = 0;
	m_bmi.biClrImportant = 0;

	GetWindowRect(dispWin,m_dispRect);
	
	switch(dispMode) 
	{
	case MODE1:
		m_dispWidth = m_dispRect.Width();
		m_dispHeight = m_dispRect.Height();
			break;
	case MODE4:
		m_dispWidth = m_dispRect.Width()/2;
		m_dispHeight = m_dispRect.Height()/2;
		break;
	case MODE9:
		m_dispWidth = m_dispRect.Width()/3;
		m_dispHeight = m_dispRect.Height()/3;
		break;
	default:
		break;
	}

	m_hdc = GetWindowDC(dispWin);

	SetStretchBltMode(m_hdc,COLORONCOLOR);
}

void CDisplay::SetDisplayMode(int mode)
{
	m_dispMode = mode;

	switch(m_dispMode) 
	{
	case MODE1:
		m_dispWidth = m_dispRect.Width();
		m_dispHeight = m_dispRect.Height();
		break;
	case MODE4:
		m_dispWidth = m_dispRect.Width()/2;
		m_dispHeight = m_dispRect.Height()/2;
		break;
	case MODE9:
		m_dispWidth = m_dispRect.Width()/3;
		m_dispHeight = m_dispRect.Height()/3;
		break;
	default:
		break;
	}
}

void CDisplay::CopyImage1DTo3D(BYTE *src, BYTE *des)
{
	int i,j,index,index1;

	for(j = 0; j < m_bmi.biHeight; j++)
	{
		index = j*m_bmi.biWidth;
		index1 = index*3;
		
		for(i = 0; i < m_bmi.biWidth; i++)
		{
			des[index1 + 3*i] = src[index + i];
			des[index1 + 3*i+1] = src[index + i];
			des[index1 + 3*i+2] = src[index + i];
		}
	}
}

void CDisplay::DrawImage(BYTE *img,int dispNum,int colorBand)
{
	if (colorBand < 3) 
	{
		CopyImage1DTo3D(img,m_dispBuf);
	}
	else
	{
		CopyMemory(m_dispBuf,img,m_bmi.biSizeImage);
	}

	//dispNum: 1 ~ 12
	switch(m_dispMode) 
	{
	case MODE1:
		StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
		break;
	case MODE4:
		switch(dispNum) 
		{
		case 1:
			StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 2:
			StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 3:
			StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 4:
			StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		default:
			break;
		}		
		break;
	case MODE9:
		switch(dispNum) {
		case 1:
			StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 2:
			StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 3:
			StretchDIBits(m_hdc,m_dispWidth*2,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 4:
			StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 5:
			StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 6:
			StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 7:
			StretchDIBits(m_hdc,0,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 8:
			StretchDIBits(m_hdc,m_dispWidth,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 9:
			StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CDisplay::MiDrawText(BYTE *img, int colorBand, char *text, int font, int x, int y,
						  COLORREF color)
{
	HFONT m_hFont;
	HBITMAP  m_bmpDib;
	HDC m_hdcDib;
	BITMAP bm;	
	HPEN pOldPen;
	int i,j;
	
	if (colorBand < 3) 
	{
		CopyImage1DTo3D(img,m_dispBuf);
	}
	else
	{
		CopyMemory(m_dispBuf,img,m_bmi.biSizeImage);
	}
	
	m_hFont = CreateFont(font, font-3, 0, 0, FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");
	m_hdcDib = CreateCompatibleDC(NULL);
	
	HFONT hOldFont = (HFONT)SelectObject(m_hdcDib, m_hFont);
	
	SetTextColor(m_hdcDib, color);
	SetBkMode( m_hdcDib, TRANSPARENT );
	m_bmpDib = CreateCompatibleBitmap(m_hdc, m_bmi.biWidth, m_bmi.biHeight);
	SelectObject(m_hdcDib, m_bmpDib);
	
	SetStretchBltMode(m_hdcDib, COLORONCOLOR);
	StretchDIBits(m_hdcDib, 0, 0, m_bmi.biWidth, m_bmi.biHeight, 0, 0,m_bmi.biWidth, m_bmi.biHeight, 
		m_dispBuf, (LPBITMAPINFO)&m_bmi, DIB_RGB_COLORS, SRCCOPY);
	TextOut(m_hdcDib, x, y,text,strlen(text));

	SelectObject(m_hdcDib,hOldFont);
	
	GetObject( m_bmpDib, sizeof(bm), &bm );
	GetDIBits( m_hdcDib, m_bmpDib, 0, m_bmi.biHeight, m_dispBuf, (BITMAPINFO*)&m_bmi, DIB_RGB_COLORS );

	if (colorBand > 1) 
	{
		CopyMemory(img,m_dispBuf,m_bmi.biSizeImage);
	}
	else
	{
		for(j = 0; j < m_bmi.biHeight; j++)
		{
			for(i = 0; i < m_bmi.biWidth; i++)
			{
				img[j*m_bmi.biWidth+i] = m_dispBuf[3*j*m_bmi.biWidth+3*i];
			}
		}
	}	
	
	DeleteObject(m_hFont);
	DeleteObject(m_bmpDib);
	DeleteDC(m_hdcDib);
}

void CDisplay::DrawImage(BYTE *img,int dispNum, int colorBand, char *title,int font,
						 BlobInfo *pBlob,int blobNum)
{
	if (m_isFullMode && (dispNum != m_selectedCH)) 
		return;
	
	HFONT m_hFont;
	HBITMAP  m_bmpDib;
	HDC m_hdcDib;
	BITMAP bm;	
	HPEN pOldPen;
	int i;
	
	if (colorBand < 3) 
	{
		CopyImage1DTo3D(img,m_dispBuf);
	}
	else
	{
		CopyMemory(m_dispBuf,img,m_bmi.biSizeImage);
	}
	
	m_hFont = CreateFont(font, font-3, 0, 0, FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");
	/*m_hFont = CreateFont(font+3, font+3, 0, 0, FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");*/
	m_hdcDib = CreateCompatibleDC(NULL);
	
	HFONT hOldFont = (HFONT)SelectObject(m_hdcDib, m_hFont);
	
	SetTextColor(m_hdcDib, RGB(255, 0, 0));
	SetBkMode( m_hdcDib, TRANSPARENT );
	m_bmpDib = CreateCompatibleBitmap(m_hdc, m_bmi.biWidth, m_bmi.biHeight);
	SelectObject(m_hdcDib, m_bmpDib);
	
	SetStretchBltMode(m_hdcDib, COLORONCOLOR);
	StretchDIBits(m_hdcDib, 0, 0, m_bmi.biWidth, m_bmi.biHeight, 0, 0,m_bmi.biWidth, 
			m_bmi.biHeight, m_dispBuf, (LPBITMAPINFO)&m_bmi, DIB_RGB_COLORS, SRCCOPY);
	TextOut(m_hdcDib, 5, 1,title,strlen(title));
	
	SetBkMode( m_hdcDib, OPAQUE );
	
	CPoint point;
	BlobInfo *tmpBlob;
	tmpBlob = pBlob;
	
	for(i = 0; i < blobNum; i++)
	{
	//	if (pBlob[i].historyCount > 3)
		{
		//	pOldPen = (HPEN)SelectObject(m_hdcDib,m_pen[pBlob[i].label]);
			
			TextOut(m_hdcDib, pBlob[i].rect.left, m_bmi.biHeight - pBlob[i].rect.top,(LPCSTR)pBlob[i].name,
				strlen(pBlob[i].name));
			MoveToEx(m_hdcDib,pBlob[i].rect.left, m_bmi.biHeight - pBlob[i].rect.top,NULL);
			LineTo(m_hdcDib,pBlob[i].rect.right, m_bmi.biHeight - pBlob[i].rect.top);
			
			LineTo(m_hdcDib,pBlob[i].rect.right, m_bmi.biHeight - pBlob[i].rect.bottom);
			LineTo(m_hdcDib,pBlob[i].rect.left, m_bmi.biHeight - pBlob[i].rect.bottom);
			LineTo(m_hdcDib,pBlob[i].rect.left, m_bmi.biHeight - pBlob[i].rect.top);
			
			/*
			point.x = pBlob[i].posQ[pBlob[i].m_tail%HISTORY_NUM].x;
			point.y = pBlob[i].posQ[pBlob[i].m_tail%HISTORY_NUM].y;
			pBlob[i].m_tail--;
			MoveToEx(m_hdcDib,point.x, point.y,NULL);
			
			int j = 1;
			while (j < pBlob[i].historyCount-1) 
			{
				point.x = pBlob[i].posQ[pBlob[i].m_tail%HISTORY_NUM].x;
				point.y = pBlob[i].posQ[pBlob[i].m_tail%HISTORY_NUM].y;
				pBlob[i].m_tail--;
				j++;
				LineTo(m_hdcDib,point.x, point.y);
			}
			*/
		}
	}
    SelectObject(m_hdcDib,hOldFont);
	
	GetObject( m_bmpDib, sizeof(bm), &bm );
	GetDIBits( m_hdcDib, m_bmpDib, 0, m_bmi.biHeight, m_dispBuf, (BITMAPINFO*)&m_bmi, DIB_RGB_COLORS );
	
	
	DeleteObject(m_hFont);
	DeleteObject(m_bmpDib);
	DeleteDC(m_hdcDib);

	//dispNum: 1 ~ 12
	switch(m_dispMode) 
	{
	case MODE1:
		StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
			m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
		break;
	case MODE4:
		switch(dispNum) 
		{
		case 1:
			StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 2:
			StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 3:
			StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 4:
			StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		default:
			break;
		}		
		break;
		case MODE9:
			switch(dispNum) {
			case 1:
				StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 2:
				StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 3:
				StretchDIBits(m_hdc,m_dispWidth*2,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 4:
				StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 5:
				StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 6:
				StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 7:
				StretchDIBits(m_hdc,0,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 8:
				StretchDIBits(m_hdc,m_dispWidth,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 9:
				StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			default:
				break;
			}
			break;
			default:
				break;
	}
}



void CDisplay::DrawImageNew(BYTE *img,int dispNum, int colorBand, char *title,int font,
						 BlobPast *pBlob, int blobNum, int check, char *count)
{
	if (m_isFullMode && (dispNum != m_selectedCH)) 
		return;
	
	HFONT m_hFont;
	HBITMAP  m_bmpDib;
	HDC m_hdcDib;
	BITMAP bm;	
	HPEN pOldPen;
	int i;
	
	if (colorBand < 3) 
	{
		CopyImage1DTo3D(img,m_dispBuf);
	}
	else
	{
		CopyMemory(m_dispBuf,img,m_bmi.biSizeImage);
	}
	
	/*m_hFont = CreateFont(font, font-3, 0, 0, FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");  */
	//(font, font-3, 0, 0,   == font hight, font width, space below, space above)
	m_hFont = CreateFont(font+5, font-3, 0, 0, FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");

	m_hdcDib = CreateCompatibleDC(NULL);
	
	HFONT hOldFont = (HFONT)SelectObject(m_hdcDib, m_hFont);
	
	SetTextColor(m_hdcDib, RGB(255, 0, 0));
	SetBkMode( m_hdcDib, TRANSPARENT );
	m_bmpDib = CreateCompatibleBitmap(m_hdc, m_bmi.biWidth, m_bmi.biHeight);
	SelectObject(m_hdcDib, m_bmpDib);
	
	SetStretchBltMode(m_hdcDib, COLORONCOLOR);
	StretchDIBits(m_hdcDib, 0, 0, m_bmi.biWidth, m_bmi.biHeight, 0, 0,m_bmi.biWidth, 
			m_bmi.biHeight, m_dispBuf, (LPBITMAPINFO)&m_bmi, DIB_RGB_COLORS, SRCCOPY);
	TextOut(m_hdcDib, 5, 1,title,strlen(title));
	//TextOut(m_hdcDib, 2, 2,title,strlen(title));
	SetBkMode( m_hdcDib, OPAQUE );
	
	CPoint point;
	//BlobInfo *tmpBlob;
	//tmpBlob = pBlob;
	

	for(i = blobNum-1; i >= 0; i--)
	{

		//if (pBlob[i].isInFrame && pBlob[i].checker <= 10 && pBlob[i].xPosition[0] > 0 && pBlob[i].histoty_count >= 5)
		if (pBlob[i].isInFrame && pBlob[i].checker <= 10 && pBlob[i].xPosition[0] > 0) 
		{
			CString name;
			name.Format(_T("%s"),pBlob[i].name_blob);
			pOldPen = (HPEN)SelectObject(m_hdcDib,m_pen[i%10]);
			
			//ID on left bottom
			//TextOut(m_hdcDib, (int)pBlob[i].lPosition[0], m_bmi.biHeight - (int)pBlob[i].tPosition[0],(LPCSTR)name,strlen(name));
			
			//ID on mid bottom
			int x = (( (int)pBlob[i].lPosition[0] + (int)pBlob[i].rPosition[0]) / 2 ) ; //-5;
			int y = ( m_bmi.biHeight - (int)pBlob[i].bPosition[0]) - 15;
			//int y =  (int)pBlob[i].tPosition[0];
			//int y = ( m_bmi.biHeight - (int)pBlob[i].tPosition[0]) + ((int)pBlob[i].tPosition[0] - (int)pBlob[i].bPosition[0]);
			TextOut(m_hdcDib, x , y,(LPCSTR)name,
				strlen(name));
			
			
			if (check == -1)
			{
				MoveToEx(m_hdcDib,(int)pBlob[i].lPosition[0], m_bmi.biHeight - (int)pBlob[i].tPosition[0],NULL);
				LineTo(m_hdcDib,pBlob[i].rpPosition[0], m_bmi.biHeight - pBlob[i].tpPosition[0]);
				LineTo(m_hdcDib,pBlob[i].rpPosition[0], m_bmi.biHeight - pBlob[i].bpPosition[0]);
				LineTo(m_hdcDib,pBlob[i].lpPosition[0], m_bmi.biHeight - pBlob[i].bpPosition[0]);
				LineTo(m_hdcDib,pBlob[i].lpPosition[0], m_bmi.biHeight - pBlob[i].tpPosition[0]);

				
				point.x = pBlob[i].xPosition[0];
				point.y = pBlob[i].yPosition[0];
			}
			else
			{
				MoveToEx(m_hdcDib,(int)pBlob[i].predictedX - (pBlob[i].xPosition[0]-pBlob[i].lpPosition[0]), (int)pBlob[i].predictedY + (pBlob[i].tpPosition[0]-pBlob[i].yPosition[0]),NULL);
				LineTo(m_hdcDib,pBlob[i].predictedX + (pBlob[i].rpPosition[0]-pBlob[i].xPosition[0]), pBlob[i].predictedY + (pBlob[i].tpPosition[0]-pBlob[i].yPosition[0]));
				LineTo(m_hdcDib,pBlob[i].predictedX + (pBlob[i].rpPosition[0]-pBlob[i].xPosition[0]), pBlob[i].predictedY - (pBlob[i].yPosition[0]-pBlob[i].bpPosition[0]));
				LineTo(m_hdcDib,pBlob[i].predictedX - (pBlob[i].xPosition[0]-pBlob[i].lpPosition[0]), pBlob[i].predictedY - (pBlob[i].yPosition[0]-pBlob[i].bpPosition[0]));
				LineTo(m_hdcDib,pBlob[i].predictedX - (pBlob[i].xPosition[0]-pBlob[i].lpPosition[0]), pBlob[i].predictedY + (pBlob[i].tpPosition[0]-pBlob[i].yPosition[0]));
			
			
				point.x = pBlob[i].predictedX;
				point.y = pBlob[i].predictedY;
			}
			MoveToEx(m_hdcDib,point.x, point.y,NULL);
			
			int j = 0;
			while (j <10) 
			{
				if(pBlob[i].previousX[j]!= 0 || pBlob[i].previousY[j]!= 0)
				{
					point.x = pBlob[i].previousX[j];
					point.y = pBlob[i].previousY[j];
					//LineTo(m_hdcDib,point.x, point.y);
				}
				j++;
			}
		}
	}
    SelectObject(m_hdcDib,hOldFont);
	
	GetObject( m_bmpDib, sizeof(bm), &bm );
	GetDIBits( m_hdcDib, m_bmpDib, 0, m_bmi.biHeight, m_dispBuf, (BITMAPINFO*)&m_bmi, DIB_RGB_COLORS );
	
	
	DeleteObject(m_hFont);
	DeleteObject(m_bmpDib);
	DeleteDC(m_hdcDib);

	//dispNum: 1 ~ 12
	switch(m_dispMode) 
	{
	case MODE1:
		StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
			m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
		break;
	case MODE4:
		switch(dispNum) 
		{
		case 1:
			StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 2:
			StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 3:
			StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		case 4:
			StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
				m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
			break;
		default:
			break;
		}		
		break;
		case MODE9:
			switch(dispNum) {
			case 1:
				StretchDIBits(m_hdc,0,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 2:
				StretchDIBits(m_hdc,m_dispWidth,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 3:
				StretchDIBits(m_hdc,m_dispWidth*2,0,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 4:
				StretchDIBits(m_hdc,0,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 5:
				StretchDIBits(m_hdc,m_dispWidth,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 6:
				StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 7:
				StretchDIBits(m_hdc,0,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 8:
				StretchDIBits(m_hdc,m_dispWidth,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			case 9:
				StretchDIBits(m_hdc,m_dispWidth*2,m_dispHeight*2,m_dispWidth,m_dispHeight,0,0,m_bmi.biWidth,m_bmi.biHeight,
					m_dispBuf,(LPBITMAPINFO)&m_bmi,DIB_RGB_COLORS,SRCCOPY);
				break;
			default:
				break;
			}
			break;
			default:
				break;
	}
}


void CDisplay::SetDisplayCH(int ch)
{
	m_selectedCH = ch;
}

void CDisplay::SetFullScreenMode(BOOL mode)
{
	m_isFullMode = mode;
}

void CDisplay::SetBlobData(BlobInfo *pBlob,int blobNum)
{
	m_blob = pBlob;
	m_blobNum = blobNum;
}