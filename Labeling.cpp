// Tracker.cpp: implementation of the CTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OBTracer.h"
#include "Labeling.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPosQueue::CPosQueue()
{
	m_iHeader = m_iTail = 0;
	m_iQueueSize = HISTORY_NUM;
}

CPosQueue::~CPosQueue()
{
}

void CPosQueue::SetQueue(int Qsize)
{
	m_iQueueSize = Qsize;
}

void CPosQueue::ClearAllData()
{
	m_iHeader = m_iTail = 0;
}

BOOL CPosQueue::GetPosition(CPoint *point)
{
	CPoint tmpPoint;

	if ((m_iTail) > 0) 
	{
		point->x = m_Queue[m_iTail % m_iQueueSize].x;
		point->y = m_Queue[m_iTail % m_iQueueSize].y;
		m_iTail--;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPosQueue::PutPosition(CPoint point)
{	
	m_iHeader++;
	m_iTail = m_iHeader;
	m_Queue[((m_iHeader) % m_iQueueSize)].x = point.x;
	m_Queue[((m_iHeader) % m_iQueueSize)].y = point.y;

	return TRUE;	
}

BOOL CPosQueue::PutPosition(int x, int y)
{	
	m_iHeader++;
	m_iTail = m_iHeader;
	m_Queue[((m_iHeader) % m_iQueueSize)].x = x;
	m_Queue[((m_iHeader) % m_iQueueSize)].y = y;

	return TRUE;	
}

CLabeling::CLabeling(int imgWidth, int imgHeight)
{
	int i;
	double wGap,hGap;


	m_imgWidth = imgWidth;
	m_imgHeight = imgHeight;

	m_tranningNum = 80;
	m_frameCount = 0;


	m_labelBuf = new DWORD[imgWidth*imgHeight*sizeof(DWORD)];

	sizeBuf = new int[imgHeight];



	m_iFilteringWidth = new int[imgHeight];
	m_iFilteringHeight = new int[imgHeight];
	m_iFilteringSize = new int[imgHeight];

	int iIndex = 0;
	int iCountOfIndex = 0;

	/*int iThresHoldWidt[30] = {3,10,10,10,10,10,14,15,17,17,18,20,23,25,30,28,28,28,18,15,13,10, 9,8,7, 6, 5, 5, 5, 5};
	int iThresHoldHeigth[30] = {3,15,15,18,20,21,25,25,25,31,31,31,35,35,35,35,25,20,15,15,10,10,10,10,10, 10, 10, 10,10};

	
	for(i = 0; i < imgHeight; i++)
	{
		m_iFilteringWidth[i] = iThresHoldWidt[iIndex];
		m_iFilteringHeight[i] = iThresHoldHeigth[iIndex];
		sizeBuf[i] = m_iFilteringWidth[i] * m_iFilteringHeight[i];
		TRACE("%d : %d * %d = %d\r\n", i,m_iFilteringWidth[i], m_iFilteringHeight[i],  sizeBuf[i]);
		iCountOfIndex++;
		if(iCountOfIndex >= 11)
		{
			iCountOfIndex = 0;
			iIndex++;
		}
	}*/

	int iThresHoldWidt[30] = {18,18,23,18,17,16,15,13,11,10,10,10,10,8,8,7,7,5,3,3,3,3,1,1,1};
	int iThresHoldHeigth[30] = {8,13,16,28,38,53,58,58,53,51,51,48,41,38,36,33,28,26,25,23,18,13,11,8,3};

	for(i = 0; i < imgHeight; i++)
	{
		m_iFilteringWidth[i] = iThresHoldWidt[iIndex];
		m_iFilteringHeight[i] = iThresHoldHeigth[iIndex];
		sizeBuf[i] = m_iFilteringWidth[i] * m_iFilteringHeight[i];
	
		iCountOfIndex++;
		if(iCountOfIndex >= 10)
		{
			iCountOfIndex = 0;
			iIndex++;
		}
	}

}

CLabeling::~CLabeling()
{
	delete [] m_labelBuf;
	delete [] sizeBuf;
	delete []m_iFilteringWidth;
	delete []m_iFilteringHeight;
	delete []m_iFilteringSize;
}

int CLabeling::Labeling(BYTE* imgBuf,BYTE* binBuf,int minSize, int maxSize, int frameCnt, BlobInfo *pBlob)
{
	int i,j,n,m,index;
	DWORD label = 1;	
	int *stackX, *stackY;
	int top = 0;
	int x,y;
	DWORD *tmpSize;
	int *minX,*maxX;
	int *minY,*maxY;	

	stackX = new int[m_imgWidth*m_imgHeight];
	stackY = new int[m_imgWidth*m_imgHeight];
	tmpSize = new DWORD[m_imgWidth*m_imgHeight];
	minX = new int[m_imgWidth*m_imgHeight];
	maxX = new int[m_imgWidth*m_imgHeight];
	minY = new int[m_imgWidth*m_imgHeight];
	maxY = new int[m_imgWidth*m_imgHeight];	

	//Initialize variables
	x = y = 0;	
	for(j = 0; j < m_imgHeight; j++)
	{
		index = j*m_imgWidth;
		for(i = 0; i < m_imgWidth; i++)
		{
			m_labelBuf[index + i] = 0;
			tmpSize[index + i] = 0;
			minX[index + i] = m_imgWidth;
			maxX[index + i] = 0;
			minY[index + i] = m_imgHeight;
			maxY[index + i] = 0;
		}
	}

	for(j = 1; j < m_imgHeight - 1; j++)
	{
		for(i = 1; i < m_imgWidth - 1; i++)
		{
			if ((binBuf[j * m_imgWidth + i] > 0) && (m_labelBuf[j * m_imgWidth+ i] == 0)) 
			{
				Push(stackX,stackY,&top,i,j);


				while (Pop(stackX,stackY,&top,&x,&y) != -1)
				{
					for(n = y-1; n <= y+1; n++)
					{
						index = n*m_imgWidth;
						for(m = x-1; m <= x+1; m++)
						{								
							if ((binBuf[index + m] > 0) && (m_labelBuf[index + m] == 0)) 
							{
								m_labelBuf[index + m] = label;
								tmpSize[m_labelBuf[index + m]]++;

								if (minX[m_labelBuf[index + m]] > m) 
								{
									minX[m_labelBuf[index + m]] = m;
								}

								if (maxX[m_labelBuf[index + m]] < m) 
								{
									maxX[m_labelBuf[index + m]] = m;
								}

								if (minY[m_labelBuf[index + m]] > n) 
								{
									minY[m_labelBuf[index + m]] = n;
								}

								if (maxY[m_labelBuf[index + m]] < n) 
								{
									maxY[m_labelBuf[index + m]] = n;
								}

								if ((n > 0) && (m > 0) && (m < m_imgWidth - 1) && (n < m_imgHeight - 1)) 
								{
									Push(stackX,stackY,&top,m,n);	
								}
							}
						}
					}
				}
				label++;
			}
		}
	}

	int filteringNum = 0;
	DWORD *filteringLabel;
	int tempSize;
	filteringLabel = new DWORD[label];
	m_curBlobNum = 0;

	//Size filtering
	for(i = 1; i < label; i++)
	{
		tempSize = (maxX[i]-minX[i])*(maxY[i]-minY[i]);
		if ((tempSize < sizeBuf[(minY[i]+maxY[i])/2]))
		{
			filteringNum++;
			filteringLabel[filteringNum] = i;
		}
		else
		{
			pBlob[m_curBlobNum].isMatched = FALSE;
			pBlob[m_curBlobNum].matchingCnt = 0;
			pBlob[m_curBlobNum].label = i;
			pBlob[m_curBlobNum].size = tmpSize[i];
			pBlob[m_curBlobNum].rect.top = minY[i];
			pBlob[m_curBlobNum].rect.bottom = maxY[i];
			pBlob[m_curBlobNum].rect.left = minX[i];
			pBlob[m_curBlobNum].rect.right = maxX[i];
			pBlob[m_curBlobNum].centerPos.x = (minX[i]+maxX[i])/2;
			pBlob[m_curBlobNum].centerPos.y = (minY[i]+maxY[i])/2;
			pBlob[m_curBlobNum].appearTime = GetBlobTime();
			double dBlobRatio = (double)tempSize / (double)tmpSize[i];
			pBlob[m_curBlobNum].iKindOfBlob = KindOfBlob(tmpSize[i], minY[i],dBlobRatio);
			m_curBlobNum++;
		}
	}

	for(n = 0; n < m_imgHeight; n++)
	{
		index = n*m_imgWidth;
		for(m = 0; m < m_imgWidth; m++)
		{
			if (binBuf[index + m] > 0) 
			{
				for(i = 1; i <= filteringNum; i++)
				{
					if (m_labelBuf[index + m] == filteringLabel[i])
					{
						m_labelBuf[index + m] = 0;
						binBuf[index + m] = 0;
					}
				}
			}
		}
	}
	delete []stackX;
	delete []stackY;
	delete []tmpSize;
	delete []filteringLabel;
	delete []minX;
	delete []maxX;
	delete []minY;
	delete []maxY;	

	return m_curBlobNum;	
}

double CLabeling::GetBlobTime()
{
	SYSTEMTIME sm;
	FILETIME fm;

	GetSystemTime(&sm);
	SystemTimeToFileTime(&sm,&fm);

	return (double)(fm.dwLowDateTime / 10000000.0);
}
int CLabeling::KindOfBlob(int Size, int Height,double Ratio)
{
	int bReturn = 1;
	if(Height >= 0 && Height < 30)
	{
		if(Size >= 0 && Size < 1000)
		{
			if(Ratio < 1.2)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 1000 && Size < 2000)
		{
			if(Ratio < 1.2)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else if(Height >= 30 && Height < 60)
	{
		if(Size >= 1000)
		{
			bReturn = PERSON;
		}
		else if(Size >= 1000 && Size < 2300)
		{
			if(Ratio < 1.6)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 2500 && Size < 2800)
		{
			if(Ratio < 1.6)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else if(Height >= 60 && Height < 90)
	{
		if(Size >= 1000)
		{
			bReturn = PERSON;
		}
		else if(Size >= 0 && Size < 2200)
		{
			if(Ratio < 1.3)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 2200 && Size < 4700)
		{
			if(Ratio < 1.3)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else if(Height >= 90 && Height < 120)
	{
		if(Size >= 1300)
		{
			bReturn = PERSON;
		}
		else if(Size >= 0 && Size < 6000)
		{
			if(Ratio < 1.3)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 6000 && Size < 8500)
		{
			if(Ratio < 1.3)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else if(Height >= 120 && Height < 150)
	{
		if(Size >= 0 && Size < 5500)
		{
			if(Ratio < 1.25)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 5500 && Size < 6000)
		{
			if(Ratio < 1.25)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else if(Height >= 150 && Height < 180)
	{
		if(Size >= 0 && Size < 1000)
		{
			bReturn = PERSON;
		}
		else if(Size >= 1000 && Size < 3900)
		{
			if(Ratio < 1.25)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else if(Size >= 3900 && Size < 4000)
		{
			if(Ratio < 1.3)
				bReturn = VEHICLE;
			else
				bReturn = PERSONS;
		}
		else
		{
			bReturn = VEHICLE;
		}
	}
	else
	{
		if(Size >= 0 && Size < 700)
		{
			bReturn = PERSON;
		}
		else if(Size >= 700 && Size < 1500)
		{
			if(Ratio < 1.25)
				bReturn = PERSON;
			else
				bReturn = PERSONS;
		}
		else
		{
			if(Ratio < 1.25)
				bReturn = PERSON;
			else
				bReturn = VEHICLE;
		}
	}
	return bReturn;
}
void  CLabeling::SobelEdge1(BYTE *src, BYTE *edge,int W, int H, int x, int y,  BYTE *mask)
{
	int i,j,index,index1,index2;

	ZeroMemory(edge,W*H);

	for(j = 10 ; j < H-10 ; j++)
	{
		index = j*W;
		index1 = (j-1)*W;
		index2 = (j+1)*W;
		for(i = 10 ; i < W-10 ; i++)
		{
			if(mask[(j + y)*W + (i+x)] == 255)
			{

				edge[index + i] = abs(src[index1 + i - 1] + 2*src[index + i - 1] + 
					src[index2 + i - 1] - src[index1 + i + 1] - 
					2*src[index + i + 1] - src[index2 + i + 1]) / 4;

				edge[index + i] += abs(src[index1 + i - 1] + 2*src[index1 + i] + 
					src[index1+ i + 1] - src[index2 + i - 1] - 
					2*src[index2 + i] - src[index2 + i + 1]) / 4;

				if(edge[index + i] >= 50)
					edge[index + i] = 255;
				else
					edge[index + i] = 0;
			}
			else
				edge[index + i] = 0;

		}
	}

}


bool CLabeling:: horizontalMergeDetection(BYTE * imgBuf,BYTE* fgBuf, int m_Width, int m_Height, int x, int y, int left, int right, int* count, std::vector<int>* positions) 
{
	BYTE* graybuf	=  new BYTE[m_Width * m_Height];
	BYTE* edgeBuff  =  new BYTE[m_Width * m_Height*3];
	BYTE* bEdgeProjectionMap =  new BYTE[m_Width * m_Height];
	BYTE* sobelEdge =  new BYTE[m_Width * m_Height];

	for(int j = 0 ; j < m_Height ; j++)
	{
		for(int i = 0 ; i < m_Width ; i++)
		{

			graybuf[j * m_Width + i] = imgBuf[(m_Height - (j + y))* 3  * m_Width + (i + x) * 3];
		}
	}

	SobelEdge1(graybuf, sobelEdge, m_Width , m_Height,x, y, fgBuf);

	MedianFilter(sobelEdge, m_Width , m_Height,  3);

	for(int i = 0 ; i < m_Width ; i++)
	{

		int iSumOfEdge=0;
		for(int j = 0 ; j < m_Width ; j++) 
		{

			if(sobelEdge[j * m_Width+i] == 255  && fgBuf[(j + y) * m_Width+(i + x)] == 255)
				iSumOfEdge++;
		}

		for(int j =  0; j < iSumOfEdge*5 ; j++) {
			bEdgeProjectionMap[j * m_Width + i] = 255;
		}
	}

	int index=0;
	int * iDividePoint;
	iDividePoint = new int [20];

	for(int i = 1 ; i < m_Width ; i++) 
	{

		if(bEdgeProjectionMap[m_Width + i] != bEdgeProjectionMap[m_Width + i - 1]) {
			iDividePoint[index] = i;
			index++;

		}
	}


	if(index > 3) 
	{
		int * iDiv = new int[(index/2)];
		int index2 = 0 ;
		boolean iDivident = false;

		for(int i = 1; i <=(index/2); i+=2 )
		{   //getting the divident point between two points
			iDiv[index2] = 0;
			if((iDividePoint[i+1] - iDividePoint[i]) < 22  && (iDividePoint[i+1] - iDividePoint[i]) > 1)
			{
				iDiv[index2] = iDividePoint[i] + ((iDividePoint[i+1] - iDividePoint[i]) / 2);
				if(iDiv[index2] > 0) {
					iDivident = true; index2 ++;
				}
			}
		}

//		if(iDivident && (m_currentBlob_number > 0 || m_curBlobNum > 0))
		{	
			bool merged = false;
			merged =  cutTheMergedBlobs(index2, iDiv,m_Width, m_Height,left, right, bEdgeProjectionMap, count, positions);
		}
		delete [] iDiv;

	}


	delete [] iDividePoint;
	delete [] graybuf;
	delete [] edgeBuff;
	delete [] bEdgeProjectionMap;
	delete [] sobelEdge;

//	return merged;
	return 0;
}


bool CLabeling::cutTheMergedBlobs(int index2, int * iDiv,int width, int height, int left, int right, BYTE* bEdgeProjectionMap, int* count, std::vector<int>* positions  )
{

	int j = 0, i = 0, peakOne = 0 , peakTwo = 0, peakHeightOne = 0 , peakHeighttwo = 0, l = 0, p = 0;
	for( i =  0; i <width ; i++) 
	{

		for(j = 0 ; j< index2; j ++) 
		{ //take care of all the divident points in iDiv.

			if(iDiv[j] != 0 && i == iDiv[j] )
			{

				peakOne = i - left; peakOne = abs(peakOne);						// //width of peak one
				peakTwo = right - i; peakTwo = abs(peakTwo);					    // //width of peak two
				peakHeightOne = 0; peakHeighttwo = 0;

				for(l = 0 ; l < height ; l++) 
				{						////for height of peak one
					for( p = left; p < i; p++ )
					{  

						//bEdgeProjectionMap[p +m_imgWidth * l] = 255;
						if(bEdgeProjectionMap[l *width+ p] == 255)
						{
							if(l > peakHeightOne) 
								peakHeightOne = l; //bEdgeProjectionMap[l *m_imgWidth+ p] = 120;
						} 
					}
				}

				for(l = 0  ; l < height ; l++)
				{						////for height of peak two
					for( p = i; p <= right; p++ ) 
					{  

						if(bEdgeProjectionMap[l * width+ p] == 255)
						{
							if(l > peakHeighttwo)
								peakHeighttwo = l;
						}
					}
				}
				if(  ((peakHeightOne > peakHeighttwo && peakHeighttwo >= (peakHeightOne/2) ) || ( peakHeighttwo > peakHeightOne &&  peakHeightOne >= (peakHeighttwo/2) ))  
					&&  ( (peakOne > peakTwo && peakTwo >= (peakOne/2) ) || ( peakTwo > peakOne &&  peakOne >= (peakTwo/2) ) )  )
				{ 
					positions->push_back(i);
					count++; 
				//	merged = true;
				}
			}
		}
	}
	return 0;
}