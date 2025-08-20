//////////////////////////////////////////////////////////////
//	image processing functions	//////////////////////////////
//////////////////////////////////////////////////////////////


//#include <windows.h>
#include <Afxcoll.h>
#include "Imgproc.h"


void Binarize(BYTE *src, BYTE *des,int W, int H, int threshold);
void ArithOperation(BYTE *src1,BYTE *src2, BYTE *des,int W, int H, int operation);
int Pop(int *stackX, int *stackY, int *top, int *x, int *y);
int Push(int *stackX, int *stackY, int *top, int x, int y);
void BinDilation(BYTE *img,int W, int H, int repeatNum);
void BinErosion(BYTE *img,int W, int H, int repeatNum);
void SobelEdge(BYTE *src, BYTE *edge,int W, int H);
void SobelEdgeEx(BYTE *src, BYTE *edge, BYTE *fgBuf,int W, int H);
void MedianFilter(BYTE *img,int W, int H, int maskSize);
void QuickSort(BYTE array[], int left, int right);



int Pop(int *stackX, int *stackY, int *top, int *x, int *y)
{
	if (*top == 0)	return -1;
	
	*x = stackX[*top];
	*y = stackY[*top];
	
	(*top)--;
	return 1;
}

int Push(int *stackX, int *stackY, int *top, int x, int y)
{
	(*top)++;
	stackX[*top] = x;
	stackY[*top] = y;
	return 1;
}

void SobelEdgeEx(BYTE *src, BYTE *edge, BYTE *fgBuf,int W, int H)
{
	int i,j,index,index1,index2;
	
	ZeroMemory(edge,W*H);
	
	for(j = 1 ; j < H-1 ; j++)
	{
		index = j*W;
		index1 = (j-1)*W;
		index2 = (j+1)*W;
		for(i = 1 ; i < W-1 ; i++)
		{
			if (fgBuf[index1 + i] != 0) 
			{
				edge[index + i] = abs(src[index1 + i - 1] + 2*src[index + i - 1] + 
					src[index2 + i - 1] - src[index1 + i + 1] - 
					2*src[index + i + 1] - src[index2 + i + 1]) / 4;
				
				edge[index + i] += abs(src[index1 + i - 1] + 2*src[index1 + i] + 
					src[index1+ i + 1] - src[index2 + i - 1] - 
					2*src[index2 + i] - src[index2 + i + 1]) / 4;			
			}
		}
	}
}

void  SobelEdge(BYTE *src, BYTE *edge,int W, int H, BYTE *mask)
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
			if(mask[index + i] == 255)
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

void QuickSort(BYTE array[], int left, int right)
{
	int i, j; 
	BYTE temp; 
	BYTE pivot; 
	
	if(left < right) { 
		pivot = array[right]; 
		
		i = left - 1; 
		j = right; 
		
		do {          
			
			do { 
				i++; 
			} while(array[i] < pivot);   
			
			do { 
				j--; 
			} while(array[j] > pivot); 
			
			if(i < j) { 
				temp = array[i]; 
				array[i] = array[j]; 
				array[j] = temp; 
			} else { 
				break; 
			} 
		} while(i < j); 
		
		temp = array[i]; 
		array[i] = array[right]; 
		array[right] = temp; 
		
		QuickSort(array, left, i - 1); 
		QuickSort(array, i + 1, right); 
	} 
}

void SobelEdge(BYTE *src, BYTE *edge,int W, int H)
{
	int i,j,index,index1,index2;
	
	ZeroMemory(edge,W*H);
	
	for(j = 1 ; j < H-1 ; j++)
	{
		index = j*W;
		index1 = (j-1)*W;
		index2 = (j+1)*W;
		for(i = 1 ; i < W-1 ; i++)
		{
			edge[index + i] = abs(src[index1 + i - 1] + 2*src[index + i - 1] + 
				src[index2 + i - 1] - src[index1 + i + 1] - 
				2*src[index + i + 1] - src[index2 + i + 1]) / 4;
			
			edge[index + i] += abs(src[index1 + i - 1] + 2*src[index1 + i] + 
				src[index1+ i + 1] - src[index2 + i - 1] - 
				2*src[index2 + i] - src[index2 + i + 1]) / 4;			
		}
	}
}

