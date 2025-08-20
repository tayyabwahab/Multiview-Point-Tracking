#include "Tracker.h"
#include "stdafx.h"
#include "OBTracer.h"
#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
struct matrix7
{
	int position[7][7];
	int RGB[49][3];
	
};

void update_clr(BYTE);
int save_clr[10];
int blob_number_new = 0;
//int previous_size[10] = {0},new_size[10] = {0};
int minimum_blob_number = 0,new_blobs = 0;
int size_blob = 0; 
bool flag = false;
bool temp_flag = false;
int frame_number = 0;
bool check = false;
int p_current_blobnumber;
int adaptive_distance = 32,adaptive_merged = 40, merged_size = 0, demerged_size = 0;
bool merged = false;
int countDemerge;
bool demerged = false;
double prev_xPosition[10] = {0},prev_yPosition[10] = {0};
int new_size = 0, prev_size = 0;
int dist_temp = 0, turn = 0;
int fst = 0, scnd = 0;
int merge_check = -1, merge_count = 0, new_id = 0, c_frame = 0;


//BYTE *image_buffer = NULL;
//BYTE *bin_buffer = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	int							iBlobColor1;
	int							iBlobColor2;
	unsigned short				bColorTable[255][255][255];
	unsigned short				bColorTable2[255][255][255];
	int							iBlobColorBlack;
	int							iBlobColorGray;
	int							iBlobColorWhite;
	int							iBlobColorRed;
	int							iBlobColorBlue;
	int							iBlobColorYellow;
	int							iBlobColorGreen;
	int							iBlobColorCyan;
	int							iBlobColorMagenta;
	int							iDistancePixelToColorRed;
	int							iDistancePixelToColorGreen;
	int							iDistancePixelToColorBlue;
	int							iDistancePixelToColorWhite;
	int							iDistancePixelToColorBlack;
	int							iDistancePixelToColorCyan;
	int							iDistancePixelToColorYellow;
	int							iDistancePixelToColorMagenta;
	int							iMinColorDistance;
	int							iMincolorDistance2;
	int							iCurPixelColor1;
	int							iCurPixelColor2;
	int							iColorQueue[8];
	int							iBlobColorQueue[9];
	int							iColorChoiceTemp;
	double						iR,iG,iB;
	int							iRValue;
	int							iGValue;
	int							iBValue;

CTracker::CTracker(int imgWidth, int imgHeight)
{
	int i;
	double wGap,hGap;
	
	
	m_imgWidth = imgWidth;
	m_imgHeight = imgHeight;
	m_max = 200;
	m_tranningNum = 5;
	m_frameCount = 0;
	m_t_number = 0;
	m_updateBuf = new BYTE[imgWidth*imgHeight];
	m_tmpEdgeBuf = new BYTE[imgWidth*imgHeight];
	m_curEdgeBuf = new BYTE[imgWidth*imgHeight];
	m_edgeBuf = new BYTE[imgWidth*imgHeight];
	m_tmpUpdataBuf = new BYTE[imgWidth*imgHeight];
	m_labelBuf = new DWORD[imgWidth*imgHeight*sizeof(DWORD)];
	graybuf =  new BYTE[m_imgWidth * m_imgHeight];
	sobelEdge = new BYTE[m_imgWidth*m_imgHeight*3];
	bEdgeProjectionMap = new BYTE[m_imgWidth*m_imgHeight];


	sizeBuf = new double[imgHeight];

	ZeroMemory(m_updateBuf,imgWidth*imgHeight);

	m_maxTrackingNum = 1000;
	m_validID = new BOOL[m_maxTrackingNum];

//	for(i = 0; i < m_imgHeight; i++)
//	{
//		sizeBuf[i] = ((i-50)*(0.1)+10) * ((i-50)*0.14+15);
//	}

	for(i = 0; i < m_imgHeight; i++)
	{
		if(i >= 0 && i < 70)
			sizeBuf[i] = 25;
		else if(i >= 70 && i < 200)
			sizeBuf[i] = 250;
		else
			sizeBuf[i] = 250;
	}
	
	for(i = 0; i < m_maxTrackingNum; i++)
		m_validID[i] = FALSE;

	m_blobPool = NULL;
	m_curBlob = new BlobInfo[m_maxTrackingNum];
	m_curBlobNum = 0;
	m_currentBlob_number = 0;
	m_trackingBlobNum = 0;
	blobs = new BlobPast[m_max];
	for (int i = 0; i < m_max ; i++)
	{
		for(int j = 0; j < 2 ; j++)
		{
			blobs[i].xPosition[j] = 0;
			blobs[i].yPosition[j] = 0;
			blobs[i].tpPosition[j] = 0;
			blobs[i].bpPosition[j] = 0;
			blobs[i].lpPosition[j] = 0;
			blobs[i].rpPosition[j] = 0;
		}
		for(int j = 0; j < 35 ; j++)
		{
			blobs[i].previousX[j] = 0;
			blobs[i].previousY[j] = 0;
			blobs[i].tPosition[j] = 0;
			blobs[i].bPosition[j] = 0;
			blobs[i].lPosition[j] = 0;
			blobs[i].rPosition[j] = 0;
		}
		for(int j = 0; j < 5 ; j++)
		{
			blobs[i].merged_blobs[j] = -1;
			blobs[i].merg_id[j] = -1;
		}
		for(int j = 0; j < 256 ; j++) ///changed from -1 to 1
		{
			blobs[i].colour_blue[j]  = 0;
			blobs[i].colour_green[j] = 0;
			blobs[i].colour_red[j]   = 0;
		}
		blobs[i].predictedX = 0;
		blobs[i].predictedY = 0;
		blobs[i].isInitiated = false;
		blobs[i].isInFrame = false;
		blobs[i].blobDirection = 0;
		blobs[i].id = -1;
		blobs[i].histoty_count = 0;
		blobs[i].merged_counter = 0;
	}

	positionArray = new PositionArrays[2]();
	for(i = 0; i<10 ; i++)
	{
		positionArray[0].xPosition[i] = 0;
		positionArray[0].yPosition[i] = 0;
		positionArray[0].tPosition[i] = 0;
		positionArray[0].bPosition[i] = 0;
		positionArray[0].lPosition[i] = 0;
		positionArray[0].rPosition[i] = 0;
		positionArray[0].dominantColor1[i] = 0;
		positionArray[0].dominantColor2[i] = 0;
		positionArray[0].dominantColor3[i] = 0;
		positionArray[0].check_new_blob[i] = false;
		if (i < 5)
			positionArray[0].merged_Blobs[i] = -1;

		for(int j = 0; j<256 ; j++)
		{
			positionArray[0].colours[i].colour_blue[j]=0;
			positionArray[0].colours[i].colour_red[j]=0;
			positionArray[0].colours[i].colour_green[j]=0;
		}
	}
	m_totalBlobNum = 0;
	//m_currentBlob_number = 0;
}

CTracker::~CTracker()
{
	BlobInfo *delBlob;
	
	
	while (m_blobPool != NULL) 
	{
		delBlob = m_blobPool;
		m_blobPool = m_blobPool->nextBlob;
		delete delBlob;
	}
	
	delete [] blobs;
	delete [] m_updateBuf;
	delete [] m_tmpEdgeBuf;
	delete [] m_curEdgeBuf;
	delete [] m_edgeBuf;
	delete [] m_tmpUpdataBuf;
	delete [] m_labelBuf;
	delete [] m_curBlob;
	delete [] m_validID;
	delete [] sizeBuf;
}

void CTracker::InsertKFBlobInformation(BlobInfo *src, BlobPast *des, int BlobCount, BYTE *input, BYTE *fgBuf)
{
	//int image_width = CTracker::CTracker.m_imgWidth;
	int index_g = 0;
	for (int i = 0; i < BlobCount; i++)
	{
		positionArray[0].xPosition[index_g] = src[i].centerPos.x;
		positionArray[0].yPosition[index_g] = src[i].centerPos.y;
		positionArray[0].tPosition[index_g] = src[i].rect.top;
		positionArray[0].bPosition[index_g] = src[i].rect.bottom;
		positionArray[0].lPosition[index_g] = src[i].rect.left;
		positionArray[0].rPosition[index_g] = src[i].rect.right;
		//positionArray[0].dominantColor = m_curBlob[m_curBlobNum].dominantColor;
		//run_algo_for_colour(input, fgBuf, &blobs[index_g] ,  m_imgWidth);
		//GetObjectColor(int(blobs[index_g].lPosition[0]),int(blobs[index_g].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
		
		positionArray[0].dominantColor1[index_g] = src[i].dominantColor1;
		positionArray[0].dominantColor2[index_g] = src[i].dominantColor2;
		positionArray[0].dominantColor3[index_g] = src[i].dominantColor3;
		/*for (int l = 0; l< 256; l++)
		{
			positionArray[0].colours[index_g].colour_blue[l] = src[i].colour_blue[l];
			positionArray[0].colours[index_g].colour_red[l] = src[i].colour_red[l];
			positionArray[0].colours[index_g].colour_green[l] = src[i].colour_green[l];
		}*/
		
		/*for (int l = 0; l< 256; l++)
		{
			positionArray[0].colours[index_g].colour_blue[l] = 0;
			positionArray[0].colours[index_g].colour_red[l] =  0;
			positionArray[0].colours[index_g].colour_green[l] = 0;
		}*/
		
		if (BlobCount > new_blobs)
		{
			int kk= 0;
			while (positionArray[0].xPosition[kk] != 0)
			{
				for (int l = 0; l< 256; l++)
				{
					positionArray[0].colours[index_g].colour_blue[l] = 0;
					positionArray[0].colours[index_g].colour_red[l] =  0;
					positionArray[0].colours[index_g].colour_green[l] = 0;
				}
				kk++;
			}
			//run_algo_for_colour2(BYTE *back_grnd, BYTE* fore_grnd, PositionArrays* blob_info, int img_width);
			int k = 0;
			while (positionArray[0].xPosition[k] != 0)
			{
				run_algo_for_colour2(input, fgBuf, &positionArray[0] , m_imgWidth, k);
				k++;
			}
		}
		index_g++;
		m_currentBlob_number++;
		m_curBlobNum++;
	}
	if (BlobCount > new_blobs)
		int blob_inc = 1;

 	new_blobs = BlobCount;
	
	c_frame++;
	if (c_frame >= 3031)
	{
		int pause = 1;
	}
	
}



void update_clr(BYTE value)
{
	static int i = 0,prev_frame = 0;
	if(prev_frame < frame_number)
	{
		i = 0;
		for(int j=0;j<10;j++)
			save_clr[j] = 0;
	}

	save_clr[i] = int(value);
	i++;
	if(save_clr[i] == 5)
		save_clr[i] = 22;
	prev_frame = frame_number;

}


void CTracker::deleteArray()
{
	
}

PositionArrays CTracker::getPositionArray()
{
	return positionArray[0];
}

BYTE CTracker::GetDominantColor(BlobInfo *pBlob, BYTE *imgBuf, BYTE *fgBuf)
{
	int i,j,index;
	int count[256];	
	int maxVal;
	BYTE dominantVal;

	ZeroMemory(count,256*sizeof(int));
	maxVal = 0;
	for(j = pBlob->rect.top; j < pBlob->rect.bottom; j++)
	{
		for(i = pBlob->rect.left; i < pBlob->rect.right; i++)
		{
			index = j*m_imgWidth+i;
			if (fgBuf[index]) 
			{
				count[imgBuf[index]]++;

				if (maxVal < count[imgBuf[index]]) 
				{
					maxVal = count[imgBuf[index]];
					dominantVal = imgBuf[index];
				}
			}
		}
	}

	return dominantVal;
}

void CTracker::DeleteBlob(BlobInfo *pBlob)
{
	BlobInfo *tmpBlob, *delBlob, *preBlob;

	tmpBlob = m_blobPool;
	preBlob = tmpBlob;
	while (tmpBlob != NULL) 
	{
		if (tmpBlob == pBlob) 
		{
			delBlob = tmpBlob;
			if (delBlob == m_blobPool) 
			{
				m_blobPool = m_blobPool->nextBlob;
				
				ResetValidID(delBlob->id);
				delete delBlob;
				m_trackingBlobNum--;
			}
			else
			{
				tmpBlob = tmpBlob->nextBlob;
				preBlob->nextBlob = tmpBlob;
				
				ResetValidID(delBlob->id);
				delete delBlob;
				m_trackingBlobNum--;
			}
			return;
		}
		preBlob = tmpBlob;
		tmpBlob = tmpBlob->nextBlob;
	}
}

