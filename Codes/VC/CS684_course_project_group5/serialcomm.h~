/**
 * @file point.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, defines the point class and the related operations.
 * @author Group5, Abhirup Ghosh, Anirban Basumallik & Gourab Roy, IIT Bombay
 * @date 07/Nov/2010
 * @version 1.0
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 * This is a property of ERTS Lab, IIT Bombay.
 *
 */

/// The serial port handler
HANDLE hPort;

/*
 * Function to open the serial port
 * @param PortSpecifier mention the com port as string. eg. COM5
 */
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

/*
 * Function to write to the serial com port
 * @param number The number to be written to the serial port
 */
bool WriteComPort(int number)
{
	DWORD byteswritten;
	char data[2] = {number,'\0'};
	bool retVal = WriteFile(hPort,data,1,&byteswritten,NULL);
	return retVal;
}

/*
 * Function that receives the array of numbers (representing the line segments 
 * to be drawn) to be sent to the serial port.
 * @param moves The array of numbers (representing the line segments 
 * to be drawn)
 * @param length The length of the 'moves' array
 */
int writeOnBot(int* moves,int length)
{
	char input[3];
	int i;

	if(!serialOpen("COM5"))
	{
		printf("Error: serial port can't be opened.");
		return -1;
	}
	for(int i = 0; i < length; i++)
	{
		int m=moves[i];
		if(i%3==0)
			m/=8;
		WriteComPort(m);
		printf("\nmove: %d",m);
		Sleep(300);
		if(i%3 == 2)
			WriteComPort(254);
	}
	WriteComPort(255);
	return 0;
}
