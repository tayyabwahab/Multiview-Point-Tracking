#ifndef MOG_H
#define MOG_H

//#include <windows.h>
#include <Afxcoll.h>

#define MAXGAUNUM  3					// maximum number of gaussian models
#define FOREGROUND 255					// foreground is full white color
#define BACKGROUND	0					// background is full black color
#define CHECKRANGE  10




typedef struct 
{
	double m_weight[MAXGAUNUM];			// weight array of each gaussian model
	double m_mean[MAXGAUNUM][3];	// expectation value array of each gaussian model
	double m_stdVar[MAXGAUNUM];			// square of deviation array of each gaussian model
	int	  m_priRank[MAXGAUNUM];			// sort the class from high priority to low
	double m_priority[MAXGAUNUM];		// sorting weight : weight ^ 2 / standard deviation ^ 2
	int   m_validModels;				// current valid model number
}PixelPara;

class CMog
{
public:
	CMog(int imgWidth, int imgHeight, int colorBand);
	~CMog();
	void		STGMM(BYTE *inImgBuf, BYTE *foregroundBUf, BYTE *Background, BYTE * StableBuf);
	void		RunMog(BYTE *inImgBuf, BYTE *maskBuf, BYTE *upDateBuf, BYTE *foregroundBUf);
	void		GetBgImage(BYTE *bgBuf);
	
private:
	double		m_IniWeight;
	double		m_Threshold;
	double		m_Lamda;
	double		m_Alpa;
	double		m_IniStd;
	double		m_maxStd;
	int			m_RunningFrame;

	
	BYTE		*PreFG;
	int			iThres;
	int			iCheckFrame;
	int			iprocessingframe;


	int			m_imgWidth;
	int			m_imgHeight;
	int			m_colorBand;
	
	PixelPara	*m_pixelParam;
	BYTE		*m_curBackGroundImg;
};


#endif