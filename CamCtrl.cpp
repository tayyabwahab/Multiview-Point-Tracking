// CamCtrl.cpp: implementation of the CCamCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CamCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BYTE g_dcPacket[MV_NUMBER][PACKET_SIZE]= {
	{0xA5,0x01,0x10,0x0d,0x98,0x55,},
	{0xA5,0x01,0x10,0x0d,0x99,0x55,},
	{0xA5,0x01,0x10,0x0d,0x89,0x55,},
	{0xA5,0x01,0x10,0x0d,0x79,0x55,},
	{0xA5,0x01,0x10,0x0d,0x78,0x55,},
	{0xA5,0x01,0x10,0x0d,0x77,0x55,},
	{0xA5,0x01,0x10,0x0d,0x87,0x55,},
	{0xA5,0x01,0x10,0x0d,0x97,0x55,},
	{0xA5,0x01,0x10,0x0d,0x88,0x55,},
	{0xA5,0x01,0x10,0x0f,0x88,0x55,},
	{0xA5,0x01,0x10,0x0b,0x88,0x55,},
	{0xA5,0x01,0x10,0x03,0x88,0x55,},
	{0xA5,0x01,0x10,0x04,0x88,0x55,},
	{0xA5,0x01,0x10,0x05,0x88,0x55,},
	{0xA5,0x01,0x10,0x06,0x88,0x55,},
	{0xA5,0x01,0x25,0x80,0x01,0x55,},
	{0xA5,0x01,0x11,0x80,0x81,0x55,},
	{0xA5,0x01,0x40,0x80,0x88,0x55,},
	{0xA5,0x01,0x41,0x00,0x00,0x00,}
};

void CQueue::Clear()
{
	m_iHead = m_iTail =0;
	memset(buff,0,BUFF_SIZE);
}
CQueue::CQueue()
{
	Clear();
}
int CQueue::GetSize()
{
	return (m_iHead - m_iTail + BUFF_SIZE) % BUFF_SIZE;
}
BOOL CQueue::PutByte(BYTE b)
{
	if(GetSize() == (BUFF_SIZE-1)) return FALSE;
	buff[m_iHead++] =b;
	m_iHead %= BUFF_SIZE;
	return TRUE;
}
BOOL CQueue::GetByte(BYTE *pb)
{
	if(GetSize() == 0) return FALSE;
	*pb = buff[m_iTail++];
	m_iTail %= BUFF_SIZE;
	return TRUE;
}
	

CCamCtrl::CCamCtrl()
{
	m_bConnected = FALSE;
	Initialize();
}

CCamCtrl::~CCamCtrl()
{

}


BOOL CCamCtrl::OpenPort(char *strPortName,DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;
	
	
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	
	if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
	{
		return FALSE;
	}


	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		return FALSE;
	}
	
	m_sPortName = strPortName;

	m_hComm = CreateFile( m_sPortName, 
						  GENERIC_READ | GENERIC_WRITE, 0, NULL,
						  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
						  NULL);


	if (m_hComm == (HANDLE) -1)
	{
		AfxMessageBox("Fail Port ofen");
		return FALSE;
	}


	SetCommMask( m_hComm, EV_RXCHAR);	

	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	

	PurgeComm( m_hComm,					
			   PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	timeouts.ReadIntervalTimeout = 0xFF; //0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts( m_hComm, &timeouts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	
	dcb.BaudRate = dwBaud;
	dcb.ByteSize = byData;
	dcb.Parity = byParity;
	dcb.StopBits = byStop;

	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	
	m_bConnected = TRUE;
	
	dwThreadID = 0;

/*	
	m_hThreadWatchComm = CreateThread( NULL, 0, 
									   (LPTHREAD_START_ROUTINE)ThreadWatchComm, 
									   this, 0, &dwThreadID);

	if (! m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
*/
	check = FALSE;

	return TRUE;
}

void CCamCtrl::ClosePort()
{
	m_bConnected = FALSE;
	SetCommMask( m_hComm, 0);
	
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	CloseHandle(m_hComm);
}

DWORD CCamCtrl::writeComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;
	
	if( !m_bConnected )		
	{
		return 0;
	}
	
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
	}
	
	return dwWritten;
}