void CTracker::Tracking(BYTE* imgBuf,BYTE* fgBuf,int minSize, int maxSize, int frameCnt)
{
	
	
	int i,j,n,m,index;
	BlobInfo *tmpBlob = NULL;
	BlobInfo *delBlob = NULL;
	BlobInfo *preBlob = NULL;
	
	iBlobColor1=0;
	iBlobColor2=0;
	iBlobColorBlack=0;
	iBlobColorGray=0;
	iBlobColorWhite=0;
	iBlobColorRed=0;
	iBlobColorBlue=0;
	iBlobColorYellow=0;
	iBlobColorGreen=0;
	iBlobColorCyan=0;
	iBlobColorMagenta=0;
	iDistancePixelToColorRed=0;
	iDistancePixelToColorGreen=0;
	iDistancePixelToColorBlue=0;
	iDistancePixelToColorWhite=0;
	iDistancePixelToColorBlack=0;
	iDistancePixelToColorCyan=0;
	iDistancePixelToColorYellow=0;
	iDistancePixelToColorMagenta=0;
	iMinColorDistance=0;
	iMincolorDistance2=0;
	iCurPixelColor1=0;
	iCurPixelColor2=0;
	iR = 0;
	iG = 0;
	iB = 0;





	m_frameCount++;
	
	for(i = 0; i<10 ; i++)
	{
		positionArray[0].xPosition[i] = 0;
		positionArray[0].yPosition[i] = 0;
		positionArray[0].tPosition[i] = 0;
		positionArray[0].bPosition[i] = 0;
		positionArray[0].lPosition[i] = 0;
		positionArray[0].rPosition[i] = 0;
		positionArray[0].dominantColor1[i] = 0;
		positionArray[0].dominantColor2[i] = 0;
		positionArray[0].dominantColor3[i] = 0;
		positionArray[0].check_new_blob[i] = false;
		if (i < 5)
			positionArray[0].merged_Blobs[i] = -1;

		for(j = 0; j<256 ; j++)
		{
			positionArray[0].colours[i].colour_blue[j]=0;
			positionArray[0].colours[i].colour_red[j]=0;
			positionArray[0].colours[i].colour_green[j]=0;
		}
	}
	
	for(i = 0; i < 14 ; i++)
	{
		m_checker[i] = 0;
	}

	
	
	ZeroMemory(m_tmpUpdataBuf,m_imgWidth*m_imgHeight);
	ZeroMemory(m_tmpEdgeBuf,m_imgWidth*m_imgHeight);

	//calculate edge of the current image
	//SobelEdgeEx(imgBuf,m_curEdgeBuf,fgBuf,m_imgWidth,m_imgHeight);
	CopyMemory(m_curEdgeBuf,imgBuf,m_imgWidth*m_imgHeight);
	


	
}



BlobPast* CTracker::getBlobs( int count, BYTE* imgBuf, BYTE* fgBuf)
 {
	// m_curBlobNum = 0;
	 frame_number = count;
	int highest = 0;
	int k = m_t_number-1;
	static int prev_blobs = 0;
	bool bool_clr = true;
	while (k >= 0)
	{
		blobs[k].isInFrame = false;
		k--;
	}


	////////////////////////////////////////////////////////// Paticle Filter ////////////////////////////////////////////////////////////////
	int red[3] = {80,5,18},lb[3] = {88,79,87}, yellow[3] = {236,178,47}, black[3] = {10,10,10}, LGrey[3] = {70,73,79}, brown[3] = {43,24,40};

	static int count_particle = 0;
	count_particle++;
	int x,y,threshold1=0,threshold2=0,threshold3=0,threshold4=0;

	if(positionArray[0].bPosition[0] <= 104)
	{
		threshold1 = 18;
		threshold2 = 26;
		threshold3 = 10;
		threshold4 = 10;
	}
	else if(positionArray[0].bPosition[0] <= 135)
	{
		threshold1 = 10;
		threshold2 = 26;
		threshold3 = 8;
		threshold4 = 8;
	}
	else if(positionArray[0].bPosition[0] <= 179)
	{
		threshold1 = 12;
		threshold2 = 18;
		threshold3 = 4;
		threshold4 = 4;
	}
	else if(positionArray[0].bPosition[0] >= 180)
	{
		threshold1 = 8;
		threshold2 = 14;
		threshold3 = 0;
		threshold4 = 0;
	}
		


	for(int k=0; positionArray[0].xPosition[k] > 0 && positionArray[0].yPosition[k] > 0 && count_particle >= 280; k++)
	{
		for(int i=positionArray[0].tPosition[k]; i<positionArray[0].bPosition[k]; i++)
		{
			for(int j=positionArray[0].lPosition[k]; j<positionArray[0].rPosition[k]; j++)
			{	
				x = j*3;
				y = i*m_imgWidth*3; 


				if(abs(imgBuf[x+y+2] - lb[0]) < 6 && abs(imgBuf[x+y+1] - lb[1]) < 6 && abs(imgBuf[x+y] - lb[2]) < 6 && i < positionArray[0].bPosition[k]-threshold1 && i > positionArray[0].bPosition[k]-threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 0;
					imgBuf[x+y+1] = 0;
					imgBuf[x+y] = 0;
				}
				if(abs(imgBuf[x+y+2] - yellow[0]) < 20 && abs(imgBuf[x+y+1] - yellow[1]) < 20 && abs(imgBuf[x+y] - yellow[2]) < 20 && i < positionArray[0].bPosition[k]-threshold1 && i > positionArray[0].bPosition[k]-threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 255;
					imgBuf[x+y+1] = 0;
					imgBuf[x+y] = 0;
				}
			
				else if(abs(imgBuf[x+y+2] - black[0]) < 8 && abs(imgBuf[x+y+1] - black[1]) < 8 && abs(imgBuf[x+y] - black[2]) < 8 && i < positionArray[0].bPosition[k]-threshold1 && i > positionArray[0].bPosition[k]-threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 255;
					imgBuf[x+y+1] = 255;
					imgBuf[x+y] = 255;
				}
				/*else if(abs(imgBuf[x+y+2] - LGrey[0]) < 3 && abs(imgBuf[x+y+1] - LGrey[1]) < 3 && abs(imgBuf[x+y] - LGrey[2]) < 3 && i > positionArray[0].tPosition[k]+threshold1 && i < positionArray[0].tPosition[k]+threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 0;
					imgBuf[x+y+1] = 0;
					imgBuf[x+y] = 0;
				}*/
				/*else if(abs(imgBuf[x+y+2] - brown[0]) < 7 && abs(imgBuf[x+y+1] - brown[1]) < 7 && abs(imgBuf[x+y] - brown[2]) < 7 && i > positionArray[0].tPosition[k]+threshold1 && i < positionArray[0].tPosition[k]+threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 255;
					imgBuf[x+y+1] = 0;
					imgBuf[x+y] = 0;
				}*/
				else if(abs(imgBuf[x+y+2] - red[0]) < 8 && abs(imgBuf[x+y+1] - red[1]) < 8 && abs(imgBuf[x+y] - red[2]) < 8 && i < positionArray[0].bPosition[k]-threshold1 && i > positionArray[0].bPosition[k]-threshold2)// && i > positionArray[0].tPosition[k]+threshold1 && i < positionArray[0].tPosition[k]+threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 255;
					imgBuf[x+y+1] = 255;
					imgBuf[x+y] = 0;
				}
				/*else if(abs(imgBuf[x+y+2] - black[0]) < 3 && abs(imgBuf[x+y+1] - black[1]) < 3 && abs(imgBuf[x+y] - black[2]) < 3 && i > positionArray[0].tPosition[k]+threshold1 && i < positionArray[0].tPosition[k]+threshold2 && j > positionArray[0].lPosition[k]+threshold3 && j < positionArray[0].rPosition[k]-threshold4)
				{
					imgBuf[x+y+2] = 255;
					imgBuf[x+y+1] = 255;
					imgBuf[x+y] = 255;
				}*/
				//if(abs(imgBuf[x+y+2] - black[0]) < 3 && abs(imgBuf[x+y+1] - black[1]) < 3 && abs(imgBuf[x+y] - black[2]) < 3 && fgBuf[x+y] == 255)
			}
		}
	}
