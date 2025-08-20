// Tracker.h: interface for the CTracker class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Imgproc.h"
#include <sys/timeb.h>
#include <math.h>
#include "StdAfx.h"
#include "Labeling.h"


#define SINGLE_BLOB 0
#define MERGE_BLOB 1
#define SPLIT_BLOB 2
#define HISTORY_NUM 60






typedef struct _tagColour{
	float		colour_blue[256];
	float		colour_red[256];
	float		colour_green[256];
}ColoursArray;

typedef struct _tagPosition {
	int			merged_Blobs[5];
	int			demerged_Blobs[5];
	double		xPosition[10];
	double		yPosition[10];
	double		tPosition[10];
	double		bPosition[10];
	double		lPosition[10];
	double		rPosition[10];
	BYTE		dominantColor;
	BYTE		dominantColor1[10];
	BYTE		dominantColor2[10];
	BYTE		dominantColor3[10];
	bool		check_new_blob[10];
	double		prev_xPosition[10];
	double		prev_yPosition[10];
	ColoursArray colours[10];
}PositionArrays;



typedef struct _tagBlobPast {

	int			id;
	int			merg_id[5];
	int			merged_counter;
	float		merge_color_red[256];
	float		merge_color_green[256];
	float		merge_color_blue[256];
	int			merged_blobs[5];
	int			demerged_blobs[5];
	bool		isInitiated;
	bool		isInFrame;
	double		xPosition[2];
	double		yPosition[2];
	double		tpPosition[2];
	double		bpPosition[2];
	double		lpPosition[2];
	double		rpPosition[2];
	double		predictedX;
	double		predictedY;
	double		previousX[35];
	double		previousY[35];
	double		tPosition[35];
	double		bPosition[35];
	double		lPosition[35];
	double		rPosition[35];
	int			blobDirection;
	int			checker;
	BYTE		dominantColor1;
	BYTE		dominantColor2;
	BYTE		dominantColor3;
	CString		name_blob;
	int			blob_size;
	int			merged;
	int			demerged;
	bool		isMerged;
	bool		isDemerged;
	float		colour_blue[256];
	float		colour_red[256];
	float		colour_green[256];
	int			histoty_count;
	bool		already_merged;
}BlobPast;

class CTracker  
{
public:
	CTracker(int imgWidth, int imgHeight);
	virtual ~CTracker();

	//void Labeling(BYTE* imgBuf,BYTE* binBuf,int minSize, int maxSize, int frameCnt, BlobInfo *pBlob);
	void Tracking(BYTE* imgBuf,BYTE* fgBuf,int minSize, int maxSize, int frameCnt);
	void run_algo_for_colour(BYTE *back_grnd, BYTE* fore_grnd, BlobPast* blob_info, int img_width);
	void run_algo_for_colour2(BYTE *back_grnd, BYTE* fore_grnd, PositionArrays* blob_info, int img_width, int k);
	//float calculate_similarity_colour (BlobPast* prev_blob, ColoursArray* curr_blob);
	float calculate_similarity_colour (BlobPast* prev_blob, PositionArrays* curr_blob, int k);
	int GetCurBlob(BlobInfo *pBlob);
	PositionArrays getPositionArray();
	void deleteArray();
	void InsertKFBlobInformation(BlobInfo *src, BlobPast *des, int BlobCount, BYTE *input, BYTE *fgBuf);

	BOOL			GetPosition(BlobInfo *pBlob,CPoint *point);
	int				m_checker[15];
	BYTE*           graybuf;
	BYTE*			sobelEdge;
	BYTE*			bEdgeProjectionMap;
	BYTE*			m_updateBuf;
	BYTE*			m_edgeBuf;
	BYTE*			m_tmpEdgeBuf;
	BYTE*			m_curEdgeBuf;
	BYTE*			m_tmpUpdataBuf;
	DWORD*			m_labelBuf;
	BlobInfo		*m_blobPool;
	BlobInfo		*m_curBlob;
	int				m_curBlobNum;
	int				m_t_number;
	int				m_trackingBlobNum;
	PositionArrays	*positionArray;
	BlobPast*		getBlobs( int, BYTE*, BYTE*);
	void			kalman(int j);
	int				m_currentBlob_number;
	BlobPast*		blobs;
	int				m_totalBlobNum;
	int				getTotalBlobNum();
	void			addBlob();
	void			GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *imgBuf, BYTE *binBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2 , BYTE *iBlobColor3);
	//void			GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *imgBuf, BYTE *fgBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2 , BYTE *iBlobColor3);
	//void			GetObjectColor(int minx, int maxx, int miny, int maxy, BYTE *input, BYTE *fgBuf, BYTE *iBlobColor1 , BYTE *iBlobColor2 , BYTE *iBlobColor3);
	void			delBlob(int);
	int				m_max;
	int				*m_iFilteringWidth;
	int				*m_iFilteringHeight;
	int				*m_iFilteringSize;
	

	

private:
	int				m_imgWidth;
	int				m_imgHeight;
	int				m_maxTrackingNum;
	BOOL			*m_validID;
	int				m_tranningNum;
	int				m_frameCount;
	
	//BOOL	isOverlap(BYTE *binBuf,BlobInfo *pBlob, int xOffset, int yOffset);
	//void	SetBlobInfo(BlobInfo *desBlob, BlobInfo *srcBlob,BYTE *fgBuf,int blobType);
	BOOL	CorrelationEx(BlobInfo *pBlob,CRect rect,int xOffset,int yOffset);
	//BOOL	IsGhost(BYTE* binEdge,BlobInfo blob);
	BYTE	GetDominantColor(BlobInfo *pBlob, BYTE *imgBuf, BYTE *fgBuf);
	
	

	//double	GetBlobTime();
	void	CalcVelocity(BlobInfo *pBlob, int xDis, int yDis);
	//int		GetMovingXDis(BlobInfo *pBlob);
	//int		GetMovingYDis(BlobInfo *pBlob);
	int		GetValidID();
	double	*sizeBuf;
	void	ResetValidID(int id);
	void	DeleteBlob(BlobInfo *pBlob);
};

