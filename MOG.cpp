#include "StdAfx.h"
#include <stdio.h>
#include <math.h>
#include "MOG.h"
#include "Imgproc.h"


BYTE		Buf[CHECKRANGE][320*240];

CMog::CMog(int imgWidth, int imgHeight,int colorBand)
{
	m_imgWidth = imgWidth;
	m_imgHeight = imgHeight;
	m_colorBand = colorBand;


	iThres = 3;
	iCheckFrame = 0;

	m_IniWeight = 0.005;
	m_Threshold = 0.85;
	m_Lamda = 2.7;

	//init alpa

//	m_Alpa = 0.02;
	m_Alpa = 0.7;

	
	m_IniStd = 300.0;
	m_maxStd = 500.0;
	m_RunningFrame = 0;
	m_pixelParam = new PixelPara[imgWidth*imgHeight];
	m_curBackGroundImg = new BYTE[imgWidth*imgHeight*m_colorBand];
	ZeroMemory(m_curBackGroundImg,imgWidth*imgHeight*m_colorBand);

	int i,j,n,m;
	for(j = 0; j < imgHeight; j++)
	{
		for(i = 0; i < imgWidth; i++)
		{
			m_pixelParam[j*imgWidth+i].m_validModels = 0;
			for(n = 0; n < MAXGAUNUM; n++)
			{
				m_pixelParam[j*imgWidth+i].m_weight[n] = 0.0;
				m_pixelParam[j*imgWidth+i].m_stdVar[n] = 0.0;
				m_pixelParam[j*imgWidth+i].m_priRank[n] = 0;

				for(m = 0; m < m_colorBand; m++)
				{
					m_pixelParam[j*imgWidth+i].m_mean[n][m] = 0.0;

				}
			}	
		}
	}

	PreFG = new BYTE[imgWidth*imgHeight];
	iprocessingframe = 0;

}

CMog::~CMog()
{
	delete[] m_pixelParam;
	delete[] m_curBackGroundImg;
	delete[] PreFG;
}

void CMog::GetBgImage(BYTE *bgBuf)
{
	int i,j,k;
	BYTE *tmpBG = m_curBackGroundImg;

	for(j = 0; j < m_imgHeight; j++)
	{
		for(i = 0; i < m_imgWidth; i++)
		{
			for(k = 0; k < m_colorBand; k++)
			{
				*tmpBG = (BYTE)m_pixelParam[j*m_imgWidth+i].m_mean[m_pixelParam[j*m_imgWidth+i].m_priRank[0]][k];
				tmpBG++;
			}
		}
	}
	CopyMemory(bgBuf,m_curBackGroundImg,m_imgWidth*m_imgHeight*m_colorBand);
}