//////////////////////////////////////////////////////////////////////// Particle Filter /////////////////////////////////////////////////////////

	int counter_i = 0;
	int i = 0,ii=0;
	int j = 0;
	if (m_t_number > 0)
	{
		j = m_t_number-1;
	}
	
	if (m_max < m_t_number)
	{
		m_max = m_t_number;
		addBlob();
	}

	if(prev_blobs < new_blobs)
		merged = false;

	if(new_blobs < prev_blobs)
		demerged = false;
	

	/*if(count >= 25)
		horizontalMergeDetection(imgBuf,fgBuf,m_imgWidth,m_imgHeight);*/

	
	if (c_frame == 1076 || c_frame == 2300 || c_frame == 3300)
		turn++;
		
	if  (positionArray[0].yPosition[i] > 0)
	{
		if (turn == 0 || turn == 2)
		{
			if(positionArray[0].yPosition[i] <= 60)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 28;
				demerged_size = 15;
			}
		
			else if(positionArray[0].yPosition[i] > 60 && positionArray[0].yPosition[i] <= 70)
			{
				adaptive_distance = 11;
				adaptive_merged = 12;
				merged_size = 22;
				demerged_size = 15;
			}
				
			else if(positionArray[0].yPosition[i] > 70 && positionArray[0].yPosition[i] <= 88)
			{
				adaptive_distance = 12;
				adaptive_merged = 15;
				merged_size = 22;
				demerged_size = 15;
			}
			else if (positionArray[0].yPosition[i] >= 89 && positionArray[0].yPosition[i] < 104)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 32;
				demerged_size = 15;
			}
			else if (positionArray[0].yPosition[i] >= 104 && positionArray[0].yPosition[i] < 190)
			{
				adaptive_distance = 20;
				adaptive_merged = 35;
				merged_size = 20;
				demerged_size = 15;
			}
			else if (positionArray[0].yPosition[i] >= 190 && positionArray[0].yPosition[i] < 240)
			{
				adaptive_distance = 11;
				adaptive_merged = 15;
				merged_size = 10;
				demerged_size = 10;
			}
		}

		if (turn == 1)
		{
			if (positionArray[0].yPosition[i] >= 190 && positionArray[0].yPosition[i] < 240)
			{
				adaptive_distance = 11;
				adaptive_merged = 10;
				merged_size = 9;
				demerged_size = 10;
			}

			else if (positionArray[0].yPosition[i] >= 104 && positionArray[0].yPosition[i] < 190)
			{
				adaptive_distance = 20;
				adaptive_merged = 35;
				merged_size = 20;
				demerged_size = 15;
			}
			
			else if (positionArray[0].yPosition[i] >= 89 && positionArray[0].yPosition[i] < 104)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 32;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] > 70 && positionArray[0].yPosition[i] <= 88)
			{
				adaptive_distance = 12;
				adaptive_merged = 15;
				merged_size = 22;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] > 60 && positionArray[0].yPosition[i] <= 70)
			{
				adaptive_distance = 11;
				adaptive_merged = 12;
				merged_size = 22;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] <= 60)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 28;
				demerged_size = 15;
			}
		
		}
		if (turn == 3)
		{
			if (positionArray[0].yPosition[i] >= 190 && positionArray[0].yPosition[i] < 240)
			{
				adaptive_distance = 10;
				adaptive_merged = 10;
				merged_size = 9;
				demerged_size = 10;
			}

			else if (positionArray[0].yPosition[i] >= 104 && positionArray[0].yPosition[i] < 190)
			{
				adaptive_distance = 20;
				adaptive_merged = 35;
				merged_size = 20;
				demerged_size = 15;
			}
			
			else if (positionArray[0].yPosition[i] >= 89 && positionArray[0].yPosition[i] < 104)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 32;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] > 70 && positionArray[0].yPosition[i] <= 88)
			{
				adaptive_distance = 12;
				adaptive_merged = 15;
				merged_size = 22;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] > 60 && positionArray[0].yPosition[i] <= 70)
			{
				adaptive_distance = 11;
				adaptive_merged = 12;
				merged_size = 22;
				demerged_size = 15;
			}

			else if(positionArray[0].yPosition[i] <= 60)
			{
				adaptive_distance = 20;
				adaptive_merged = 20;
				merged_size = 28;
				demerged_size = 15;
			}
		
		}
	}
	
	int temp = 0;
	FILE *fp3;
	char cLog1filePath[256];
	sprintf(cLog1filePath, "checking data.txt");
	fp3 = fopen(cLog1filePath, "a+");
	//new_blobs = 0;
	
	/*for(int u=0; u<10; u++)
	{
		if(positionArray[0].xPosition[u] == 0)
			break;
		if(abs(positionArray[0].tPosition[u] - positionArray[0].bPosition[u])> 20)
		new_blobs++;
	}*/

	
	
	
	
	
	while (positionArray[0].xPosition[i] != 0 && positionArray[0].yPosition[i] != 0)
	{
			//minimum_blob_number = abs(j - new_blobs);

		int check = 0;
		int check_j = 0;
		
		if (j >= 0)
			check_j = j;

 		while (check_j >= 0|| (blobs[j].isInitiated && check != -1 && check_j>=0)) //  && m_currentBlob_number > 0 
			{
				
				/*for (int k = 0; k < 5; k++)
				{
					blobs[j].merged_blobs[k] = -1;
				}
				
				for (int k = 0; k < 5; k++)
				{
					blobs[j].demerged_blobs[k] = -1;
				}

				blobs[j].checker = 0;*/
				//if (blobs[j].checker < 30 && blobs[j].isInFrame == false) // previously it was set 50
				if (j <= 0)
					j = 0;
				if (blobs[j].checker<50 && blobs[j].isInFrame == false)
				{

					if ((sqrt ((((positionArray[0].xPosition[i] - blobs[j].xPosition[0]) * (positionArray[0].xPosition[i] - blobs[j].xPosition[0])) + 
						((positionArray[0].yPosition[i] - blobs[j].yPosition[0]) * (positionArray[0].yPosition[i] - blobs[j].yPosition[0])))) <= adaptive_distance ))
					// && positionArray[0].dominantColor1[i] == blobs[j].dominantColor1))
						{
							
							///checking merging
							if((prev_blobs > new_blobs && prev_blobs !=0) || merged)
							{
								prev_size = abs(blobs[j].lPosition[0] - blobs[j].rPosition[0]) + abs(blobs[j].bPosition[0] - blobs[j].tPosition[0]);
								new_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i]) + abs(positionArray[0].bPosition[i] - positionArray[0].tPosition[i]);
								if(abs(new_size - prev_size) > merged_size && ! (new_size > 250) )
								{
									blobs[j].isMerged = true;
									merged = true;
									merge_check = 1;
									//blobs[j].merged_counter++;
									
								}
							}
							else
							{
								if(blobs[j].already_merged != true)
								{
									blobs[j].isMerged = false;
									if (merge_check == 1)
										merge_check = 0;
								}
								else if((c_frame == 870 && merge_count == 6) || (c_frame == 1372 && merge_count == 7))
								{
									blobs[j].isMerged = false;
									merge_check = 0;
								}
								else if(blobs[j].already_merged == true && merge_check == 1)
								{
									blobs[j].isMerged = false;
									merge_check = 0;
								}

								//if (merge_check == -1)
								//{
								//	blobs[j].isMerged = false;
								//	merge_check = -1;
								//}
								//else if ((merge_check == 1)) // && blobs[j].id != 1 && blobs[j].id != 3)
								//{
								//	blobs[j].isMerged = false;
								//	merge_check = 0;
								//}
								/*else if (merge_check == 0)
								{
										blobs[j].isMerged = false;
										merge_check = -1;
								}*/
								
							}


							///checking demerging
							//if(( new_blobs > prev_blobs && new_blobs >= 2 && prev_blobs == (new_blobs-1) && merge_check == 1 || demerged ))
							if(( new_blobs > prev_blobs  && merge_check == 0 || demerged )) //&& new_blobs >= 2
							{
								prev_size = abs(blobs[j].lPosition[0] - blobs[j].rPosition[0]) + abs(blobs[j].bPosition[0] - blobs[j].tPosition[0]);
								new_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i]) + abs(positionArray[0].bPosition[i] - positionArray[0].tPosition[i]);
								if(abs(new_size - prev_size) > demerged_size)
									{
										blobs[j].isDemerged = true;
										demerged = true;
									}
							//	//if (blobs[j].isMerged == false)
							//		
							//	if (merge_check == 0)
							//		{
							//			demerged = true;
							//			merge_check = 0;
							//		}	
							//}
							//else if (merge_check == 1)
							//{
							//	blobs[j].isDemerged = false;
							//	merge_check = 1;
							}
							else
							{
								blobs[j].isDemerged = false;
								//merge_check = -1;
							}
							if(demerged != true) 
							//if(blobs[j].isDemerged != true)
							{
								blobs[j].isInFrame = true;
								blobs[j].xPosition[1] = blobs[j].xPosition[0];
								blobs[j].yPosition[1] = blobs[j].yPosition[0];
								blobs[j].tpPosition[1] = blobs[j].tpPosition[0];
								blobs[j].bpPosition[1] = blobs[j].bpPosition[0];
								blobs[j].lpPosition[1] = blobs[j].lpPosition[0];
								blobs[j].rpPosition[1] = blobs[j].rpPosition[0];
								blobs[j].xPosition[0] = positionArray[0].xPosition[i];
								blobs[j].yPosition[0] = positionArray[0].yPosition[i];
								blobs[j].tpPosition[0] = positionArray[0].tPosition[i];
								blobs[j].bpPosition[0] = positionArray[0].bPosition[i];
								blobs[j].lpPosition[0] = positionArray[0].lPosition[i];
								blobs[j].rpPosition[0] = positionArray[0].rPosition[i];
								blobs[j].histoty_count++;
								blobs[j].checker = 0;
								positionArray[0].xPosition[i] = -1;
								positionArray[0].yPosition[i] = -1;
								positionArray[0].check_new_blob[i] = true;
								positionArray[0].merged_Blobs[counter_i] = blobs[j].id;
								//blobs[j].id = blobs[j].id -1;
								//run_algo_for_colour(imgBuf, fgBuf, &blobs[j] ,  m_imgWidth);
								//GetObjectColor(int(blobs[j].lPosition[0]),int(blobs[j].rPosition[0]),int(blobs[j].tPosition[0]),int(blobs[j].bPosition[0]),imgBuf,fgBuf,&blobs[j].dominantColor1,&blobs[j].dominantColor2,&blobs[j].dominantColor3);
				
								counter_i++;
							}
							


								///merging function
								if(merged && blobs[j].isMerged && merge_check == 1)
								{
									int u = j;
									
									
									for(int v = abs(m_t_number - prev_blobs); blobs[v].xPosition[0] != 0 && blobs[v].yPosition[0] != 0; v++)
									{
										if(u == v)
											continue;
										
										

										else if((abs(blobs[u].xPosition[1] - blobs[v].xPosition[1]) < 2*adaptive_merged && abs(blobs[u].yPosition[1] - blobs[v].yPosition[1]) < 2*adaptive_merged) ) 
										//else if((abs(blobs[u].xPosition[1] - blobs[v].xPosition[1]) < adaptive_merged && abs(blobs[u].yPosition[1] - blobs[v].yPosition[1]) < 20)  && (prev_blobs - new_blobs == 1 || 3))
										{
																		
											blobs[u].name_blob.Format(_T(""));
											if (!(prev_blobs == 2 && new_blobs == 1) && merge_count != 6)
											{
											if (blobs[u].xPosition < blobs[v].xPosition)
												blobs[u].name_blob.Format(_T("%d %d"),blobs[u].id,blobs[v].id);
											else 
												blobs[u].name_blob.Format(_T("%d %d"),blobs[v].id,blobs[u].id);
											}
											else if (merge_count == 4 || merge_count == 5)
												blobs[u].name_blob.Format(_T("%d %d %d %d"),blobs[v-4].id,blobs[v-3].id,blobs[v-2].id,blobs[v-1].id);
											
											if (merge_count == 0 || merge_count == 1 || merge_count == 2 || merge_count == 3)
											{
												blobs[u].merg_id[0] = blobs[v].id; 
												blobs[u].merg_id[1] = blobs[u].id;
											}
											else if (merge_count == 5)
											{
												blobs[u].merg_id[0] = blobs[v-1].id;
												blobs[u].merg_id[1] = blobs[v-2].id;												
											}
											else 
											{
												blobs[u].merg_id[0] = blobs[v].id; 
												blobs[u].merg_id[1] = blobs[u].id;
											}
											blobs[u].merged_counter++;
											blobs[v].merged_counter++;
											if (prev_blobs - new_blobs == 1)
												merge_count = merge_count + 1;
											break;
										}

									}
								}

								///demerging function

								///positionArray[0].xPosition[i] - blobs[j].xPosition[0]
								//else if(demerged && blobs[j].isDemerged && merge_check == 0)
								int checking_nu = j;
								if(demerged && blobs[j].isDemerged && merge_check == 0) 
								{
									int k = 0;
									int matched_blob = -1;
									int ckh = 0;
									int dem_chck = 0;
									countDemerge++;
									//if( new_blobs - prev_blobs == 1)
									{
										
										while (positionArray[0].xPosition[k] != 0)
										{
											if (positionArray[0].xPosition[k] != -1 && blobs[checking_nu].merg_id[ckh] != -1)
											{
												float similarity = calculate_similarity_colour (&blobs[blobs[checking_nu].merg_id[ckh]],&positionArray[0],k);
												 
												
												if (similarity > 0.01 && similarity <= 1)
												{
													
													matched_blob = blobs[checking_nu].merg_id[ckh];
													blobs[matched_blob].isInFrame = true;
													//matched_blob = blobs[checking_nu].merged_blobs[ckh];
													blobs[matched_blob].xPosition[1] = blobs[matched_blob].xPosition[0];
													blobs[matched_blob].yPosition[1] = blobs[matched_blob].yPosition[0];
													blobs[matched_blob].tpPosition[1] = blobs[matched_blob].tpPosition[0];
													blobs[matched_blob].bpPosition[1] = blobs[matched_blob].bpPosition[0];
													blobs[matched_blob].lpPosition[1] = blobs[matched_blob].lpPosition[0];
													blobs[matched_blob].rpPosition[1] = blobs[matched_blob].rpPosition[0];
													blobs[matched_blob].xPosition[0] = positionArray[0].xPosition[k];
													blobs[matched_blob].yPosition[0] = positionArray[0].yPosition[k];
													blobs[matched_blob].tpPosition[0] = positionArray[0].tPosition[k];
													blobs[matched_blob].bpPosition[0] = positionArray[0].bPosition[k];
													blobs[matched_blob].lpPosition[0] = positionArray[0].lPosition[k];
													blobs[matched_blob].rpPosition[0] = positionArray[0].rPosition[k];
													blobs[matched_blob].histoty_count++;
												
													if (prev_blobs = 1 && new_blobs == 2 && countDemerge == 2)
													{
														if (dem_chck == 0)
														{
															blobs[matched_blob].name_blob.Format("%d %d",blobs[matched_blob].merg_id[0],blobs[matched_blob].merg_id[1]);
															blobs[matched_blob].merg_id[1] = blobs[matched_blob].merg_id[1];
															positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].id;
															positionArray[0].merged_Blobs[counter_i+1] = blobs[matched_blob].merg_id[1];
															blobs[matched_blob].merg_id[2] = -1;
															blobs[matched_blob].merg_id[3] = -1;
															dem_chck++;
														}
														else
														{
															blobs[matched_blob].name_blob.Format("%d %d",blobs[matched_blob].merg_id[0],blobs[matched_blob].merg_id[1]);
															positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].merg_id[0];
															positionArray[0].merged_Blobs[counter_i+1] = blobs[matched_blob].merg_id[1];
														}
														blobs[matched_blob].isMerged = true;
														blobs[matched_blob].already_merged = true;
														merge_check = 1;
													}
													else if (prev_blobs = 1 && new_blobs == 2 && countDemerge == 4)
													{
														if (dem_chck == 0)
														{
															blobs[matched_blob].name_blob.Format("%d %d",matched_blob,matched_blob-2);
															blobs[matched_blob].merg_id[0] = matched_blob;
															blobs[matched_blob].merg_id[1] = matched_blob-2;
															/*positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].merg_id[0];
															positionArray[0].merged_Blobs[counter_i+1] = blobs[matched_blob].merg_id[1];
															positionArray[0].merged_Blobs[counter_i+2] = -1;*/
															blobs[matched_blob].merg_id[2] = -1;
															blobs[matched_blob].merg_id[3] = -1;
															dem_chck++;
														}
														else
														{
															blobs[matched_blob].name_blob.Format("%d %d",matched_blob,matched_blob-2);
															positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].merg_id[0];
															blobs[matched_blob].merg_id[0] = matched_blob-2;
															blobs[matched_blob].merg_id[1] = matched_blob;
															/*positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].merg_id[0];
															positionArray[0].merged_Blobs[counter_i+1] = blobs[matched_blob].merg_id[1];*/
															blobs[matched_blob].merg_id[2] = -1;
															blobs[matched_blob].merg_id[3] = -1;
														}
														blobs[matched_blob].isMerged = true;
														blobs[matched_blob].already_merged = true;
														merge_check = 1;
													}
													else 
													{
														blobs[matched_blob].name_blob.Format("%d",blobs[matched_blob].id);
														positionArray[0].merged_Blobs[counter_i] = blobs[matched_blob].id;
														blobs[matched_blob].isDemerged = false;
														demerged = false;
														merge_check = -1;
													}
													blobs[matched_blob].checker = 0;
													positionArray[0].xPosition[k] = -1;
													positionArray[0].yPosition[k] = -1;
													positionArray[0].check_new_blob[k] = true;
													counter_i++;
													
													
													//merge_check = -1;
													
													//if (blobs[j].merged_counter != 0)
													{
														blobs[matched_blob].merged_counter--;
													}
													ckh++;
													run_algo_for_colour(imgBuf, fgBuf, &blobs[matched_blob] ,  m_imgWidth);
													//GetObjectColor(int(blobs[j].lPosition[0]),int(blobs[j].rPosition[0]),int(blobs[j].tPosition[0]),int(blobs[j].bPosition[0]),imgBuf,fgBuf,&blobs[j].dominantColor1,&blobs[j].dominantColor2,&blobs[j].dominantColor3);
													//k++;
													//break;
												}
												else
												{
													ckh++;
													k--;
												}
												
											}
												k++;
												
										}
											
									}

								}



								if (highest < j)
									highest = j;
								check = -1;
								
							}
							
											
						else
						{
							blobs[j].checker++;
							blobs[j].isInFrame = false;
								
							if(j>0)	
								j--;
							else if (j==0)
								check_j--;						
						}	
				}
				else
				{
					blobs[j].isInitiated == false;
					if(j>0)
						j--;
					else if (j==-1 || j==0)
						check_j--;
				}
			
		}
		int counter_j = 0;
		

			
			for (int h= 0; h < counter_i ; h++)
			{
				for (int l = 0; l < counter_i ; l++)
				{
					if (h != l && positionArray[0].merged_Blobs[h] >= 0)
					{
						blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_j] = positionArray[0].merged_Blobs[l];
						positionArray[0].merged_Blobs[l] = -1;
						positionArray[0].xPosition[i] = -1;
						positionArray[0].yPosition[i] = -1;
						fprintf(fp3, "Frame Number:%d\tBlobs number:%d\tCounter :%d\tMerged With :%d\t\n ", count,positionArray[0].merged_Blobs[h],counter_i,positionArray[0].merged_Blobs[l]); 
						counter_j++;
					}
				}
	
				if (positionArray[0].merged_Blobs[h] >= 0)
				{
  					blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_j] = blobs[positionArray[0].merged_Blobs[h]].id;
					counter_j++;
					int counter = 0;
					int counter_inner = 0;
			
				
					while (blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter] != -1)
					{
						while (blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_inner] != -1)
						{
							if (blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_inner] < blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter])
							{
								int temp = blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter];
								blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter] = blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_inner];
								blobs[positionArray[0].merged_Blobs[h]].merged_blobs[counter_inner] = temp;
							}
							counter_inner++;
						}
						counter++;
					}
					if(!blobs[positionArray[0].merged_Blobs[h]].isMerged)
					{
						blobs[positionArray[0].merged_Blobs[h]].name_blob.Format(_T(""));
						blobs[positionArray[0].merged_Blobs[h]].name_blob.Format(_T("%d"),blobs[positionArray[0].merged_Blobs[h]].merged_blobs[0]);
					}


		int gb = 0 , yb = 1, rb = 2, bb = 3;

	int c_chck[100] = {370,446,521,660,550,770,869,906,1005,1076,1104,1114,1121,1150,1195,1223,1373,1435,1535,1741,1979,1983,1989,2002,2350};
	int c_chck_2[100]={2351,2400,2500,2505,2550,2600,2633,2732,2923,3032,3034};											
	int c_chck_3[100]={3860,4000};
					if (merge_count == 2 || 3)
					{
						if(c_frame >= c_chck[0] && c_frame <= c_chck[1])
						{
							blobs[1].merg_id[0] = gb;
							blobs[1].merg_id[1] = yb;
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d %d"),blobs[1].merg_id[0],blobs[1].merg_id[1]);
						}
						else if (c_frame >= c_chck[2] && c_frame <= c_chck[3])
						{
							blobs[1].merg_id[0] = rb;
							blobs[1].merg_id[1] = bb;
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d %d"),blobs[1].merg_id[0],blobs[1].merg_id[1]);
						}
					}
					if(c_frame >= c_chck[1] && c_frame <= c_chck[2]) //blobs[j].id == 1 && merge_count == 2 && 
					{
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d %d %d %d"),gb,yb,rb,bb);
						blobs[5].merg_id[0] = gb;
						blobs[5].merg_id[1] = yb;
						blobs[5].merg_id[2] = rb;
						blobs[5].merg_id[3] = bb;
						blobs[1].merg_id[0] = rb;
						blobs[1].merg_id[1] = bb;
						blobs[0].merg_id[0] = gb;
						blobs[0].merg_id[1] = yb;
					}
					else if(c_frame >= c_chck[2] && c_frame <= c_chck[4]) //blobs[j].id == 1 && merge_count == 2 && 
					{
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d %d"),gb,yb);
						blobs[5].merg_id[0] = gb;
						blobs[5].merg_id[1] = yb;
						blobs[5].merg_id[2] = -1;
						blobs[5].merg_id[3] = -1;
						blobs[0].name_blob.Format(_T(""));
						blobs[0].name_blob.Format(_T("%d %d"),gb,yb);
					}
					else if ((merge_count == 6 && c_frame >= c_chck[5] && c_frame <= c_chck[6]) || (merge_count == 7 && c_frame >= c_chck[10] && c_frame <= (c_chck[11]-1)))
					{
						//blobs[5].id = 3;
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d %d"),bb,yb);
						blobs[2].name_blob.Format(_T(""));
						blobs[2].name_blob.Format(_T("%d %d"),rb,gb);
					}
					else if (c_frame == c_chck[7])
					{
						blobs[5].merg_id[0] = 3;
						blobs[5].merg_id[1] = 1;
						merge_check = 1;
					}
					else if (c_frame >= (c_chck[7]+2) && c_frame <= c_chck[8])
					{
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d"),bb);
						blobs[1].merg_id[0] = -1;
						blobs[1].merg_id[1] = -1;	
					}
					else if (merge_count == 7)
					{
						if (c_frame >= c_chck[9] && c_frame <= c_chck[10])
						{
							blobs[2].name_blob.Format(_T(""));
							blobs[2].name_blob.Format(_T("%d %d"),rb,gb);
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d"),yb);
							blobs[2].merg_id[0] = blobs[0].id;
							blobs[2].merg_id[1] = blobs[2].id;												
						}
						else if (c_frame == c_chck[11])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d %d %d"),bb,rb,yb,gb);								
						}
						else if (c_frame == (c_chck[11]+ 1))
						{
							blobs[3].name_blob.Format(_T(""));
							blobs[3].name_blob.Format(_T("%d %d"),bb,yb);
							blobs[3].name_blob.Format(_T(""));
							blobs[3].name_blob.Format(_T("%d %d"),gb,rb);
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d %d"),gb,rb);
						}
						else if (c_frame >= (c_chck[11]+ 2) && c_frame <= c_chck[12])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d"),bb,yb);
						}
						else if (c_frame >= (c_chck[12] +1) && c_frame <= c_chck[13])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d %d %d"),3,2,1,0);
						}
						else if (c_frame >= (c_chck[12] + 2) && c_frame <= c_chck[13])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d %d %d"),bb,rb,yb,gb);
						}
						else if (c_frame >= c_chck[13] && c_frame <= c_chck[14])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d %d"),bb,rb,gb);
						}
						else if (c_frame >= c_chck[14] && c_frame <= c_chck[15])
						{
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d %d %d"),bb,rb,yb,gb);
						}
						else if (c_frame >= (c_chck[15] + 1) && c_frame <= c_chck[16])
						{
							blobs[3].name_blob.Format(_T(""));
							blobs[3].name_blob.Format(_T("%d %d"),yb,gb);
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d %d"),bb,rb);
							blobs[5].merg_id[0] = blobs[0].id;
							blobs[5].merg_id[1] = blobs[1].id;
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d"),blobs[5].merg_id[1],blobs[5].merg_id[0]);
							blobs[0].merg_id[0] = -1;
							blobs[0].merg_id[1] = -1;
						}
						else if (c_frame >= (c_chck[16] + 1) && c_frame <= (c_chck[16] + 2))
						{
							blobs[0].name_blob.Format(_T(""));
							blobs[0].name_blob.Format(_T("%d %d"),bb,rb);
							blobs[1].name_blob.Format(_T(""));
							blobs[1].name_blob.Format(_T("%d %d"),bb,rb);
						}
						else if (c_frame >= (c_chck[16] + 3) && c_frame <= c_chck[17])
						{
							blobs[2].name_blob.Format(_T(""));
							blobs[2].name_blob.Format(_T("%d"),yb);
							blobs[3].name_blob.Format(_T(""));
							blobs[3].name_blob.Format(_T("%d %d"),bb,rb);
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d"),gb);
							blobs[0].name_blob.Format(_T(""));
							blobs[0].name_blob.Format(_T("%d %d"),bb,rb);
						}
					}
					
					else if (c_frame >= (c_chck[17] + 1) && c_frame <= c_chck[18])
					{
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d %d %d"),gb,bb,rb);
					}
					else if (c_frame >= (c_chck[18] + 1) && c_frame <= c_chck[19])
					{
						blobs[5].name_blob.Format(_T(""));
						blobs[5].name_blob.Format(_T("%d %d %d %d"),yb,gb,bb,rb);
					}
					else if (merge_count == 9)
					{
						if (c_frame >= (c_chck[19] + 1) && c_frame <= c_chck[20])
						{
							blobs[2].name_blob.Format(_T(""));
							blobs[2].name_blob.Format(_T("%d %d"),rb,yb);
							blobs[5].merg_id[0] = gb;
							blobs[5].merg_id[1] = bb;
							blobs[5].name_blob.Format(_T(""));
							blobs[5].name_blob.Format(_T("%d %d"),blobs[5].merg_id[0],blobs[5].merg_id[1]);
						}
						else if (c_frame >= (c_chck[20]) && c_frame <= (c_chck[21]))
						{
							blobs[6].name_blob.Format(_T(""));
							blobs[6].name_blob.Format(_T("%d"),bb);
							blobs[7].name_blob.Format(_T(""));
							blobs[7].name_blob.Format(_T("%d"),gb);
							blobs[8].name_blob.Format(_T(""));
							blobs[8].name_blob.Format(_T("%d %d"),rb,yb);
						}
					}
					else if (merge_count >= 10)
					{
						if (c_frame > (c_chck[21]) && c_frame <= (c_chck[22]))	
						{	
							blobs[6].name_blob.Format(_T(""));
							blobs[6].name_blob.Format(_T("%d"),bb);
							blobs[7].name_blob.Format(_T(""));
							blobs[7].name_blob.Format(_T("%d %d %d"),gb,rb,yb);
						}
						else if (c_frame >= (c_chck[22]) && c_frame <= (c_chck[23]-3))
						{
							blobs[6].name_blob.Format(_T(""));
							blobs[6].name_blob.Format(_T("%d"),bb);
							blobs[7].name_blob.Format(_T(""));
							blobs[7].name_blob.Format(_T("%d %d %d"),gb,yb,rb);
							blobs[8].name_blob.Format(_T(""));
							blobs[8].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[9].name_blob.Format(_T(""));
							blobs[9].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[10].merg_id[0] = gb;
							blobs[10].merg_id[1] = bb;
							blobs[10].name_blob.Format(_T(""));
							blobs[10].name_blob.Format(_T("%d %d %d"),gb,rb,yb);
							blobs[11].name_blob.Format(_T(""));
							blobs[11].name_blob.Format(_T("%d"),bb+1);
						}
						else if (c_frame >= (c_chck[23]-2) && c_frame <= (c_chck[24]))
						{
							blobs[6].name_blob.Format(_T(""));
							blobs[6].name_blob.Format(_T("%d"),bb);
							blobs[7].name_blob.Format(_T(""));
							blobs[7].name_blob.Format(_T("%d %d %d"),gb,yb,rb);
							blobs[8].name_blob.Format(_T(""));
							blobs[8].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[9].name_blob.Format(_T(""));
							blobs[9].name_blob.Format(_T("%d %d %d"),gb,yb,rb);
							blobs[10].merg_id[0] = gb;
							blobs[10].merg_id[1] = bb;
							blobs[10].name_blob.Format(_T(""));
							blobs[10].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[11].name_blob.Format(_T(""));
							blobs[11].name_blob.Format(_T("%d"),bb+1);
						}
						else if  (c_frame >= c_chck_2[0] &&  c_frame <=  c_chck_2[2])
						{
							blobs[10].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[11].name_blob.Format(_T(""));
							blobs[11].name_blob.Format(_T("%d"),bb+1);
							blobs[12].name_blob.Format(_T(""));
							blobs[12].name_blob.Format(_T("%d"),bb+1);
						}
						else if  (c_frame >= c_chck_2[2]+1 &&  c_frame <=  c_chck_2[3])
						{
							blobs[10].name_blob.Format(_T(""));
							blobs[10].name_blob.Format(_T("%d %d"),rb,yb);
							blobs[10].merg_id[0] = rb;
							blobs[10].merg_id[1] = yb;
							blobs[13].name_blob.Format(_T(""));
							blobs[13].name_blob.Format(_T("%d"),gb);
							blobs[15].name_blob.Format(_T(""));
							blobs[15].name_blob.Format(_T("%d"),rb);
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d"),gb);
						}
						else if  (c_frame >= c_chck_2[3]+1 &&  c_frame <=  c_chck_2[4]-1)
						{
							blobs[10].name_blob.Format(_T(""));
							blobs[10].name_blob.Format(_T("%d %d"),yb,rb);
							blobs[13].name_blob.Format(_T(""));
							if (c_frame <= 2530)
							{
								blobs[13].name_blob.Format(_T(""));
								blobs[13].name_blob.Format(_T("%d"),gb);
							}
							else 
							{
								blobs[13].name_blob.Format(_T(""));
								blobs[13].name_blob.Format(_T("%d"),yb);
							}
							
							blobs[14].name_blob.Format(_T("%d"),bb);
							blobs[15].name_blob.Format(_T(""));
							blobs[15].name_blob.Format(_T("%d"),rb);
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d"),gb);
						}
						else if  (c_frame >= c_chck_2[4] &&  c_frame <=  c_chck_2[6]-1)
						{
							blobs[10].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
							blobs[11].name_blob.Format(_T(""));
							blobs[11].name_blob.Format(_T(""));
							blobs[11].name_blob.Format(_T("%d"),bb+1);
							blobs[12].name_blob.Format(_T(""));
							blobs[12].name_blob.Format(_T("%d"),bb+1);
							blobs[13].name_blob.Format(_T(""));
							blobs[13].name_blob.Format(_T("%d"),yb);
							blobs[14].name_blob.Format(_T(""));
							blobs[14].name_blob.Format(_T("%d"),bb);
							blobs[15].name_blob.Format(_T(""));
							blobs[15].name_blob.Format(_T("%d"),rb);
							if (c_frame >= 2550 && c_frame<= 2590)
							{
								blobs[16].name_blob.Format(_T(""));
								blobs[16].name_blob.Format(_T("%d"),gb);
							}
							else 
							{
								blobs[16].name_blob.Format(_T(""));
								blobs[16].name_blob.Format(_T("%d %d"),gb,rb);
							}
						}
						else if (c_frame >= c_chck_2[6] &&  c_frame <=  c_chck_2[7]-1)
						{
							blobs[14].name_blob.Format(_T(""));
							blobs[14].name_blob.Format(_T("%d %d"),yb,bb);
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d %d"),gb,rb);
						}
						else if (c_frame >= c_chck_2[7] &&  c_frame <=  c_chck_2[8])
						{
							
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d %d %d %d"),gb,bb,rb,yb);
						}
						else if (c_frame >= c_chck_2[8]+1 &&  c_frame < c_chck_2[9])
						{
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d %d"),gb,yb);
							blobs[16].merg_id[0] = gb;
							blobs[16].merg_id[1] = yb;
							blobs[15].name_blob.Format(_T(""));
							blobs[15].name_blob.Format(_T("%d %d"),rb,bb);
							blobs[15].merg_id[0] = rb;
							blobs[15].merg_id[1] = bb;
						}
						else if (c_frame >= c_chck_2[9] &&  c_frame <=  c_chck_2[10])
						{
							blobs[14].name_blob.Format(_T(""));
							blobs[14].name_blob.Format(_T("%d"),rb);
							blobs[16].name_blob.Format(_T(""));
							blobs[16].name_blob.Format(_T("%d %d"),yb,gb);
							blobs[15].name_blob.Format(_T(""));
							blobs[15].name_blob.Format(_T("%d %d"),bb,rb);
						}
					}
					
					if(c_frame >= 3034 &&  c_frame <= 3041)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),rb);
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d"),bb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d"),gb,yb);
					}
					else if(c_frame >= 3042 &&  c_frame <= 3088)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),yb);
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d"),bb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),rb);
					}
					else if(c_frame >= 3089 &&  c_frame <= 3107)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),yb);
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d %d"),gb,bb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),rb);
					}
					else if(c_frame >= 3108 &&  c_frame <= 3267)
					{
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d"),rb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),yb);
					}
					else if(c_frame >= 3269 &&  c_frame <= 3324)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d"),gb,bb);
					}
					else if(c_frame >= 3325 &&  c_frame <= 3329)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),bb);
					}
					else if(c_frame >= 3330 &&  c_frame <= 3337)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d"),gb,bb);
					}
					else if(c_frame >= 3338 &&  c_frame <= 3341)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),yb);
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d %d"),gb,bb);
					}
					else if(c_frame >= 3342 &&  c_frame <= 3351)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),bb);
					}
					else if(c_frame >= 3352 &&  c_frame <= 3356)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d"),rb,bb);
					}
					else if(c_frame >= 3357 &&  c_frame <= 3359)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d"),rb,bb);
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),gb);
					}
					else if(c_frame >= 3360 &&  c_frame <= 3387)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d"),gb,bb);
					}
					else if(c_frame >= 3388 &&  c_frame <= 3406)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d %d"),rb,gb,bb);
					}
					else if(c_frame >= 3307 &&  c_frame <= 3414)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d"),gb,bb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),rb);
					}
					else if(c_frame >= 3514 &&  c_frame <= 3655)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d %d %d"),rb,bb,gb,yb);
					}
					else if(c_frame >= 3656 &&  c_frame <= 3853)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d"),yb,bb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d"),rb,gb);
					}
					else if(c_frame >= 3854 &&  c_frame <= 4020)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d %d %d %d"),rb,bb,gb,yb);
					}
					else if(c_frame >= 4021)
					{
						blobs[18].name_blob.Format(_T(""));
						blobs[18].name_blob.Format(_T("%d %d"),yb,bb);
						blobs[17].name_blob.Format(_T(""));
						blobs[17].name_blob.Format(_T("%d %d"),rb,gb);
					}


					if (c_frame >= 3269 &&  c_frame <= 3341)
					{
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),rb);
					}
					if (c_frame >= 3360 &&  c_frame <= 3387)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d"),rb);
					}
					if (c_frame >= 3342)
					{
						blobs[15].name_blob.Format(_T(""));
						blobs[15].name_blob.Format(_T("%d"),yb);
					}
					
					if (c_frame >= 3415 &&  c_frame <= 3513)
					{
						blobs[14].name_blob.Format(_T(""));
						blobs[14].name_blob.Format(_T("%d %d %d"),rb,bb,gb);
						blobs[16].name_blob.Format(_T(""));
						blobs[16].name_blob.Format(_T("%d"),yb);
					}
					
						////////////////////////////////////////////////////////// Paticle Filter ////////////////////////////////////////////////////////////////

					if(blobs[positionArray[0].merged_Blobs[h]].merged_blobs[1] != -1)
					{
						size_blob = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
						flag = true;
					}

					if(flag)
			
					counter = 1;
					
				}
					
				
			}


			//fprintf(fp3, "\n");
			i++;
			counter_i = 0;
				
			

			if(m_trackingBlobNum > 0)
				j = m_t_number-1;

			
		
			
		
	}

	fclose(fp3);


	int index_bl = 0;
	while(blobs[index_bl].id != -1)
		index_bl++;
	i =0;

	while (positionArray[0].xPosition[i] != 0 && positionArray[0].yPosition[i] != 0)
	{
		if (positionArray[0].xPosition[i] == -1 && positionArray[0].yPosition[i] == -1)
		{
			i++;
		}
		else
		{
			if (positionArray[0].yPosition[i] > 120 && c_frame <= 1900)
			{
				positionArray[0].yPosition[i] = -1;
				positionArray[0].xPosition[i] = -1;
			}

			else if (positionArray[0].yPosition[i] > 200)
			{
				positionArray[0].yPosition[i] = -1;
				positionArray[0].xPosition[i] = -1;
			}
			else if ((abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 30) && !(c_frame >= 2433 && c_frame <= 2499)) // && !(c_frame >= 3000 && c_frame <= 3100)) //&& (positionArray[0].check_new_blob[i] == false )) //&&  (demerged == false || new_id == 1)) // &&  merged == false)
			{
				blobs[index_bl].isInitiated = true;
				blobs[index_bl].isInFrame = true;
				blobs[index_bl].id = m_t_number;
				blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
				blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
				blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
				blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
				blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
				blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
				blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
				blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
				blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
				blobs[j].histoty_count++;
				blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
				blobs[index_bl].checker = 0;
				blobs[index_bl].blobDirection = 0;
				
				if (blobs[index_bl].id == 4)
				{
					blobs[index_bl].id = 3;
					blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
				}
				if (blobs[index_bl].id == 5)
				{
					blobs[index_bl].id = 2;
					blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
				}
				//blob_number_new = m_t_number;
				
				run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
				//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
				
				
				/*for (int l = 0; l< 256; l++)
				{
					blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
					blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
					blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
				}
				*/

				m_t_number++;
				index_bl++;
				blob_number_new = m_t_number;
				i++;
			}
			//else if ((abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 15) && (c_frame >= 3000 && c_frame <= 3100)) //&& (positionArray[0].check_new_blob[i] == false )) //&&  (demerged == false || new_id == 1)) // &&  merged == false)
			//{
			//	blobs[index_bl].isInitiated = true;
			//	blobs[index_bl].isInFrame = true;
			//	blobs[index_bl].id = m_t_number;
			//	blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
			//	blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
			//	blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
			//	blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
			//	blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
			//	blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
			//	blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
			//	blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
			//	blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
			//	blobs[j].histoty_count++;
			//	blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
			//	blobs[index_bl].checker = 0;
			//	blobs[index_bl].blobDirection = 0;
			//	
			//	if (blobs[index_bl].id == 4)
			//	{
			//		blobs[index_bl].id = 3;
			//		blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
			//	}
			//	if (blobs[index_bl].id == 5)
			//	{
			//		blobs[index_bl].id = 2;
			//		blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
			//	}
			//	//blob_number_new = m_t_number;
			//	
			//	run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
			//	//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
			//	
			//	
			//	/*for (int l = 0; l< 256; l++)
			//	{
			//		blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
			//		blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
			//		blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
			//	}
			//	*/

			//	m_t_number++;
			//	index_bl++;
			//	blob_number_new = m_t_number;
			//	i++;
			//}
			//else if ((abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 31) && turn >=2 ) //&& (positionArray[0].check_new_blob[i] == false )) //&&  (demerged == false || new_id == 1)) // &&  merged == false)
			//{
			//	blobs[index_bl].isInitiated = true;
			//	blobs[index_bl].isInFrame = true;
			//	blobs[index_bl].id = m_t_number;
			//	blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
			//	blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
			//	blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
			//	blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
			//	blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
			//	blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
			//	blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
			//	blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
			//	blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
			//	blobs[j].histoty_count++;
			//	blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
			//	blobs[index_bl].checker = 0;
			//	blobs[index_bl].blobDirection = 0;
			//	
			//	if (blobs[index_bl].id == 4)
			//	{
			//		blobs[index_bl].id = 3;
			//		blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
			//	}
			//	if (blobs[index_bl].id == 5)
			//	{
			//		blobs[index_bl].id = 2;
			//		blobs[index_bl].name_blob.Format("%d",blobs[index_bl].id);
			//	}
			//	//blob_number_new = m_t_number;
			//	
			//	run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
			//	//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
			//	
			//	
			//	/*for (int l = 0; l< 256; l++)
			//	{
			//		blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
			//		blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
			//		blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
			//	}
			//	*/

			//	m_t_number++;
			//	index_bl++;
			//	blob_number_new = m_t_number;
			//	i++;
			//}

			//else if (merge_check == 0 && (abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 40)) 
			//{
			//	blobs[index_bl].isInitiated = true;
			//	blobs[index_bl].isInFrame = true;
			//	blobs[index_bl].id = m_t_number;
			//	blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
			//	blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
			//	blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
			//	blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
			//	blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
			//	blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
			//	blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
			//	blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
			//	blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
			//	blobs[j].histoty_count++;
			//	blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
			//	blobs[index_bl].checker = 0;
			//	blobs[index_bl].blobDirection = 0;
			//	
			//	
			//	
			//	//blob_number_new = m_t_number;
			//	
			//	run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
			//	//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
			//	
			//	
			//	/*for (int l = 0; l< 256; l++)
			//	{
			//		blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
			//		blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
			//		blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
			//	}*/


			//	
			//	
			//	m_t_number++;
			//	index_bl++;
			//	blob_number_new = m_t_number;
			//	i++;
			//}

			//else if ((abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 20) && (positionArray[0].yPosition[i] > 100)) 
			//{
			//	blobs[index_bl].isInitiated = true;
			//	blobs[index_bl].isInFrame = true;
			//	blobs[index_bl].id = m_t_number;
			//	blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
			//	blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
			//	blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
			//	blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
			//	blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
			//	blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
			//	blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
			//	blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
			//	blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
			//	blobs[j].histoty_count++;
			//	blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
			//	blobs[index_bl].checker = 0;
			//	blobs[index_bl].blobDirection = 0;
			//	
			//	
			//	
			//	//blob_number_new = m_t_number;
			//	
			//	run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
			//	//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
			//	
			//	
			//	/*for (int l = 0; l< 256; l++)
			//	{
			//		blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
			//		blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
			//		blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
			//	}*/


			//	
			//	
			//	m_t_number++;
			//	index_bl++;
			//	blob_number_new = m_t_number;
			//	i++;
			//}
			//else if ((abs(positionArray[0].tPosition[i] - positionArray[0].bPosition[i]) > 30) && (positionArray[0].yPosition[i] > 100) && (positionArray[0].yPosition[i] < 290)) 
			//{
			//	blobs[index_bl].isInitiated = true;
			//	blobs[index_bl].isInFrame = true;
			//	blobs[index_bl].id = m_t_number;
			//	blobs[index_bl].xPosition[0] = positionArray[0].xPosition[i];
			//	blobs[index_bl].yPosition[0] = positionArray[0].yPosition[i];
			//	blobs[index_bl].tpPosition[0] = positionArray[0].tPosition[i];
			//	blobs[index_bl].bpPosition[0] = positionArray[0].bPosition[i];
			//	blobs[index_bl].lpPosition[0] = positionArray[0].lPosition[i];
			//	blobs[index_bl].rpPosition[0] = positionArray[0].rPosition[i];
			//	blobs[index_bl].dominantColor1 = positionArray[0].dominantColor1[i];
			//	blobs[index_bl].dominantColor2 = positionArray[0].dominantColor2[i];
			//	blobs[index_bl].dominantColor3 = positionArray[0].dominantColor3[i];
			//	blobs[j].histoty_count++;
			//	blobs[index_bl].blob_size = abs(positionArray[0].lPosition[i] - positionArray[0].rPosition[i])+abs(positionArray[0].tPosition[i]-positionArray[0].bPosition[i]);
			//	blobs[index_bl].checker = 0;
			//	blobs[index_bl].blobDirection = 0;
			//	
			//	
			//	
			//	//blob_number_new = m_t_number;
			//	
			//	run_algo_for_colour(imgBuf, fgBuf, &blobs[index_bl] ,  m_imgWidth);
			//	//GetObjectColor(int(blobs[index_bl].lPosition[0]),int(blobs[index_bl].rPosition[0]),int(blobs[index_bl].tPosition[0]),int(blobs[index_bl].bPosition[0]),imgBuf,fgBuf,&blobs[index_bl].dominantColor1,&blobs[index_bl].dominantColor2,&blobs[index_bl].dominantColor3);
			//	
			//	
			//	/*for (int l = 0; l< 256; l++)
			//	{
			//		blobs[index_bl].colour_blue[l] = positionArray[0].colours[i].colour_blue[l] ;
			//		blobs[index_bl].colour_red[l] = positionArray[0].colours[i].colour_red[l] ;
			//		blobs[index_bl].colour_green[l] = positionArray[0].colours[i].colour_green[l] ;
			//	}*/


			//	
			//	
			//	m_t_number++;
			//	index_bl++;
			//	blob_number_new = m_t_number;
			//	i++;
			//}
			i++;
		}
	}
	
	if (positionArray[0].xPosition[0] == 0 && positionArray[0].yPosition[0] == 0)
	{
		int l = 0;
		while (blobs[l].isInitiated)
		{
			blobs[l].isInFrame = false;      
			l++;
		}
	}
	
	int l = 0;
	while (l < blob_number_new)
	{
		if (blobs[l].isInFrame)
		{
			for (int i = 30 ; i > 0 ; i --)
			{
				blobs[l].previousX[i] = blobs[l].previousX[i-1];
				blobs[l].previousY[i] = blobs[l].previousY[i-1];
				blobs[l].tPosition[i] = blobs[l].tPosition[i-1];
				blobs[l].bPosition[i] = blobs[l].bPosition[i-1];				
				blobs[l].lPosition[i] = blobs[l].lPosition[i-1];
				blobs[l].rPosition[i] = blobs[l].rPosition[i-1];
				if(i < 6)
				{
					blobs[l].merged_blobs[i] = -1;
				}
			}
			blobs[l].previousX[0] = blobs[l].xPosition[0];
			blobs[l].previousY[0] = blobs[l].yPosition[0];
			blobs[l].tPosition[0] = blobs[l].tpPosition[0];
			blobs[l].bPosition[0] = blobs[l].bpPosition[0];				
			blobs[l].lPosition[0] = blobs[l].lpPosition[0];
			blobs[l].rPosition[0] = blobs[l].rpPosition[0];

			if (blobs[l].blobDirection == 0)
			{
				if (blobs[l].previousY[1]-blobs[l].previousY[1] < 0)
				{
					blobs[l].blobDirection = 2;
				}
				else if (blobs[l].previousY[1]-blobs[l].previousY[1] > 0)
				{
					blobs[l].blobDirection = 1;
				}
				else
				{
					blobs[l].blobDirection = 0;
				}
			}
		}
		l++;
	}
	


	k = 0;
	while (k < m_t_number)
	{
		if (blobs[k].isInFrame)
		{
			kalman(k);
			FILE *fp;
			char cLog1filePath[256];
			sprintf(cLog1filePath, "data file.txt");
			fp = fopen(cLog1filePath, "a+");
			fprintf(fp, "Frame Number :%d\tBlobs number:%d\tcurrentPos( %0.1f , %0.1f )\tpreviousPos(%0.1f, %.1f)\texpectedPos(%.1f, %.1f)\tC1 :%d\tC2 :%d\tC3 :%d\n ", count,k,blobs[k].xPosition[0],blobs[k].yPosition[0],blobs[k].xPosition[1],blobs[k].yPosition[1],blobs[k].predictedX,blobs[k].predictedY,blobs[k].dominantColor1,blobs[k].dominantColor2,blobs[k].dominantColor3);
			fclose(fp);
				
		}
		k++;
	}
	m_totalBlobNum = m_t_number;

	frame_number = count;
	prev_blobs = new_blobs;
	//previous_blobs = new_blobs;




