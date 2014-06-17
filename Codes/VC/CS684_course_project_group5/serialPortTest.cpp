// serialPortTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <atlstr.h>

HANDLE hPort;
DWORD byteswritten;

bool serialOpen(CString PortSpecifier)
{
	DCB dcb;
	hPort = CreateFile(
		PortSpecifier,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	if (!GetCommState(hPort,&dcb))
		return false;
	dcb.BaudRate = CBR_9600; //9600 Baud
	dcb.ByteSize = 8; //8 data bits
	dcb.Parity = NOPARITY; //no parity
	dcb.StopBits = ONESTOPBIT; //1 stop
	if (!SetCommState(hPort,&dcb))
		return false;
	return true;
}

bool WriteComPort(int number)//CString data
{
	char data[2] = {number,'\0'};
	bool retVal = WriteFile(hPort,data,1,&byteswritten,NULL);
	return retVal;
}

int writeOnBot(int* moves,int length)//int argc, _TCHAR* argv[])
{
	char input[3];
	if(!serialOpen("COM7"))
		return -1;
//	int array[10] = {10,30,10,-45,15,255};
//	list<double>::iterator it;
//	it=moves.begin();

	int i;
//	for(;it!=moves.end();it++)
	for(i=0;i<length;i++)
	{
		int m=moves[i];//(*it);
//		WriteComPort(m);
//		printf("\nmove: %d",m);
		Sleep(500);
	}
	return 0;
}