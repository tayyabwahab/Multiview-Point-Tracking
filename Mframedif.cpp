// Mframedif.cpp: implementation of the CMframedif class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OBTracer.h"
#include "Mframedif.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMframedif::CMframedif(int imgW, int imgH,int colorBand)
{
	m_alpa = 0.005;
	m_Threshold = 30;
	m_colorBand = colorBand;
	m_imgWidth = imgW;
	m_imgHeight = imgH;

	m_tranningNum = 80;
	m_frameCount =0;
	
	m_pixelParam = new PixelData[imgW*imgH];
	m_curBackGroundImg = new BYTE[imgW*imgH*colorBand];
	ZeroMemory(m_curBackGroundImg,imgW*imgH*colorBand);
	
	int i,j,index;
	for(j = 0; j < imgH; j++)
	{
		index = j*imgW;
		for(i = 0; i < imgW; i++)
		{
			m_pixelParam[index+i].m_validModels = 0;
		}
	}
}

CMframedif::~CMframedif()
{
	delete [] m_pixelParam;
	delete [] m_curBackGroundImg;
}

void CMframedif::SetParam(double alpa, int threshold)
{
	m_alpa = alpa;
	m_Threshold = threshold;
}

void CMframedif::GetBgImage(BYTE *bgBuf)
{
/*	int i,j,k,index,index1;
	BYTE *tmpBG = m_curBackGroundImg;
	
	for(j = 0; j < m_imgHeight; j++)
	{
		index = j*m_imgWidth;
		index1 = index*m_colorBand;
		for(i = 0; i < m_imgWidth; i++)
		{
			for(k = 0; k < m_colorBand; k++)
			{
				bgBuf[index1+m_colorBand*i+k] = (BYTE)m_pixelParam[index+i].m_mean[k];
			}
		}
	}
*/}

void CMframedif::RunFrameDif(BYTE *inImgBuf, BYTE *maskBuf, BYTE *upDateBuf, BYTE *fgBuf)
{
	int i,j,k,index,index1;
	double tmpDif;
	bool isMatched;

	if (m_frameCount < 10) 
	{
		m_alpa = 0.5;
	}
	else
	{
		m_alpa = 0.00005;	
	}
	m_frameCount++;
	
	for(j = 0; j < m_imgHeight; j++)
	{
		index = j*m_imgWidth;
		index1 = index*m_colorBand;
		
		for(i = 0; i < m_imgWidth; i++)
		{
			if ((maskBuf[index+i] == 255)) 
				continue;

			//if no model is valid
			if (m_pixelParam[index+i].m_validModels == 0) 
			{
				for(k = 0; k < m_colorBand; k++)
				{
					m_pixelParam[index+i].m_mean[k] = (double)inImgBuf[index1+m_colorBand*i+k];
				}
				m_pixelParam[index+i].m_validModels++;
			}
			else
			{
				isMatched = false;
				for(k = 0; k < m_colorBand; k++)
				{
					tmpDif = abs(m_pixelParam[index+i].m_mean[k] - (double)inImgBuf[index1+m_colorBand*i+k]);
					
					if (tmpDif > m_Threshold) 
					{
						isMatched = true;
						fgBuf[index+i] = 255;

						if (upDateBuf[index+i] == 0) 
						{
							m_pixelParam[index+i].m_mean[k] = (1 - m_alpa)*m_pixelParam[index+i].m_mean[k] 
								+ m_alpa*inImgBuf[index1+m_colorBand*i+k];
						}
					}
					else
					{
						if (upDateBuf[index+i] == 0) 
						{
							m_pixelParam[index+i].m_mean[k] = (1 - 0.05)*m_pixelParam[index+i].m_mean[k] 
								+ 0.05*inImgBuf[index1+m_colorBand*i+k];
						}
					}					
				}

				if (!isMatched) 
				{
					fgBuf[index+i] = 0;
				}
			}
		}
	}

	MedianFilter(fgBuf, m_imgWidth,m_imgHeight,3);
	BinDilation(fgBuf,m_imgWidth,m_imgHeight,1);
//	BinErosion(fgBuf,m_imgWidth,m_imgHeight,1);
	BinDilation(fgBuf,m_imgWidth,m_imgHeight,1);
	BinErosion(fgBuf,m_imgWidth,m_imgHeight,1);
}