UINT CCamCtrl::ThreadWatchComm(LPVOID pParam)
{
	CCamCtrl *pComm = (CCamCtrl *)pParam;
	DWORD           dwEvent;
	OVERLAPPED      os;
	BOOL            bOk = TRUE;
	BYTE            buff[2048];			// �б� ����
	DWORD           dwRead;				// ���� ����Ʈ��.
	
	// Event, OS ����.
	memset( &os, 0, sizeof(OVERLAPPED));
	
	//--> �̺�Ʈ ����..
	if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
	{
		bOk = FALSE;
	}
	
	//--> �̺�Ʈ ����ũ..
	if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
	{
		bOk = FALSE;
	}
	
	//--> �̺�Ʈ��..����ũ ������ ������..
	if( !bOk )
	{
		return FALSE;
	}
	
	while (pComm ->m_bConnected)
	{
		dwEvent = 0;
		
        // ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
		
		//--> �����Ͱ� ���ŵǾ��ٴ� �޼����� �߻��ϸ�..
        if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
        {
            // ��Ʈ���� ���� �� �ִ� ��ŭ �д´�.
			//--> buff �� �޾Ƴ���..
			do
			{
				dwRead = pComm->readComm(buff, 1024);
				if(BUFF_SIZE - pComm->m_QueueRead.GetSize() > (int)dwRead)
				{
					for( WORD i = 0; i < dwRead; i++ )
					{
						pComm->m_QueueRead.PutByte(buff[i]);
					}
				}
				else
				{
				}
			}while(dwRead);
//			::PostMessage(m_pHwnd, WM_COMM_READ, 0, 0 );
			
			//::SendMessage(hCommWnd, WM_COMM_READ, 0, 0 );
		}
		Sleep(0);
	}
	
	CloseHandle( os.hEvent);
	
	//--> ������ ���ᰡ �ǰ���?
	pComm->m_hThreadWatchComm = NULL;
	
	return TRUE;
}

// ��Ʈ�κ��� pBuff�� nToWrite��ŭ �д´�.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCamCtrl::readComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;
	
	//--- system queue�� ������ byte���� �̸� �д´�.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);
	
	//--> �ý��� ť���� ���� �Ÿ��� ������..
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{		
		//--> ���ۿ� �ϴ� �о���̴µ�.. ����..�о���ΰ��� ���ٸ�..		
		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			//--> ���� �Ÿ��� ��������..
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts�� ������ �ð���ŭ ��ٷ��ش�.
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}
	
	//--> ���� �о���� ������ ����.
	return dwRead;	
}

BOOL CCamCtrl::sendPacket(int index)
{
	BYTE *packet;
	int i,nWrite = 0;
	char twoChar[2];
	
	if ( m_bConnected == NULL ) return FALSE;	
	
	packet = g_dcPacket[index];	
	packet[PACKET_SIZE-1] = CheckSum (packet,PACKET_SIZE-1);	
	
	memset(m_strPacket,0,MAX_PACKET*2);
	
	for(i=0; i < PACKET_SIZE; i++)
	{
		sprintf(twoChar,"%02X", packet[i]); 
		
		m_strPacket[2*i ] = twoChar[0];
		m_strPacket[2*i+1] = twoChar[1];
	}
	
	nWrite = writeComm ( (BYTE *)packet, PACKET_SIZE);
	
	if ( nWrite != PACKET_SIZE) return FALSE;
	
	return TRUE;
}

BYTE CCamCtrl::CheckSum(BYTE *packet,int maxSize)
{
	int i;
	BYTE sum = 0 ;
	
	for(i=0; i < maxSize;i++)
	{
		sum = sum + packet[i];
	}
	
	return sum;
}

void CCamCtrl::SetPreset(int presetNum)
{
	BYTE tmpPacket[PACKET_SIZE];
	int upperNum,lowerNum;
	int nWrite = 0;

	upperNum = presetNum / 16;
	lowerNum = presetNum % 16;

	tmpPacket[0] = 0xA5;
	tmpPacket[1] = 0x01;
	tmpPacket[2] = 0x25;
	tmpPacket[3] = 1;
	tmpPacket[3] = tmpPacket[3] << 3;
	tmpPacket[3] += upperNum;
	tmpPacket[4] = 1;
	tmpPacket[4] = tmpPacket[4] << 3;
	tmpPacket[4] += lowerNum;
	tmpPacket[5] = 0x55;

	tmpPacket[6] = CheckSum(tmpPacket,PACKET_SIZE-1);
	
	nWrite = writeComm ( (BYTE *)tmpPacket, PACKET_SIZE);
}

void CCamCtrl::GoPreset(int presetNum)
{
	BYTE tmpPacket[PACKET_SIZE];
	int upperNum,lowerNum;
	int nWrite = 0;
	
	upperNum = presetNum / 16;
	lowerNum = presetNum % 16;
	
	tmpPacket[0] = 0xA5;
	tmpPacket[1] = 0x01;
	tmpPacket[2] = 0x11;
	tmpPacket[3] = 1;
	tmpPacket[3] = tmpPacket[3] << 3;
	tmpPacket[3] += upperNum;
	tmpPacket[4] = 1;
	tmpPacket[4] = tmpPacket[4] << 3;
	tmpPacket[4] += lowerNum;
	tmpPacket[5] = 0x55;
	
	tmpPacket[6] = CheckSum(tmpPacket,PACKET_SIZE-1);
	nWrite = writeComm ( (BYTE *)tmpPacket, PACKET_SIZE);
}

