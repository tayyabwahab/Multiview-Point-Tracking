#include "StdAfx.h"
#include "ReadFile.h"


CReadFile::CReadFile(int imgWidth, int imgHeight,int colorBand)
{
	m_imgWidth = imgWidth;
	m_imgHeight = imgHeight;
	m_colorBand = colorBand;
	
}

CReadFile::~CReadFile()
{
}

void	CReadFile::RunReadFile(char file_name[DATA_SIZE], BYTE *inImgBuf)
{

	FILE				*in_file;
	BITMAPFILEHEADER	hf;			
	BITMAPINFOHEADER	hInfo;	

	in_file = fopen(file_name, "rb");

	if(in_file == NULL)
	{		
		AfxMessageBox(_T("ERROR"),MB_OK ); 
		return;
	}

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, in_file);
	if(hf.bfType != 0x4D42)		
	{
		AfxMessageBox(_T("ERROR"),MB_OK ); 
		exit(1);    
	}
	
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, in_file);
	int resize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);
	int resize2 = WIDTHBYTES(24 * hInfo.biWidth);

	BYTE *IpImg = new BYTE[3 * resize * hInfo.biHeight];

	fread(IpImg, sizeof(char), hInfo.biSizeImage, in_file);
	fclose(in_file);

	for(int j = 0 ; j < m_imgHeight ; j++)
	{
		for(int i = 0 ; i < m_imgWidth; i++)
		{
			inImgBuf[j * m_imgWidth * 3 + i * 3 + 2] = IpImg[(m_imgHeight - j) * m_imgWidth * 3 + i * 3 + 2];
			inImgBuf[j * m_imgWidth * 3 + i * 3 + 1] = IpImg[(m_imgHeight - j) * m_imgWidth * 3 + i * 3 + 1];
			inImgBuf[j * m_imgWidth * 3 + i * 3 ] = IpImg[(m_imgHeight- j) * m_imgWidth * 3 + i * 3 ];
		}
	}
	delete IpImg;

}