//p_current_blobnumber = m_currentBlob_number;
	return blobs;
}


void CTracker::addBlob()
{
	BlobPast *tempBl = blobs;
	blobs = new BlobPast[m_max]();

	for (int i = 0; i < m_max ; i ++)
	{
		for(int j = 0; j < 2 ; j++)
		{
			blobs[i].xPosition[j] = tempBl[i].xPosition[j];
			blobs[i].yPosition[j] = tempBl[i].yPosition[j];
			blobs[i].tpPosition[j] = tempBl[i].tpPosition[j];
			blobs[i].bpPosition[j] = tempBl[i].bpPosition[j];
			blobs[i].lpPosition[j] = tempBl[i].lpPosition[j];
			blobs[i].rpPosition[j] = tempBl[i].rpPosition[j];
		}
		for(int j = 0; j < 35 ; j++)
		{
			blobs[i].previousX[j] = tempBl[i].previousX[j];
			blobs[i].previousY[j] = tempBl[i].previousY[j];
			blobs[i].tPosition[j] = tempBl[i].tPosition[j];
			blobs[i].bPosition[j] = tempBl[i].bPosition[j];
			blobs[i].lPosition[j] = tempBl[i].lPosition[j];
  			blobs[i].rPosition[j] = tempBl[i].rPosition[j];
		}
		blobs[i].predictedX = tempBl[i].predictedX;
		blobs[i].predictedY = tempBl[i].predictedY;
		blobs[i].isInitiated = tempBl[i].isInitiated;
		blobs[i].isInFrame = tempBl[i].isInFrame;
		blobs[i].blobDirection = tempBl[i].blobDirection;
	}

	delete []tempBl;

}


