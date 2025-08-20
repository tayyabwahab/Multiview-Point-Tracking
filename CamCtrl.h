// CamCtrl.h: interface for the CCamCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMCTRL_H__47945643_6B4F_43A0_9B7A_9E0974324ED4__INCLUDED_)
#define AFX_CAMCTRL_H__47945643_6B4F_43A0_9B7A_9E0974324ED4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFF_SIZE 4192
#define PACKET_SIZE 7
#define MAX_PACKET 15

#define MV_NUMBER		20

#define MV_RIGHT		0
#define MV_RIGHT_UP		1
#define MV_UP			2
#define MV_LEFT_UP		3
#define MV_LEFT			4
#define MV_LEFT_DOWN	5
#define MV_DOWN			6
#define MV_RIGHT_DOWN	7
#define MV_STOP			8
#define MV_ZOOM_TELE	9
#define MV_ZOOM_WIDE	10
#define MV_FOCUS_NEAR	11
#define MV_FOCUS_FAR	12
#define MV_IRIS_OPEN	13
#define MV_IRIS_CLOSE	14
#define MV_SET_PRESET1	15
#define MV_GO_PRESET1	16
#define GET_CUR_POS		17
#define SET_CUR_POS		18

#define WM_COMM_READ (WM_USER + 1)

#define PAN_MIN			0
#define PAN_MAX			9600
#define TILT_MIN		0
#define TILT_MAX		2400
#define ZOOM_MIN		64
#define ZOOM_MAX		3152

#define PRESET_NUM		1

class CQueue
{
public:
	BYTE buff[BUFF_SIZE];
	int m_iHead, m_iTail;
	CQueue();
	void Clear();
	int GetSize();
	BOOL PutByte(BYTE b);
	BOOL GetByte(BYTE *pb);
};


class CCamCtrl  
{
public:
	CCamCtrl();
	virtual ~CCamCtrl();

	BOOL	OpenPort(char *strPortName,DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity);
	void	ClosePort();
	BOOL	Initialize();
	DWORD	writeComm(BYTE *pBuff, DWORD nToWrite);
	DWORD	readComm(BYTE *pBuff, DWORD nToRead);
	BOOL	sendPacket(int index);
	BYTE	CheckSum(BYTE* packet,int MaxSize);
	void	SetPreset(int presetNum);
	void	GoPreset(int presetNum);
	int		GetPosition(BYTE *packet);
	void	MovePosition(BYTE *pan, BYTE *tilt, BYTE *zoom);
	BYTE	strToHex(char *str);
	int		charToHex(char cHexData);
	void	ClickAndMove(BYTE *packet,long x, long y);

	static	UINT ThreadWatchComm(LPVOID pParam);

	BOOL        check;
	HANDLE		m_hComm;
	char *		m_sPortName;
	BOOL		m_bConnected;
	OVERLAPPED	m_osRead, m_osWrite;
	HANDLE		m_hThreadWatchComm;
	WORD		m_wPortID;
	char		m_strPacket[MAX_PACKET*2];
	
	CQueue      m_QueueRead;

private:
	int			m_panPos[PRESET_NUM];
	int			m_tiltPos[PRESET_NUM];
	int			m_zoomPos[PRESET_NUM];

	double		m_pixelPerPan[ZOOM_MAX - ZOOM_MIN];
	double		m_pixelPerTilt[ZOOM_MAX - ZOOM_MIN];
	double		m_zoomLevel[320];
};

#endif // !defined(AFX_CAMCTRL_H__47945643_6B4F_43A0_9B7A_9E0974324ED4__INCLUDED_)
