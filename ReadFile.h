#ifndef ReadFile_H
#define ReadFile_H

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#define DATA_ROUTINE			"f:\\data\\back_sunny\\aaa%04d.bmp"
#define DATA_SIZE				50

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#pragma comment(lib, "winmm.lib")


class CReadFile
{
public:
	CReadFile(int imgWidth, int imgHeight, int colorBand);
	~CReadFile();
	void		RunReadFile(char file_name[DATA_SIZE], BYTE *inImgBuf);
	
private:
	int			m_imgWidth;
	int			m_imgHeight;
	int			m_colorBand;		
};


#endif