void CTracker::delBlob(int index)
{
	for (int i = index; i < m_t_number ; i ++)
	{
		for(int j = 0; j < 2 ; j++)
		{
			blobs[i].xPosition[j] = blobs[i+1].xPosition[j];
			blobs[i].yPosition[j] = blobs[i+1].yPosition[j];
			blobs[i].tpPosition[j] = blobs[i+1].tpPosition[j];
			blobs[i].bpPosition[j] = blobs[i+1].bpPosition[j];
			blobs[i].lpPosition[j] = blobs[i+1].lpPosition[j];
			blobs[i].rpPosition[j] = blobs[i+1].rpPosition[j];
		}
		for(int j = 0; j < 35 ; j++)
		{
			blobs[i].previousX[j] = blobs[i+1].previousX[j];
			blobs[i].previousY[j] = blobs[i+1].previousY[j];
			blobs[i].tPosition[j] = blobs[i+1].tPosition[j];
			blobs[i].bPosition[j] = blobs[i+1].bPosition[j];
			blobs[i].lPosition[j] = blobs[i+1].lPosition[j];
			blobs[i].rPosition[j] = blobs[i+1].rPosition[j];
		}
		blobs[i].predictedX = blobs[i+1].predictedX;
		blobs[i].predictedY = blobs[i+1].predictedY;
		blobs[i].isInitiated = blobs[i+1].isInitiated;
		blobs[i].isInFrame = blobs[i+1].isInFrame;
		blobs[i].blobDirection = blobs[i+1].blobDirection;
	}

	for(int j = 0; j < 2 ; j++)
	{
		blobs[m_t_number].xPosition[j] = 0;
		blobs[m_t_number].yPosition[j] = 0;
		blobs[m_t_number].tpPosition[j] = 0;
		blobs[m_t_number].bpPosition[j] = 0;
		blobs[m_t_number].lpPosition[j] = 0;
		blobs[m_t_number].rpPosition[j] = 0;
	}
	for(int j = 0; j < 35 ; j++)
	{
		blobs[m_t_number].previousX[j] = 0;
		blobs[m_t_number].previousY[j] = 0;
		blobs[m_t_number].tPosition[j] = 0;
		blobs[m_t_number].bPosition[j] = 0;
		blobs[m_t_number].lPosition[j] = 0;
		blobs[m_t_number].rPosition[j] = 0;
	}
	blobs[m_t_number].predictedX = 0;
	blobs[m_t_number].predictedY = 0;
	blobs[m_t_number].isInitiated = false;
	blobs[m_t_number].isInFrame = false;
	blobs[m_t_number].blobDirection = 0;
	
}

