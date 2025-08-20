// Avi.cpp: implementation of the CAvi class.
//
//////////////////////////////////////////////////////////////////////
//	압축되지 않은 avi파일에서 영상 한장찍 꺼내는 클래스 //////////////
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Avi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAvi::CAvi()
{
	m_frameWidth = 0;
	m_frameHeight = 0;
	m_curFrame = 0;
	m_totalFrameNum = 0;
}

CAvi::~CAvi()
{

}

BOOL CAvi::IniReadAvi(char* fileName)
{
	AVIFileInit();

    PAVIFILE avi;
    int res = AVIFileOpen(&avi, fileName, OF_READ, NULL);

    if (res!=AVIERR_OK)
    {
        //an error occures
        if (avi!=NULL)
            AVIFileRelease(avi);
        
        return FALSE;
    }

    AVIFILEINFO avi_info;
    AVIFileInfo(avi, &avi_info, sizeof(AVIFILEINFO));

	/*
    CString szFileInfo;
    szFileInfo.Format("Dimention: %dx%d\n"
                      "Length: %d frames\n"
                      "Max bytes per second: %d\n"
                      "Samples per second: %d\n"
                      "Streams: %d\n"
                      "File Type: %d", avi_info.dwWidth,
                            avi_info.dwHeight,
                            avi_info.dwLength,
                            avi_info.dwMaxBytesPerSec,
                            (DWORD) (avi_info.dwRate / avi_info.dwScale),
                            avi_info.dwStreams,
                            avi_info.szFileType);

    AfxMessageBox(szFileInfo, MB_ICONINFORMATION | MB_OK);
	*/

    
    res = AVIFileGetStream(avi, &m_pStream, streamtypeVIDEO /*video stream*/, 
                                               0 /*first stream*/);

    if (res != AVIERR_OK)
    {

        if (m_pStream != NULL)
            AVIStreamRelease(m_pStream);

        AVIFileExit();
        return FALSE;
    }    

    m_curFrame = AVIStreamStart(m_pStream);
    if (m_curFrame == -1)
    {
        //Error getteing the frame inside the stream
        if (m_pStream != NULL)
            AVIStreamRelease(m_pStream);

        AVIFileExit();
        return FALSE;
    }

    m_totalFrameNum = AVIStreamLength(m_pStream);
    if (m_totalFrameNum == -1)
    {
        //Error getteing the number of frames inside the stream
        
        if (m_pStream != NULL)
            AVIStreamRelease(m_pStream);
        
        AVIFileExit();
        return FALSE;
    }    
    
    m_pFrame = AVIStreamGetFrameOpen(m_pStream, 
           NULL/*(BITMAPINFOHEADER*) AVIGETFRAMEF_BESTDISPLAYFMT*/ /*&bih*/);
    
    return TRUE;
}

BOOL CAvi::GetFrame(LPBYTE pImage)
{	
    BYTE* pDIB = (BYTE*) AVIStreamGetFrame(m_pFrame, m_curFrame);
	
	//get the BitmapInfoHeader
    BITMAPINFOHEADER bih;
    RtlMoveMemory(&bih.biSize, pDIB, sizeof(BITMAPINFOHEADER));

	m_frameWidth = bih.biWidth;
	m_frameHeight = bih.biHeight;
	
    //now get the bitmap bits
    if (bih.biSizeImage < 1)
    {
        return FALSE;
    }
	
    BYTE* Bits = new BYTE[bih.biSizeImage];
	
    RtlMoveMemory(Bits, pDIB + sizeof(BITMAPINFOHEADER), bih.biSizeImage);

	CopyMemory(pImage, Bits, m_frameWidth * m_frameHeight * 3);

	if ((m_curFrame++) > m_totalFrameNum) {
		m_curFrame = 0;
	}
    delete [] Bits;    

	return TRUE;
}

BOOL CAvi::DestroyAvi()
{
	AVIStreamGetFrameClose(m_pFrame);
	
    //close the stream after finishing the task
    if (m_pStream!=NULL)
        AVIStreamRelease(m_pStream);
	
    AVIFileExit();

	return TRUE;
}