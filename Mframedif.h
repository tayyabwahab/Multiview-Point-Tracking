// Mframedif.h: interface for the CMframedif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFRAMEDIF_H__B7702A2C_4EC7_41C7_ADAD_D18A01DB4465__INCLUDED_)
#define AFX_MFRAMEDIF_H__B7702A2C_4EC7_41C7_ADAD_D18A01DB4465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Imgproc.h"


typedef struct 
{
	double m_mean[3];
	double m_stdVar[3];
	int m_validModels;
}PixelData;


class CMframedif  
{
public:
	CMframedif(int imgW, int imgH, int colorBand);
	virtual ~CMframedif();

	void		GetBgImage(BYTE *bgBuf);
	void		RunFrameDif(BYTE *inImgBuf, BYTE *maskBuf, BYTE *upDateBuf, BYTE *fgBUf);
	void		SetParam(double alpa, int threshold);

private:
	double			m_alpa;
	int				m_Threshold;
	int				m_imgWidth;
	int				m_imgHeight;
	int				m_colorBand;
	int				m_tranningNum;
	int				m_frameCount;

	BYTE*			m_curBackGroundImg;
	PixelData*		m_pixelParam;
};

#endif // !defined(AFX_MFRAMEDIF_H__B7702A2C_4EC7_41C7_ADAD_D18A01DB4465__INCLUDED_)