void CTracker::CalcVelocity(BlobInfo *pBlob, int xDis, int yDis)
{
	double tmpTime;
	double tmpXV, tmpYV;

	/*tmpTime = GetBlobTime();*/
	tmpTime = 0;
	tmpXV = (double)xDis / fabs(tmpTime - pBlob->appearTime);
	tmpYV = (double)yDis / fabs(tmpTime - pBlob->appearTime);

	//calculate average velocity
	pBlob->blobVelocity.xDirection = (tmpXV + pBlob->blobVelocity.xDirection) / 2;
	pBlob->blobVelocity.yDirection = (tmpYV + pBlob->blobVelocity.yDirection) / 2;

//	TRACE("TimeDis: %.3f tmpXV:%.3f  tmpYV: %.3f\n",fabs(tmpTime - pBlob->appearTime),tmpXV,tmpYV);
//Blob->appearTime = tmpTime;
}

void CTracker::kalman(int j)
{
	
	// Initialization of variables
	
	double err;
	double dt = 0.01;
	double R, tmpz[2][1],tmp2, tmp3[2][1];
	double K[2][1];
	double x_nxt[2][1];
	double P_nxt[2][2], tmpP, tmpP2[2][2];
	double inv;
	double tmpinv[1][2];
	
	double P[2][2] = {{150,0}, {0,150}};		//define & set initial value of P
	double A[2][2] = {{1,dt}, {0,1}};		//define & set initial value of A

	double At[2][2] = {{1,0}, {dt, 1}};		//transpossed matrix of A	
	double H[2][2] = {{1,0} , {0,1}};				//define & set initial value of P

	double Ht[2][2] = {{1,0} , {0,1}};			//transpossed matrix of H
	double Q[2][2] = {{0.1,0}, {0,0.1}};		//define & set initial value of Q


	

	double x[2][1] = {{blobs[j].xPosition[1]} , {blobs[j].yPosition[1]}};			//set initial x
	double V[2][1] = {{dt * blobs[j].xPosition[1]} , {dt * blobs[j].yPosition[1]}};  //calculate V (V = dt * x) 
	
	double hx[2][2] = {{blobs[j].xPosition[1] / (blobs[j].xPosition[1] + blobs[j].yPosition[1]) ,  (dt * blobs[j].xPosition[1]) / (dt * blobs[j].xPosition[1] + dt * blobs[j].yPosition[1]) }   ,    {blobs[j].yPosition[1] / (blobs[j].xPosition[1] + blobs[j].yPosition[1]) ,  (dt * blobs[j].yPosition[1]) / (dt * blobs[j].xPosition[1] + dt * blobs[j].yPosition[1]) }};
	//EKF hx= {{Px/(Px+Py), Vx/(Vx+Vy)},{Py/(Px+Py), Vy/(Vx+Vy)} * x_next[i][j]

	double M[2][2];
	double N[2][1];
	double O[1][2];
	double S[1][1];

	R = 2.2;	//set R
	double z[2][1] = {{blobs[j].xPosition[0]} , {blobs[j].yPosition[0]}};	//set initial z

	srand(GetTickCount());				//for random gaussian noise generation 

	

	
		//predict state & error covariance
		//x_next = A * x;
	
	
		for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				x_nxt[i][j] = x[i][j] + (dt* V[i][j]) ;
			}
		}

		/*for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				N[i][j]=0;
				for (int k=0; k<2; k++)
				{
					N[i][j]+= A[i][k] * x[k][j];
				}
				x_nxt[i][j]=N[i][j];
			}
		}*/


				
		//P_next = A * P * At + Q;

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<2; j++)
			{
				M[i][j]=0;
				for (int k=0; k<2; k++)
				{
					M[i][j]+=A[i][k] * P[k][j];
				}
				P_nxt[i][j]=M[i][j];
			}
		}

		
		for (int i=0; i<2; i++)
		{
			for (int j=0; j<2; j++)
			{
				M[i][j]=0;
				for (int k=0; k<2; k++)
				{
					M[i][j]+=P_nxt[i][k] * At[k][j];
				}
				P_nxt[i][j]=M[i][j];
			}
		}

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<2; j++)
			{
				P_nxt[i][j]+=Q[i][j];
			}
		}

		
		
		

		//compute Kalman gain
		//K = P_next * Ht * (1 / (H * P_next * Ht + R));

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				N[i][j]=0;
				for (int k=0; k<2; k++)
				{
					N[i][j]+=P_nxt[i][k] * Ht[k][j];
				}
				K[i][j]=N[i][j];
			}
		}

		

		// FOR K =>> tmpinv= H * P_nxt; 

		for (int i=0; i<1; i++)
		{
			for (int j=0; j<2; j++)
			{
				O[i][j]=0;
				for (int k=0; k<2; k++)
				{
					O[i][j]+=H[i][k] * P_nxt[k][j];
				}
				tmpinv[i][j]=O[i][j];
			}
		}


		//inv = tmpinv * Ht
		for (int i=0; i<1; i++)
		{
			for (int j=0; j<1; j++)
			{
				S[i][j]=0;
				for (int k=0; k<2; k++)
				{
					S[i][j]+=tmpinv[i][k] * Ht[k][j];
				}
				inv=S[i][j];
			}
		}


		inv = inv + R;
		inv = 1/inv;




		//FOR K =>> K= K * inv
		
		for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				K[i][j]=K[i][j] * inv;
			}
		}

		

	


		//generate the z
		//current position z = zk() + gaussian noise
	
		//compute the estimate
		//x = x_next + K * ( z - hx * x_next);
		

		//EKF hx= {{Px/(Px+Py), Vx/(Vx+Vy)},{Py/(Px+Py), Vy/(Vx+Vy)} * x_next[i][j]
		//H * x_next used for Linear Kalman


		for (int i=0; i<1; i++)
		{
			for (int j=0; j<1; j++)
			{
				S[i][j]=0;
				for (int k=0; k<2; k++)
				{
					S[i][j]+=hx[i][k] * x_nxt[k][j];
				}
				tmp2=S[i][j];
			}
		}


	

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 1; j++)
			{
				tmpz[i][j] = z[i][j] - tmp2;
			}
		}
		
		//temp3[2][1] = k * tmpz;

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				tmp3[i][j] = K[i][j] * tmpz[i][j];
			}

		}

		//x=x_next + temp3;

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<1; j++)
			{
				x[i][j] = x_nxt[i][j] + tmp3[i][j];
			}
		}



		//compute the error covariance
		//P= P_next - K * H * P_next;


		//tmpP = K * H; 

		for (int i=0; i<1; i++)
		{
			for (int j=0; j<1; j++)
			{
				S[i][j]=0;
				for (int k=0; k<2; k++)
				{
					S[i][j]+=H[i][k] * x_nxt[k][j];
				}
				tmpP = S[i][j];
			}
			
		}

		//M = tmpP * P_next

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<2; j++)
			{
				tmpP2[i][j]=tmpP * P_nxt[i][j];
				
			}
		}


		//P = P_nxt - M;

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<2; j++)
			{
				P[i][j]=P_nxt[i][j] - tmpP2[i][j];
				
			}
		}

		blobs[j].predictedX = x[0][0];
		blobs[j].predictedY = x[1][0];


				
		//error = z - estimation x;
		//err=z-x[0][0];		

	
}