void CMog::RunMog(BYTE *inImgBuf, BYTE *maskBuf, BYTE *upDateBuf, BYTE *foregroundBUf)
{
	int i,j,n,m;
	int thresholdRank;
	double tmpStd;
	double covariance,tmpCovariance;
	double fgTh;
	double pou;
	double sqrDisVector[3];
	double tmpPriority;
	bool isMatched;
	int index1,index2,index3;
	int matchedRank;
	int tmpRank;

	for(j = 20; j < m_imgHeight - 20; j++)
	{
		index1 = j*m_imgWidth;
		for(i = 0; i < m_imgWidth; i++)
		{
			index2 = m_colorBand*i;
			index3 = index1*m_colorBand;

			//no operation
			if (maskBuf[index1+i] == 0) 
			{
				foregroundBUf[index1+i] = BACKGROUND; 
			}
			else
			{
				//create the first gaussian model and initialize
				if (m_pixelParam[index1+i].m_validModels == 0) 
				{
					m_pixelParam[index1+i].m_weight[0] = m_IniWeight;
					m_pixelParam[index1+i].m_stdVar[0] = m_IniStd;
					m_pixelParam[index1+i].m_priRank[0] = 0;

					for(m = 0; m < m_colorBand; m++)
					{
						m_pixelParam[index1+i].m_mean[0][m] = (double)inImgBuf[index3+index2+m];
					}

					m_pixelParam[index1+i].m_validModels++;
				}
				else
				{
					//Find the threshold model rank
					thresholdRank = 0;
					fgTh = 0.0;

					while (true)
					{
						fgTh += m_pixelParam[index1+i].m_weight[m_pixelParam[index1+i].m_priRank[thresholdRank]];
						if ((fgTh > m_Threshold) || (thresholdRank >= m_pixelParam[index1+i].m_validModels - 1)) 
							break;		
						thresholdRank++;
					}

					isMatched = false;
					matchedRank = 0;

					for(n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
					{
						tmpRank = m_pixelParam[index1+i].m_priRank[n];
						covariance = 0.0;
						tmpCovariance = 0.0;
						tmpStd = 0.0;

						if (!isMatched) 
						{
							for(m = 0; m < m_colorBand; m++)
							{
								tmpStd = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m])
									*(m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
								covariance += tmpStd;
							}
							covariance = covariance / m_pixelParam[index1+i].m_stdVar[tmpRank];		
							covariance = sqrt(covariance);						
						}

						if ((covariance < m_Lamda) && (!isMatched))
						{
							isMatched = true;
							matchedRank = n;
							tmpCovariance = covariance;
							//update Buffer Test
							//	if (upDateBuf[index1+i] != 255) 
							m_pixelParam[index1+i].m_weight[tmpRank] = (1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank] + m_Alpa;
							pou = m_Alpa / m_pixelParam[index1+i].m_weight[tmpRank];
							tmpStd = (1 - pou) * m_pixelParam[index1+i].m_stdVar[tmpRank];

							//update Buffer Test
							//	if (upDateBuf[index1+i] != 255) 
							{
								for(m = 0; m < m_colorBand; m++)
								{
									m_pixelParam[index1+i].m_mean[tmpRank][m] = 
										(1 - pou) * m_pixelParam[index1+i].m_mean[tmpRank][m] + pou * (double)inImgBuf[index1*m_colorBand+index2+m];
									sqrDisVector[m] = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
									sqrDisVector[m] = sqrDisVector[m] * sqrDisVector[m];
									tmpStd +=  pou * sqrDisVector[m];
								}
								if (tmpStd > m_maxStd) 
								{
									m_pixelParam[index1+i].m_stdVar[tmpRank] = m_maxStd;
								}
								else
								{
									m_pixelParam[index1+i].m_stdVar[tmpRank] = tmpStd;
								}
								//update Buffer Test
							}	
						}
						else if (isMatched && (tmpCovariance > covariance)) 
						{
							isMatched = true;
							matchedRank = n;
							tmpCovariance = covariance;
							//update Buffer Test
							//	if (upDateBuf[index1+i] != 255) 
							m_pixelParam[index1+i].m_weight[tmpRank] = 
								(1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank] + m_Alpa;
							pou = m_Alpa / m_pixelParam[index1+i].m_weight[tmpRank];
							tmpStd = (1 - pou) * m_pixelParam[index1+i].m_stdVar[tmpRank];
							//update Buffer Test
							//	if (upDateBuf[index1+i] != 255)
							{
								for (m = 0; m < m_colorBand; m++)
								{
									m_pixelParam[index1+i].m_mean[tmpRank][m] = 
										(1 - pou) * m_pixelParam[index1+i].m_mean[tmpRank][m] + pou * (double)inImgBuf[index1*m_colorBand+index2+m];
									sqrDisVector[m] = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
									sqrDisVector[m] = sqrDisVector[m] * sqrDisVector[m];
									tmpStd +=  pou * sqrDisVector[m];
								}
								if (tmpStd > m_maxStd) 
								{
									m_pixelParam[index1+i].m_stdVar[tmpRank] = m_maxStd;
								}
								else
								{
									m_pixelParam[index1+i].m_stdVar[tmpRank] = tmpStd;
								}

								//update Buffer Test
							}	
						}
						else
						{
							//update Buffer Test
							//	if (upDateBuf[index1+i] != 255) 
							m_pixelParam[index1+i].m_weight[tmpRank] = 
								(1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank];					
						}
					}

					if (!isMatched) 
					{
						if (m_pixelParam[index1+i].m_validModels >= MAXGAUNUM)
						{
							/* if valid models is equal to maximum gaussian number, remove the one 
							that has the lowest priority */

							tmpRank = 
								m_pixelParam[index1+i].m_priRank[m_pixelParam[index1+i].m_validModels-1];

							m_pixelParam[index1+i].m_weight[tmpRank] = m_IniWeight;
							m_pixelParam[index1+i].m_stdVar[tmpRank] = m_IniStd;
							m_pixelParam[index1+i].m_priRank[tmpRank] = tmpRank;

							for(m = 0; m < m_colorBand; m++)
							{
								m_pixelParam[index1+i].m_mean[tmpRank][m] 
								= (double)inImgBuf[index1*m_colorBand+index2+m];
							}
						}
						else
						{
							/* Add a new model */
							tmpRank = m_pixelParam[index1+i].m_validModels;
							m_pixelParam[index1+i].m_validModels++;
							m_pixelParam[index1+i].m_weight[tmpRank] = m_IniWeight;
							m_pixelParam[index1+i].m_stdVar[tmpRank] = m_IniStd;
							m_pixelParam[index1+i].m_priRank[tmpRank] = tmpRank;

							for(m = 0; m < m_colorBand; m++)
							{
								m_pixelParam[index1+i].m_mean[tmpRank][m] 
								= (double)inImgBuf[index1*m_colorBand+index2+m];
							}
						}

						matchedRank = tmpRank;
					}

					tmpStd = 0.0;

					//normalize weight
					for (n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
					{
						tmpStd += m_pixelParam[index1+i].m_weight[n];
					}

					// calculate the order according to square of (weight/simga)
					for (n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
					{
						m_pixelParam[index1+i].m_weight[n] = m_pixelParam[index1+i].m_weight[n] / tmpStd;

						m_pixelParam[index1+i].m_priRank[n] = n;
						m_pixelParam[index1+i].m_priority[n] = 
							(m_pixelParam[index1+i].m_weight[n]) / sqrt(m_pixelParam[index1+i].m_stdVar[n]);
					}

					/* sort the gaussian models */ 
					for (n = 0; n < m_pixelParam[index1+i].m_validModels - 1; n++)
					{
						for (m = n + 1; m < m_pixelParam[index1+i].m_validModels; m++)
						{
							if (m_pixelParam[index1+i].m_priority[n] < m_pixelParam[index1+i].m_priority[m])
							{
								tmpRank = m_pixelParam[index1+i].m_priRank[n];
								m_pixelParam[index1+i].m_priRank[n] = m_pixelParam[index1+i].m_priRank[m];
								m_pixelParam[index1+i].m_priRank[m] = tmpRank;


								tmpPriority = m_pixelParam[index1+i].m_priority[n];
								m_pixelParam[index1+i].m_priority[n] = m_pixelParam[index1+i].m_priority[m];
								m_pixelParam[index1+i].m_priority[m] = tmpPriority;							
							}
						}
					}

					/* Segment foreground according to optimal model position */
					//	if(m_RunningFrame > 150)
					{
						m_Alpa = 0.001;
						if (matchedRank <= thresholdRank)
						{
							foregroundBUf[index1+i] = BACKGROUND; 
						}
						else
						{
							foregroundBUf[index1+i] = FOREGROUND; 
						}
					}
				}
			}
		}
	}

	BinDilation(foregroundBUf,m_imgWidth,m_imgHeight,1);
	BinErosion(foregroundBUf,m_imgWidth,m_imgHeight,1);
	BinDilation(foregroundBUf,m_imgWidth,m_imgHeight,1);
	BinErosion(foregroundBUf,m_imgWidth,m_imgHeight,1);
}
void CMog::STGMM(BYTE *inImgBuf, BYTE *foregroundBUf, BYTE *Background, BYTE * StableBuf)
{
	int i,j,n,m;
	int thresholdRank;
	double tmpStd;
	double covariance,tmpCovariance;
	double fgTh;
	double pou;
	double sqrDisVector[3];
	double tmpPriority;
	bool isMatched;
	int index1,index2,index3;
	int matchedRank;
	int tmpRank;

	for(j = 1; j < m_imgHeight - 1; j++)
	{
		index1 = j*m_imgWidth;
		for(i = 1; i < m_imgWidth - 1; i++)
		{
			index2 = m_colorBand*i;
			index3 = index1*m_colorBand;

			//create the first gaussian model and initialize
			if (m_pixelParam[index1+i].m_validModels == 0) 
			{
				m_pixelParam[index1+i].m_weight[0] = m_IniWeight;
				m_pixelParam[index1+i].m_stdVar[0] = m_IniStd;
				m_pixelParam[index1+i].m_priRank[0] = 0;

				for(m = 0; m < m_colorBand; m++)
				{
					m_pixelParam[index1+i].m_mean[0][m] = (double)inImgBuf[index3+index2+m];
				}
				m_pixelParam[index1+i].m_validModels++;
			}
			else
			{
				//Find the threshold model rank
				thresholdRank = 0;
		
				fgTh = 0.0;
				while (true)
				{
					fgTh += m_pixelParam[(j) * m_imgWidth + (i)].m_weight[m_pixelParam[(j) * m_imgWidth + (i)].m_priRank[thresholdRank]];					

					if ((fgTh > m_Threshold) || (thresholdRank >= m_pixelParam[(j) * m_imgWidth + (i)].m_validModels - 1)) 
					{
						break;		
					}
					thresholdRank++;
				}
				
				isMatched = false;
				matchedRank = 0;

				for(n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
				{
					tmpRank = m_pixelParam[index1+i].m_priRank[n];
					covariance = 0.0;
					tmpCovariance = 0.0;
					tmpStd = 0.0;

					if (!isMatched) 
					{
						for(m = 0; m < m_colorBand; m++)
						{
							tmpStd = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m])
								*(m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
							covariance += tmpStd;
						}
						covariance = covariance / m_pixelParam[index1+i].m_stdVar[tmpRank];		
						covariance = sqrt(covariance);						
					}

					if ((covariance < m_Lamda) && (!isMatched))
					{

						isMatched = true;
						matchedRank = n;
						tmpCovariance = covariance;

						m_pixelParam[index1+i].m_weight[tmpRank] = (1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank] + m_Alpa;
						pou = m_Alpa / m_pixelParam[index1+i].m_weight[tmpRank];
						tmpStd = (1 - pou) * m_pixelParam[index1+i].m_stdVar[tmpRank];

						for (m = 0; m < m_colorBand; m++)
						{
							m_pixelParam[index1+i].m_mean[tmpRank][m] = 
								(1 - pou) * m_pixelParam[index1+i].m_mean[tmpRank][m] + pou * (double)inImgBuf[index1*m_colorBand+index2+m];
							sqrDisVector[m] = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
							sqrDisVector[m] = sqrDisVector[m] * sqrDisVector[m];
							tmpStd +=  pou * sqrDisVector[m];
						}

						if (tmpStd > m_maxStd) 
						{
							m_pixelParam[index1+i].m_stdVar[tmpRank] = m_maxStd;
						}
						else
						{
							m_pixelParam[index1+i].m_stdVar[tmpRank] = tmpStd;
						}
					}
					else if (isMatched && (tmpCovariance > covariance)) 
					{
						isMatched = true;
						matchedRank = n;
						tmpCovariance = covariance;

						m_pixelParam[index1+i].m_weight[tmpRank] = 
							(1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank] + m_Alpa;
						pou = m_Alpa / m_pixelParam[index1+i].m_weight[tmpRank];
						tmpStd = (1 - pou) * m_pixelParam[index1+i].m_stdVar[tmpRank];

						for (m = 0; m < m_colorBand; m++)
						{
							m_pixelParam[index1+i].m_mean[tmpRank][m] = 
								(1 - pou) * m_pixelParam[index1+i].m_mean[tmpRank][m] + pou * (double)inImgBuf[index1*m_colorBand+index2+m];
							sqrDisVector[m] = (m_pixelParam[index1+i].m_mean[tmpRank][m] - (double)inImgBuf[index1*m_colorBand+index2+m]);
							sqrDisVector[m] = sqrDisVector[m] * sqrDisVector[m];
							tmpStd +=  pou * sqrDisVector[m];
						}

						if (tmpStd > m_maxStd) 
						{
							m_pixelParam[index1+i].m_stdVar[tmpRank] = m_maxStd;
						}
						else
						{
							m_pixelParam[index1+i].m_stdVar[tmpRank] = tmpStd;
						}
					}
					else
					{
						m_pixelParam[index1+i].m_weight[tmpRank] = 
							(1 - m_Alpa) * m_pixelParam[index1+i].m_weight[tmpRank];					
					}
				}

				if (!isMatched) 
				{
					if (m_pixelParam[index1+i].m_validModels >= MAXGAUNUM)
					{
						/* if valid models is equal to maximum gaussian number, remove the one 
						that has the lowest priority */

						tmpRank = 
							m_pixelParam[index1+i].m_priRank[m_pixelParam[index1+i].m_validModels-1];

						m_pixelParam[index1+i].m_weight[tmpRank] = m_IniWeight;
						m_pixelParam[index1+i].m_stdVar[tmpRank] = m_IniStd;
						m_pixelParam[index1+i].m_priRank[tmpRank] = tmpRank;

						for(m = 0; m < m_colorBand; m++)
						{
							m_pixelParam[index1+i].m_mean[tmpRank][m] 
							= (double)inImgBuf[index1*m_colorBand+index2+m];
						}
					}
					else
					{
						/* Add a new model */
						tmpRank = m_pixelParam[index1+i].m_validModels;
						m_pixelParam[index1+i].m_validModels++;
						m_pixelParam[index1+i].m_weight[tmpRank] = m_IniWeight;
						m_pixelParam[index1+i].m_stdVar[tmpRank] = m_IniStd;
						m_pixelParam[index1+i].m_priRank[tmpRank] = tmpRank;

						for(m = 0; m < m_colorBand; m++)
						{
							m_pixelParam[index1+i].m_mean[tmpRank][m] 
							= (double)inImgBuf[index1*m_colorBand+index2+m];
						}
					}

					matchedRank = tmpRank;
				}

				tmpStd = 0.0;
				for (n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
				{
					tmpStd += m_pixelParam[index1+i].m_weight[n];
				}

			
				for (n = 0; n < m_pixelParam[index1+i].m_validModels; n++)
				{
					m_pixelParam[index1+i].m_weight[n] = m_pixelParam[index1+i].m_weight[n] / tmpStd;

					m_pixelParam[index1+i].m_priRank[n] = n;
					m_pixelParam[index1+i].m_priority[n] = 
						(m_pixelParam[index1+i].m_weight[n]) / sqrt(m_pixelParam[index1+i].m_stdVar[n]);
				}

				for (n = 0; n < m_pixelParam[index1+i].m_validModels - 1; n++)
				{
					for (m = n + 1; m < m_pixelParam[index1+i].m_validModels; m++)
					{
						if (m_pixelParam[index1+i].m_priority[n] < m_pixelParam[index1+i].m_priority[m])
						{
							tmpRank = m_pixelParam[index1+i].m_priRank[n];
							m_pixelParam[index1+i].m_priRank[n] = m_pixelParam[index1+i].m_priRank[m];
							m_pixelParam[index1+i].m_priRank[m] = tmpRank;


							tmpPriority = m_pixelParam[index1+i].m_priority[n];
							m_pixelParam[index1+i].m_priority[n] = m_pixelParam[index1+i].m_priority[m];
							m_pixelParam[index1+i].m_priority[m] = tmpPriority;							
						}
					}
				}

				if (matchedRank <= thresholdRank)
				{
					foregroundBUf[index1+i] = BACKGROUND; 
				}
				else
				{
					foregroundBUf[index1+i] = FOREGROUND; 
				}

				StableBuf[index1+i] = 0;
				int check=0;
				int chectfg=0;
				int chectgbg=0;
				
				int iSumValue=0;
				int iChecksumvlaue = 0;

				if(PreFG[index1+i] != foregroundBUf[index1+i])
				{
					Buf[iprocessingframe][index1+i] = 1;
				}
				else if(PreFG[index1+i] == foregroundBUf[index1+i])
				{
					Buf[iprocessingframe][index1+i] = 0;
				}
				if(foregroundBUf[index1+i] == FOREGROUND)
				{	
					for(int c = 0 ;  c < CHECKRANGE ; c++)
					{
						iSumValue += Buf[c][index1+i];
					}
					if(iSumValue >= iThres)
					{
						chectgbg = 0;
						chectfg = 0;
						StableBuf[index1+i] = 255;
						for(int x = -1 ; x < 2 ; x++)
						{
							for(int y = -1 ; y < 2 ; y++)
							{
								iChecksumvlaue = 0;
								for(int u = 0 ; u < CHECKRANGE ; u++)
								{
									iChecksumvlaue = Buf[u][(j+x) * m_imgWidth + (i+y)];
								}
								if(iChecksumvlaue < iThres)
								{
									if(foregroundBUf[(j+x) * m_imgWidth + (i+y)] == BACKGROUND)
										chectgbg++;
									else if(foregroundBUf[(j+x) * m_imgWidth + (i+y)] == FOREGROUND)
										chectfg++;
								}
							}
						}
						if(chectgbg > chectfg)
							foregroundBUf[index1+i] = BACKGROUND;
						else if(chectfg > chectgbg)
							foregroundBUf[index1+i] = FOREGROUND;
						else if(chectfg == chectgbg)
							foregroundBUf[index1+i] = FOREGROUND;
					}
				}
				if(iprocessingframe == CHECKRANGE)
				{
					iprocessingframe = 0;
				}
				iprocessingframe++;
				PreFG[index1+i] = foregroundBUf[index1+i];
			}
		}
	}

	MedianFilter(foregroundBUf,m_imgWidth, m_imgHeight,3);
}