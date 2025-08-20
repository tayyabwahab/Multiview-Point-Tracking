// Tracker.h: interface for the CTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACKER_H__67197302_0EF2_4136_88ED_64ECBA8343BB__INCLUDED_)
#define AFX_TRACKER_H__67197302_0EF2_4136_88ED_64ECBA8343BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Imgproc.h"
#include <sys/timeb.h>
#include <math.h>
#include <vector>


#define HISTORY_NUM 60
#define PERSON  0
#define VEHICLE  1
#define PERSONS  2
#define ETC   3


class CPosQueue
{
public:
	CPosQueue();
	virtual ~CPosQueue();

	void	SetQueue(int Qsize);
	BOOL	GetPosition(CPoint *point);
	BOOL	PutPosition(CPoint point);
	BOOL	PutPosition(int x, int y);
	void	ClearAllData();

private:
	int		m_iHeader;
	int		m_iTail;
	int		m_iQueueSize;
	CPoint	m_Queue[10];
};


typedef struct _tagVelocity {
	double		xDirection;
	double		yDirection;
}Velocity;

typedef struct _tagBlobInfo {
	int			iKindOfBlob;
	CPoint		posQ[HISTORY_NUM];
	int			m_header;
	int			m_tail;
	CPoint		centerPos;
	Velocity	blobVelocity;
	CRect		rect;
	int			id;
	DWORD		label;
	int			size;
	BYTE		dominantColor1;
	BYTE		dominantColor2;
	BYTE		dominantColor3;
	double		appearTime;
	BOOL		isMatched;
	int			blobType;
	int			matchingCnt;
	DWORD		matchID[5];
	CPoint		historyPos[10];
	int			historyCount;
	_tagBlobInfo*	nextBlob;
	_tagBlobInfo*	matchingBlob[5];
	CString		name;
	float		colour_blue[256];
	float		colour_red[256];
	float		colour_green[256];
}BlobInfo;


class CLabeling
{
public:
	CLabeling(int imgWidth, int imgHeight);
	virtual ~CLabeling();
	int Labeling(BYTE* imgBuf,BYTE* binBuf,int minSize, int maxSize, int frameCnt, BlobInfo *pBlob);
	//int Labeling(BYTE* imgBuf,BYTE* binBuf,int minSize, int maxSize, int frameCnt);
	bool horizontalMergeDetection(BYTE * imgBuf,BYTE* fgBuf, int m_Width, int m_Height, int x, int y, int left, int right, int* count, std::vector<int>* positions);
	void SobelEdge1(BYTE *src, BYTE *edge,int W, int H, int x, int y,  BYTE *mask);
	bool cutTheMergedBlobs(int index2, int * iDiv,int width, int height, int left, int right, BYTE* bEdgeProjectionMap, int* count, std::vector<int>* positions  );
	DWORD*			m_labelBuf;
	int				m_curBlobNum;
private:
	int				m_imgWidth;
	int				m_imgHeight;
	BOOL			*m_validID;
	int				m_tranningNum;
	int				m_frameCount;
	int				KindOfBlob(int Size, int Height,double Ratio);
	double			GetBlobTime();
	int				*sizeBuf;	
	int				*m_iFilteringWidth;
	int				*m_iFilteringHeight;
	int				*m_iFilteringSize;

};

#endif // !defined(AFX_TRACKER_H__67197302_0EF2_4136_88ED_64ECBA8343BB__INCLUDED_)