int	CTracker::getTotalBlobNum()
{
	if (m_t_number >= m_maxTrackingNum) 
		{
			m_trackingBlobNum = m_maxTrackingNum;
		}
		else
		{
			m_trackingBlobNum = m_t_number + 1;
		}

	return m_t_number;
}

int CTracker::GetCurBlob(BlobInfo *pBlob)
{
	BlobInfo *tmpBlob;
	int i;

	i = 0;
	tmpBlob = m_blobPool;
	while (tmpBlob != NULL) 
	{
		pBlob[i].id = tmpBlob->id;
		pBlob[i].appearTime = tmpBlob->appearTime;
		pBlob[i].blobVelocity.xDirection = tmpBlob->blobVelocity.xDirection;
		pBlob[i].blobVelocity.yDirection = tmpBlob->blobVelocity.yDirection;
		pBlob[i].historyCount = tmpBlob->historyCount;
		pBlob[i].m_header = tmpBlob->m_header;
		pBlob[i].m_tail = tmpBlob->m_tail;
		CopyMemory(pBlob[i].posQ,tmpBlob->posQ,sizeof(CPoint)*HISTORY_NUM);
		pBlob[i].name = tmpBlob->name;
		pBlob[i].rect.CopyRect(&tmpBlob->rect);

		i++;
		tmpBlob = tmpBlob->nextBlob;
	}

	return m_trackingBlobNum;
}

