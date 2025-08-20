
#define I_AND		0
#define I_OR		1
#define I_XOR		2
#define I_SUB_ABS	3

#include <sys/timeb.h>
#include <math.h>
#include "StdAfx.h"

extern void Binarize(BYTE *src, BYTE *des,int W, int H, int threshold);
extern void ArithOperation(BYTE *src1,BYTE *src2, BYTE *des,int W, int H, int operation);
extern void ReverseTopAndBottom(BYTE *src,int W, int H, int colorBand);
extern int Pop(int *stackX, int *stackY, int *top, int *x, int *y);
extern int Push(int *stackX, int *stackY, int *top, int x, int y);
extern void BinDilation(BYTE *img,int W, int H, int repeatNum);
extern void BinErosion(BYTE *img,int W, int H, int repeatNum);
extern void SobelEdge(BYTE *src, BYTE *edge,int W, int H);
extern void SobelEdgeEx(BYTE *src, BYTE *edge, BYTE *fgBuf,int W, int H);
extern void SobelEdge(BYTE *src, BYTE *edge,int W, int H, BYTE *mask);
extern void MedianFilter(BYTE *img,int W, int H, int maskSize);