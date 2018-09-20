#include "SerialPort.h"

#include <windows.h>
#include <cstring>
#include <memory>
#include "../common/Common.h"


SerialPort::SerialPort(void)
{
	memset(&_ci, 0, sizeof(_ci));
	_hComm = -1;
	_ci.sockMode = SM_Null;
	_ci.connType = CT_SerialPort;
}

SerialPort::SerialPort(const SerialPort &sp)
{
	this->_hComm = sp._hComm;
	_ci = this->_ci;
}

SerialPort::~SerialPort(void)
{
}



bool SerialPort::init(int commNum)
{
	DCB dcb;
	BOOL result;
	char buffer[32];
	COMMTIMEOUTS commTimeouts;
	HANDLE hComm;
	sprintf(buffer, "\\\\.\\COM%d", commNum);
	_ci.hostAddress = commNum;
	hComm = CreateFileA(buffer, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);


	int nError = GetLastError();
	if (hComm == INVALID_HANDLE_VALUE)
	{
		printDebugInfo("the comm cann't be open!!!");
		return false;
	}


	SetupComm(hComm, 8096, 8096);
	SetCommMask(hComm, EV_RXCHAR);

	commTimeouts.ReadIntervalTimeout = MAXWORD;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.ReadTotalTimeoutConstant = 1000;
	commTimeouts.WriteTotalTimeoutMultiplier = 2 * CBR_57600 / 57600;
	commTimeouts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(hComm, &commTimeouts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState( hComm, &dcb);
	dcb.BaudRate = CBR_57600;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	dcb.fBinary = TRUE;//������ͨ��, ���ַ�ͨ��
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxCtsFlow = 0;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	//dcb.fInX = dcb.fOutX = 1;
	//dcb.XonChar = 0X11;
	//dcb.XoffChar = 0X13;
	//dcb.XonLim = 100;
	//dcb.XoffLim = 100;
	dcb.fParity = TRUE;



	//�����豸���ƿ�����ͨ���豸
	result = SetCommState(hComm, &dcb);

	if (!result) return false;

	//ˢ�建����
	PurgeComm( hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	//ָ������ִ����չ����
	EscapeCommFunction( hComm, SETDTR);



	// 	//���ô��� ���յ��ַ����뻺�������¼�
	// 	if (!SetCommMask(COMFile, EV_RXCHAR )) return ( FALSE );
	_hComm = (unsigned int)hComm;
	return true;
}

void SerialPort::close()
{
	if (_hComm == (unsigned int)INVALID_HANDLE_VALUE)
	{
		return;
	}
	printDebugInfo("CXCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
	printDebugInfo("CXCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
	CloseHandle((HANDLE)_hComm);
	_hComm = (unsigned int)INVALID_HANDLE_VALUE;
}




void SerialPort::setCommunicationInfomation(const CommunicatioInformation &ci)
{
	_ci = ci;
}

CommunicatioInformation SerialPort::getCommunicationInformation() const
{
	return _ci;
}