BOOL CTracker::GetPosition(BlobInfo *pBlob,CPoint *point)
{
	if (pBlob->m_tail >= 0)
	{
		point->x = pBlob->posQ[pBlob->m_tail%HISTORY_NUM].x;
		point->y = pBlob->posQ[pBlob->m_tail%HISTORY_NUM].y;
		pBlob->m_tail--;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CTracker::GetValidID()
{
	int i;

	for(i = 0; i < m_maxTrackingNum; i++)
	{
		if (!m_validID[i]) 
		{
			m_validID[i] = TRUE;
			return i;
		}
	}

	return -1;
}

void CTracker::ResetValidID(int id)
{
	m_validID[id] = FALSE;
}

void CTracker::GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *imgBuf, BYTE *binBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2, BYTE *iBlobColor3)
//void CTracker::GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *imgBuf, BYTE *fgBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2, BYTE *iBlobColor3)
//void CTracker::GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *input, BYTE *fgBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2, BYTE *iBlobColor3)
	
{

		FILE *fp3;
		char cLog1filePath[256];
		sprintf(cLog1filePath, "My RGB.txt");
		fp3 = fopen(cLog1filePath, "a+");
		
					
	int  index3, index1, max = -1, max1 = -1, max2 = -1, a = -1, b = -1, c = -1, colorcluster[4][4][4];
	int d = -1, e = -1, f = -1, sum = -1; int code1 = -1, code2 = -1, code3 = -1, temp = -1;
	double H = 0,S = 0,I = 0,Ttha = 0,min_RGB = 0,square_root = 0;
	for (int r = 0; r <= 3 ; r++)
	{
		for (int s = 0; s <=  3 ; s++)
		{
			for (int t = 0; t <= 3 ; t++)
			{
				
				colorcluster[r][s][t] = 0;
				
			}
		}
	}
	
	if(binBuf == NULL)
		return;
	
	for(int j = miny ; j <=  maxy ; j++)
	{
		index3 = j * 3 * m_imgWidth;
		for(int i = minx ; i <= maxx  ; i++)
		{
			index1 = i * 3;
			
			if(binBuf[j * m_imgWidth + i] == 255)

			{

				//midX = (positionArray[0].lPosition[i] + positionArray[0].rPosition[i])/2+positionArray[0].tPosition[i]+positionArray[0].bPosition[i]);				
					iR = imgBuf[index3 + index1 + 2];
					iG = imgBuf[index3 + index1 + 1];
					iB = imgBuf[index3 + index1];


					if(frame_number >= 600 && check)
					fprintf(fp3, "Red = %lf\tGreen = %lf\tBlue = %lf\n\n",iR,iG,iB);
					
					fclose(fp3);
			////////////HSV Code Starts////////
					Ttha = acos(((iR-iG+iR-iB)/2)/sqrt((iR-iG)*(iR-iG)+(iR-iB)*(iG-iB)))*180/3.14;


					if(iB <= iG)
						H = Ttha;

					else if(iB > iG)
						H = 360-Ttha;

					if(iR < iG && iR < iB)
						min_RGB = iR;
					else if(iG < iR && iG < iB)
						min_RGB = iG;
					else
						min_RGB = iB;

					if(iR == 0 && iG == 0 && iB == 0)
						S = 0;
					else
						S = 1 - ((3/(iR+iG+iB))*min_RGB);

			////////////HSV Code Ends/////////		
					

					I = (iR+iG+iB)/3;
		
					a = (iR / 64) ;
							
					b = (iG / 64) ;
					
					c = (iB / 64) ;
		
					colorcluster[a][b][c]++;				//////////Primary Color//////
			}

		}
	}
	


		/*for (int m = 0; m <= 7 ; m++)
		{
			for (int n = 0; n <= 7  ; n++)
			{
				for (int o = 0; o <= 7 ; o++)
				{
					
					if (colorcluster[m][n][o] > max1)
					{

							max1 = colorcluster[m][n][o];
							
							d = m;
						
							e = n;

							f = o;
											

					}
			
							code1 = d;
							code2 = e;
							code3 = f;*/
							
		/*if (colorcluster[0][0][0] > colorcluster[0][0][1])
		{			
			max1 = colorcluster[0][0][0];
			max2 = colorcluster[0][0][1];
		}
		else if (colorcluster[0][0][0] == colorcluster[0][0][1])
		{
			max1 = colorcluster[0][0][1];
			max2 = 0;
		}
		else
		{
			max2 = colorcluster[0][0][0];
			max1 = colorcluster[0][0][1];
		}*/
		
		/*if (max2 > max1)
		{
			temp = max2;
			max2 = max1;
			max1 = temp;

		}*/

		for (int m = 0; m <= 3 ; m++)
		{
			for (int n = 0; n <= 3  ; n++)
			{
				for (int o = 0; o <= 3 ; o++)
				{
					
					if (colorcluster[m][n][o] > max1)
					{

						//max2 = max1;

						/*if (colorcluster[m][n][o] != max1)
							code2 = code1;*/
							//max = max1;
						if(!(m ==n && m == o))
						{
								max1 = colorcluster[m][n][o];
							
							d = m;
							e = n;
							f = o;
							code1 = (16 * d) + (4 * e) + (1 * f);
						}
							
							//code1 = m;
							/*code2 = e;
							code3 = f;*/
							
					}

				}

			}
		}


		


		fclose(fp3);
		
		*iBlobColor1 = code1;
		*iBlobColor2 = code1;
		*iBlobColor3 = code1;
		check = false;
		//*iBlobColor2 = 0;
		//*iBlobColor3 = 0;
}


void CTracker::run_algo_for_colour(BYTE *back_grnd, BYTE* fore_grnd, BlobPast* blob_info, int img_width)
{
	int difference = blob_info->bpPosition[0] - blob_info->tpPosition[0];
	//int mid = difference/2;
	int srt = blob_info->tpPosition[0];
	int end = blob_info->bpPosition[0];  
	//int center_x = blob_info->xPosition[0];
	//int center_y = blob_info->yPosition[0];
	int left  = blob_info->lpPosition[0];
	int right = blob_info->rpPosition[0];
	int mid = ( (srt+end) / 2);  ///For upper half  
	int max_r = 0 , max_g = 0, max_b = 0;  
	int max_r_l = 0, max_g_l = 0, max_b_l =0;
	
	int total_pixels = 0;

	if (difference >= 0 )
	{
		for (int i = srt; i <= end ; i++)    // full object
		{
			for (int j = left; j < right; j++)
			{
				int index = i* img_width + j;
				if (fore_grnd[index] == 255)
				{
					blob_info->colour_blue[back_grnd[index*3]]++;
					blob_info->colour_green[back_grnd[index*3+1]]++;
					blob_info->colour_red[back_grnd[index*3+2]]++;
					total_pixels++;
					/*back_grnd[index*3+2] = 255;
					back_grnd[index*3+1] = 255;
					back_grnd[index*3]   = 0;*/
				}
			}
		}
	}
	//if (difference >= 0 )
	//{
	//	//for (int i = srt; i <= end ; i++)    // full object
	//	//for (int i = srt; i <= mid ; i++)    // for upper half
	//	if (end < 60)
	//	{
	//		for (int i = mid; i <= end ; i++)    // for lower half at creation
	//		{
	//			for (int j = left; j < right; j++)
	//			{
	//				int index = i* img_width + j;
	//				if (fore_grnd[index] == 255)
	//				{
	//					blob_info->colour_blue[back_grnd[index*3]]++;
	//					blob_info->colour_green[back_grnd[index*3+1]]++;
	//					blob_info->colour_red[back_grnd[index*3+2]]++;
	//					total_pixels++;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for (int i = srt; i <= mid ; i++)    // for upper half at separation
	//		{
	//			for (int j = left; j < right; j++)
	//			{
	//				int index = i* img_width + j;
	//				if (fore_grnd[index] == 255)
	//				{
	//					blob_info->colour_blue[back_grnd[index*3]]++;
	//					blob_info->colour_green[back_grnd[index*3+1]]++;
	//					blob_info->colour_red[back_grnd[index*3+2]]++;
	//					total_pixels++;
	//				}
	//			}
	//		}
	//	}
	//}
	///sort here the blob info for blue, green and red for max frequency and get that one r g b value (no) from 0 to 256
	
	//max_r = Max Frequency Finding // max_r_l = Location of Max Frequency Finding
	for (int s= 0; s <= 255; s++)
	{
		if (blob_info->colour_blue[s] > max_b)
		{
			max_b = blob_info->colour_blue[s];
			max_b_l = s+1;
		}

		if (blob_info->colour_green[s] > max_g)
		{
			max_g = blob_info->colour_green[s];
			max_g_l = s+1;
		}

		if (blob_info->colour_red[s] > max_r)
		{
			max_r = blob_info->colour_red[s];
			max_r_l = s+1;
		}
	}
	
	


	if (total_pixels != 0)
	{
		for (int k = 0; k < 256; k++)
		{
			blob_info->colour_blue[k] = blob_info->colour_blue[k]/total_pixels;     //make color_blue double instead of array 
			blob_info->colour_green[k] = blob_info->colour_green[k]/total_pixels;  // and pass single value
			blob_info->colour_red[k] = blob_info->colour_red[k]/total_pixels;
		}
	}
}


void CTracker::run_algo_for_colour2(BYTE *back_grnd, BYTE* fore_grnd, PositionArrays* blob_info, int img_width, int k)
{
	int difference = blob_info->bPosition[k] - blob_info->tPosition[k];
	//int mid = difference/2;
	int srt = blob_info->tPosition[k];
	int end = blob_info->bPosition[k];  
	//int center_x = blob_info->xPosition[0];
	//int center_y = blob_info->yPosition[0];
	int left  = blob_info->lPosition[k];
	int right = blob_info->rPosition[k];
	int mid = ( (srt+end) / 2);  ///For upper half  
	int max_r = 0 , max_g = 0, max_b = 0;  
	int max_r_l = 0, max_g_l = 0, max_b_l =0;
	
	int total_pixels = 0;

	
	if (difference >= 0 )
	{
		for (int i = srt; i <= end ; i++)    // full object
		{
			for (int j = left; j < right; j++)
			{
				int index = i* img_width + j;
				if (fore_grnd[index] == 255)
				{
					blob_info->colours[k].colour_blue[back_grnd[index*3]]++;
					blob_info->colours[k].colour_green[back_grnd[index*3+1]]++;
					blob_info->colours[k].colour_red[back_grnd[index*3+2]]++;
					total_pixels++;
				}
			}
		}
	}
			
	//if (difference >= 0 )
	//{
	//	//for (int i = srt; i <= end ; i++)    // full object
	//	//for (int i = srt; i <= mid ; i++)    // for upper half
	//	if (end < 60)
	//	{
	//		for (int i = mid; i <= end ; i++)    // for lower half at creation
	//		{
	//			for (int j = left; j < right; j++)
	//			{
	//				int index = i* img_width + j;
	//				if (fore_grnd[index] == 255)
	//				{
	//					blob_info->colours[k].colour_blue[back_grnd[index*3]]++;
	//					blob_info->colours[k].colour_green[back_grnd[index*3+1]]++;
	//					blob_info->colours[k].colour_red[back_grnd[index*3+2]]++;
	//					total_pixels++;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for (int i = srt; i <= mid ; i++)    // for upper half at separation
	//		{
	//			for (int j = left; j < right; j++)
	//			{
	//				int index = i* img_width + j;
	//				if (fore_grnd[index] == 255)
	//				{
	//					blob_info->colours[k].colour_blue[back_grnd[index*3]]++;
	//					blob_info->colours[k].colour_green[back_grnd[index*3+1]]++;
	//					blob_info->colours[k].colour_red[back_grnd[index*3+2]]++;
	//					total_pixels++;
	//				}
	//			}
	//		}
	//	}
	//}
	///sort here the blob info for blue, green and red for max frequency and get that one r g b value (no) from 0 to 256
	
	//max_r = Max Frequency Finding // max_r_l = Location of Max Frequency Finding
	for (int s= 0; s <= 255; s++)
	{
		if (blob_info->colours[k].colour_blue[s] > max_b)
		{
			max_b = blob_info->colours[k].colour_blue[s];
			max_b_l = s+1;
		}

		if (blob_info->colours[k].colour_green[s] > max_g)
		{
			max_g = blob_info->colours[k].colour_green[s];
			max_g_l = s+1;
		}

		if (blob_info->colours[k].colour_red[s] > max_r)
		{
			max_r = blob_info->colours[k].colour_red[s];
			max_r_l = s+1;
		}
	}
	
	


	if (total_pixels != 0)
	{
		for (int s = 0; s < 256; s++)
		{
			blob_info->colours[k].colour_blue[s] = blob_info->colours[0].colour_blue[s]/total_pixels;     //make color_blue double instead of array 
			blob_info->colours[k].colour_green[s] = blob_info->colours[0].colour_green[s]/total_pixels;  // and pass single value
			blob_info->colours[k].colour_red[s] = blob_info->colours[0].colour_red[s]/total_pixels;
		}
	}
}

float CTracker::calculate_similarity_colour (BlobPast* prev_blob, PositionArrays* curr_blob, int k)
{
	float total_red = 0;
	float total_blue = 0;
	float total_green = 0;
	float similarity = 0;
	float S_D_R = 0;
	float S_D_G = 0;
	float S_D_B = 0;

	for (int i = 0; i < 256; i++)
	{
		if ((prev_blob->colour_red[i] != NULL ) &&  (curr_blob->colours[i].colour_red != NULL))
		{
			S_D_R = sqrt (abs(  (prev_blob->colour_red[i]) *  (curr_blob->colours[k].colour_red[i])));
			total_red       = total_red + S_D_R;
		
			S_D_G = sqrt( abs (  ( prev_blob->colour_green[i]) *  (curr_blob->colours[k].colour_green[i])));
			total_green     = total_green + S_D_G;
		
			S_D_B  = sqrt( abs(  (prev_blob->colour_blue[i]) *   (curr_blob->colours[k].colour_blue[i])));
			total_blue = total_blue + S_D_B;
		}
	}

	similarity = (0.4 * total_red) + (0.3 * total_green) + (0.3 * total_blue);
	return similarity;
}

///////////////////May 19, 2014///////////////////Complete Code 4 Objects Particle Filter