BOOL CCamCtrl::Initialize()
{
	double panGap[2],tiltGap[2],zoomGap;
	int panVal,tiltVal,zoomVal;
	int xGap,yGap;

	zoomGap = 0x490 - 0xd0;
	m_pixelPerPan[0] = (0x45e - 0x2a2)/108;
	m_pixelPerTilt[0] = (0x1cf - 0xb2)/66;

	panGap[1] = (0x574 - 0x456)/143;
	tiltGap[1] = (0x1cf - 0xe5)/107;

	return TRUE;
}

void CCamCtrl::ClickAndMove(BYTE *packet, long x, long y)
{
	long xOff,yOff;
	BYTE tmpPacket[PACKET_SIZE+4];
	int nWrite = 0;
	int pan,tilt,zoom;

	/*xOff = (IMG_W/2)-x;
	yOff = y - (IMG_H/2);*/

	pan = (int)(0x456 + xOff*m_pixelPerPan[0]);
	tilt = (int)(0x1cf + yOff*m_pixelPerTilt[0]);
	
	tmpPacket[0] = 0xA5;
	tmpPacket[1] = 0x01;
	tmpPacket[2] = 0x41;
	tmpPacket[3] = pan/256;
	tmpPacket[4] = pan%256;
	tmpPacket[5] = tilt/256;
	tmpPacket[6] = tilt%256;
	tmpPacket[7] = 0x07;
	tmpPacket[8] = 0xd0;
	tmpPacket[9] = 0x55;
	
	tmpPacket[10] = CheckSum(tmpPacket,PACKET_SIZE+3);
	nWrite = writeComm ( (BYTE *)tmpPacket, PACKET_SIZE+4);
}

int CCamCtrl::GetPosition(BYTE *packet)
{
	DWORD           dwEvent;
	OVERLAPPED      os;
	BOOL            bOk = TRUE;
	DWORD           dwRead;				// ���� ����Ʈ��.
	DWORD			count;
	
	// Event, OS ����.
	memset( &os, 0, sizeof(OVERLAPPED));
	
	//--> �̺�Ʈ ����..
	if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
	{
		bOk = FALSE;
	}
	
	//--> �̺�Ʈ ����ũ..
	if( !SetCommMask( m_hComm, EV_RXCHAR) )
	{
		bOk = FALSE;
	}
	
	//--> �̺�Ʈ��..����ũ ������ ������..
	if( !bOk )
	{
		return 0;
	}	

	dwEvent = 0;
	
    WaitCommEvent( m_hComm, &dwEvent, NULL);
	
    if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
    {
		count = 0;
		do
		{
			dwRead = readComm(packet, 1024);
			count += dwRead;
			
		}while(dwRead);
	}

	CloseHandle( os.hEvent);

	return count;
}

void CCamCtrl::MovePosition(BYTE *pan, BYTE *tilt, BYTE *zoom)
{
	BYTE tmpPacket[PACKET_SIZE+4];
	int nWrite = 0;
	
	tmpPacket[0] = 0xA5;
	tmpPacket[1] = 0x01;
	tmpPacket[2] = 0x41;
	tmpPacket[3] = pan[0];
	tmpPacket[4] = pan[1];
	tmpPacket[5] = tilt[0];
	tmpPacket[6] = tilt[1];
	tmpPacket[7] = zoom[0];
	tmpPacket[8] = zoom[1];
	tmpPacket[9] = 0x55;
	
	tmpPacket[10] = CheckSum(tmpPacket,PACKET_SIZE+3);	
	nWrite = writeComm ( (BYTE *)tmpPacket, PACKET_SIZE+4);	
}

BYTE CCamCtrl::strToHex(char * str)
{
	int upper,lower;
	
	upper = charToHex(str[0]);
	lower = charToHex(str[1]);
	
	return upper * 16 + lower;

}

int CCamCtrl::charToHex(char cHexaData)
{
	int HexValue;
	
	//--- 0 ~ 9
	if( ('0' <= cHexaData) && (cHexaData <= '9') )
	{
		HexValue = cHexaData - '0';		//'7' - '0' = 7
	}
	//--- A ~ F
	else if( ('A' <= cHexaData) && (cHexaData <= 'F') )
	{
		HexValue = cHexaData - 'A' + 10;//'C' - 'A' = 2 ==> 12
	}
	//--- a ~ f(�ҹ��� �Է�)
	else if( ('a' <= cHexaData) && (cHexaData <= 'f') )
	{
		HexValue = cHexaData - 'a' + 10;//'c' - 'a' = 2 ==> 12
	}
	//--- �� �ܴ� ����..
	else
	{
		return -1;
	}
	
	return HexValue;
}

/*
*	
while(inp > 0) {
a = inp % 16;
k[cnt++] = a;
inp = (inp - a) / 16;
}

 */