// Avi.h: interface for the CAvi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVI_H__E41090BD_0B1A_40BF_9655_ED268C6D7A5A__INCLUDED_)
#define AFX_AVI_H__E41090BD_0B1A_40BF_9655_ED268C6D7A5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>

class CAvi  
{
public:
	CAvi();
	virtual ~CAvi();

	BOOL		IniReadAvi(char* fileName);
	BOOL		DestroyAvi();
	BOOL		GetFrame(LPBYTE pImage);

	int			GetFrameWidth() const {	return m_frameWidth; }
	int			GetFrameHeight() const { return m_frameHeight; }
	int			GetCurrentFrameNumber() const { return m_curFrame;}
	int			GetTotalFrameNumber() const { return m_totalFrameNum;}
	
private:
	int			m_frameWidth;
	int			m_frameHeight;
	int			m_curFrame;
	int			m_totalFrameNum;

	PAVISTREAM	m_pStream;
	PGETFRAME	m_pFrame;

};

#endif // !defined(AFX_AVI_H__E41090BD_0B1A_40BF_9655_ED268C6D7A5A__INCLUDED_)
