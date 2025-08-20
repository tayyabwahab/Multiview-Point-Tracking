// Display.h: interface for the CDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAY_H__009BF9DC_9F64_4FF5_9A00_6A6D1478C989__INCLUDED_)
#define AFX_DISPLAY_H__009BF9DC_9F64_4FF5_9A00_6A6D1478C989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Labeling.h"
#include "Tracker.h"

#define MODE1	1
#define MODE4	2
#define MODE9	3

#define WIDTHBYTE(bits)  ((bits)+31 / 32 *4 )

class CDisplay  
{
public:
	CDisplay();
	virtual ~CDisplay();
	
	void SetDisplay(HWND dispWin,int dispMode,int imgWidth, int imgHeight);
	void DrawImage(BYTE *img,int dispNum, int colorBand);
	void DrawImage(BYTE *img,int dispNum, int colorBand, char *title, int font,BlobInfo *pBlob,
				int blobNum);
    void DrawImageNew(BYTE *img,int dispNum, int colorBand, char *title,int font, BlobPast *pBlob, int blobNum, int check, char *count);
	void MiDrawText(BYTE *img, int colorBand, char *text, int font, int x, int y,COLORREF color);
	void MiDrawRect(BYTE *img, int colorBand, int top, int left, int bottom, int right, 
				COLORREF color);
	void SetBlobData(BlobInfo *pBlob,int blobNum);
	void CopyImage1DTo3D(BYTE *src, BYTE *des);
	void SetFullScreenMode(BOOL mode);	//TRUE: full screenmode
	void SetDisplayCH(int ch);
	void SetDisplayMode(int mode);
	
private:
	CRect				m_dispRect;
	HDC					m_hdc;
	HWND				m_dispWin;
	BITMAPINFOHEADER	m_bmi;
	BYTE*				m_dispBuf;
	BYTE*				m_pbDibData;
	BlobInfo*			m_blob;
	
	
	int					m_dispMode;
	int					m_dispWidth;
	int					m_dispHeight;
	int					m_selectedCH;
	BOOL				m_isFullMode;
	int					m_blobNum;

	HPEN				m_pen[10];
	COLORREF			m_color[10];
};

#endif // !defined(AFX_DISPLAY_H__009BF9DC_9F64_4FF5_9A00_6A6D1478C989__INCLUDED_)