void BinErosion(BYTE *img,int W, int H, int repeatNum)
{
	int i,j,n,r,c,index,index1;
	int sum;
	BYTE *resImg = new BYTE[W*H];	
	
	for(n = 0; n < repeatNum; n++)
	{
		for(j = 1; j < H - 1; j++)
		{
			index = j*W;
			for(i = 1; i < W - 1; i++)
			{
				sum = 0;
				for(r = -1; r < 2; r++)
				{
					//index1 = (j + r) * W + i;
					for(c = -1; c < 2; c++)
					{
						sum += img[(j + r) * W + i + c];
					}
				}
				
				if ((sum/8) >= 255) 
				{
					resImg[index + i] = 255;
				}
				else
				{
					resImg[index + i] = 0;
				}
			}
		}
	}

	CopyMemory(img,resImg,W*H);
	
	delete []resImg;
}

void BinDilation(BYTE *img,int W, int H, int repeatNum)
{
	int i,j,n,r,c,index,index1;
	int sum;
	BYTE *resImg = new BYTE[W*H];
		
	for(n = 0; n < repeatNum; n++)
	{
		for(j = 1; j < H - 1; j++)
		{
			index = j*W;
			for(i = 1; i < W - 1; i++)
			{
				sum = 0;
				for(r = -1; r < 2; r++)
				{
					//index1 = (j + r) * W + i;
					for(c = -1; c < 2; c++)
					{
						sum += img[(j + r) * W + i + c];
					}
				}
				
				if (sum > 0) 
				{
					resImg[index + i] = 255;
				}
				else
				{
					resImg[index + i] = 0;
				}
			}
		}
	}
	
	CopyMemory(img,resImg,W*H);
	
	delete []resImg;
}

void MedianFilter(BYTE *img,int W, int H, int maskSize)
{
	int x, y, dx, dy;
	BYTE *Buf;
	int cnt;
	BYTE *out;
	
	out = new BYTE [W*H];	
	Buf = new BYTE [maskSize*maskSize];
	
	for(y = 0 ; y < H ; y++)
		for(x = 0 ; x < W ; x++)
		{
			cnt = 0;
			for(dy = maskSize / 2 * -1 ; dy < (double)maskSize / 2. ; dy++)
				for(dx = maskSize / 2 * -1 ; dx < (double)maskSize / 2. ; dx++)
					Buf[cnt++] = img[((y+dy+H)%H)*W+((x+dx+W)%W)];
				
				QuickSort(Buf, 0, cnt-1);
				
				out[y*W+x] = Buf[cnt/2];
		}
		
		for(y = 0 ; y < H ; y++)
			for(x = 0 ; x < W ; x++)
				img[y*W+x] = out[y*W+x];
			
			delete Buf;
			delete out;
}

void Binarize(BYTE *src, BYTE *des,int W, int H, int threshold)
{
	int x,y,index;
	
	for(y = 0; y < H; y++)
	{
		index = y*W;
		for(x = 0; x < W; x++)
		{
			if (src[index+x] >= threshold) 
			{
				des[index+x] = 255;
			}
			else
			{
				des[index+x] = 0;
			}
		}
	}
}

void ArithOperation(BYTE *src1,BYTE *src2, BYTE *des,int W, int H, int operation)
{
	int i,j,index;
	
	switch(operation) 
	{
	case I_AND:
		
		for(j = 0; j < H; j++)
		{
			index = j*W;			
			for(i = 0; i < W; i++)
			{				 
				des[index+i] = src1[index+i] & src2[index+i];				
			}
		}
		
		break;
	case I_OR:
		
		for(j = 0; j < H; j++)
		{
			index = j*W;
			
			for(i = 0; i < W; i++)
			{
				des[index+i] = src1[index+i] | src2[index+i];					
			}
		}
		
		break;
	case I_XOR:
		
		for(j = 0; j < H; j++)
		{
			index = j*W;
			for(i = 0; i < W; i++)
			{
				des[index+i] = src1[index+i] ^ src2[index+i];
			}
		}
		
		break;
	case I_SUB_ABS:
		
		for(j = 0; j < H; j++)
		{
			index = j*W;
			for(i = 0; i < W; i++)
			{				
				des[index+i] = abs(src1[index+i] - src2[index+i]);				
			}
		}
		
		break;	
	}
}

void ReverseTopAndBottom(BYTE *src,int W, int H,int colorBand)
{
	int x,y,d,index,index1;
	BYTE *tmpBuf;

	tmpBuf = new BYTE[W*H*colorBand];
	
	for(y = 0; y < H; y++)
	{
		index = y*W*colorBand;
		index1 = (H-y-1)*W*colorBand;
		
		for(x = 0; x < W; x++)
		{
			for(d = 0; d < colorBand; d++)
			{
				tmpBuf[index+colorBand*x+d] = src[index1+colorBand*x+d];
			}
		}
	}

	CopyMemory(src,tmpBuf,W*H*colorBand);

	delete []tmpBuf;
}


