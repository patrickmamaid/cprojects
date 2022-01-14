#include "session.h"


//*************************************************************
// Function: BOOL EnterCommandMode()
//
// Description: Disconnects the connection between the ports
//
// Returns: TRUE is successful
//*************************************************************
BOOL EnterCommandMode() {
	if (connectMode == 0)
		return FALSE;		
	StopComm();
	connectMode = 0;
	return TRUE;
}
//***********************************************************
// Function: BOOL StopComm()
//
// Description: Stops reading from ports and kills threads and
//              communication handles
//
// Returns: TRUE is successful
//***********************************************************
BOOL StopComm() {
	bReading = FALSE;
	CloseHandle (hThread);
	CloseHandle (hComm);
	return TRUE;
}

//*************************************************************
// Function: DWORD ReadThread (LPDWORD lpdwParam1)
//
// Description: Thread dedicated to reading characters.
//
// Returns: TRUE is successful
//*************************************************************
DWORD ReadThread (LPDWORD lpdwParam1)
{	
	BYTE inbuff[100];
	DWORD nBytesRead;
	
	memset (&to, 0, sizeof(to));
	to.ReadIntervalTimeout = MAXDWORD;
	SetCommTimeouts (hComm, &to);	

	while (bReading) {
		if (!ReadFile(hComm, inbuff, 100, &nBytesRead, NULL)) {
			return FALSE;
		} else {
			if (nBytesRead)
				WriteToBlock(programWnd, (LPBYTE)inbuff, nBytesRead);		
		}		
	}	
	return TRUE;
}
//*************************************************************
// Function: BOOL EnterConnectMode()
//
// Description: Creates the connection between the ports
//
// Returns: TRUE is successful
//*************************************************************
BOOL EnterConnectMode() {
	if (connectMode == 1) 
		return FALSE;
	if (InitializeComm() == FALSE)
		return FALSE;
	StartComm();
	connectMode = 1;
	return TRUE;
}
//***********************************************************
// Function: BOOL InitializeComm()
//
// Description: Initializes a handle for reading and writing between
//              ports
//
// Returns: TRUE is successful
//***********************************************************
BOOL InitializeComm() {
	if (portSet == 0) {
		MessageBox (NULL, TEXT("Port has not been set, check 'Settings'"), TEXT("Error"), MB_OK);
		return FALSE;		
	}
	switch (commNo) {
		case 1: 
			if ((hComm = CreateFile (TEXT("COM1"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 2: 
			if ((hComm = CreateFile (TEXT("COM2"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 3: 
			if ((hComm = CreateFile (TEXT("COM3"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 4: 
			if ((hComm = CreateFile (TEXT("COM4"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 5: 
			if ((hComm = CreateFile (TEXT("COM5"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 6: 
			if ((hComm = CreateFile (TEXT("COM6"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 7: 
			if ((hComm = CreateFile (TEXT("COM7"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 8: 
			if ((hComm = CreateFile (TEXT("COM8"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 9: 
			if ((hComm = CreateFile (TEXT("COM9"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, 0, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
	}
	BuildCommDCB (TEXT("96,N,8,1"), &dcb); //Set it itself because SetPort function
	SetCommState (hComm, &dcb);            //does not work
	return TRUE;
}

//***********************************************************
// Function: BOOL StartComm()
//
// Description: Starts a thread to handle reading from ports
//
// Returns: TRUE is successful
//***********************************************************
BOOL StartComm() {
	bReading = TRUE;
	if ((hThread=CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)ReadThread, NULL, 
		0, &id)) == INVALID_HANDLE_VALUE) {			
			MessageBox (NULL, TEXT("Error creating READ thread"),TEXT(""),MB_OK);
			CloseHandle (hComm);
			return FALSE;
	}
	return TRUE;
}


//***********************************************************
// Function: void GetLargestDisplayMode (int *pcxBitmap, int *pcyBitmap)
//
// Description: --
//***********************************************************
void GetLargestDisplayMode (int *pcxBitmap, int *pcyBitmap){
     DEVMODE devmode;
     int     iModeNum = 0;
     *pcxBitmap = *pcyBitmap = 0;
     ZeroMemory (&devmode, sizeof(DEVMODE));
     devmode.dmSize = sizeof(DEVMODE);     
     while (EnumDisplaySettings (NULL, iModeNum++, &devmode)){
          *pcxBitmap = max(*pcxBitmap, (int)devmode.dmPelsWidth);
          *pcyBitmap = max(*pcyBitmap, (int)devmode.dmPelsHeight);
     }
}
//*************************************************************
// Function: BOOL SetPort(HWND hWnd, int portNo)
//
// Description: Gets the port properties a user wants and sets it
//              for the port
//
// Returns: TRUE is successful
//
// Note: It doesn't work.
//*************************************************************
BOOL SetPort(HWND hWnd, int portNo){ 
	if (connectMode == 1) {
		MessageBox (NULL, TEXT("Must be in command mode to set ports"), TEXT("Error"), MB_OK);
		return FALSE;
	}	
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;
	GetCommConfig (hComm, &cc, &cc.dwSize);
	switch (portNo) {
		case 1:
			if (!CommConfigDialog (TEXT("COM1"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}			
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 1;
			MessageBox (NULL, TEXT("Port set to COM1"), TEXT("COM1"), MB_OK);
			break;
		case 2:
			if (!CommConfigDialog (TEXT("COM2"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 2;
			MessageBox (NULL, TEXT("Port set to COM2"), TEXT("COM2"), MB_OK);
			break;
		case 3:
			if (!CommConfigDialog (TEXT("COM3"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 3;
			MessageBox (NULL, TEXT("Port set to COM3"), TEXT("COM3"), MB_OK);
			break;
		case 4:
			if (!CommConfigDialog (TEXT("COM4"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 4;
			MessageBox (NULL, TEXT("Port set to COM4"), TEXT("COM4"), MB_OK);
			break;
		case 5:
			if (!CommConfigDialog (TEXT("COM5"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 5;
			MessageBox (NULL, TEXT("Port set to COM5"), TEXT("COM5"), MB_OK);
			break;
		case 6:
			if (!CommConfigDialog (TEXT("COM6"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 6;
			MessageBox (NULL, TEXT("Port set to COM6"), TEXT("COM6"), MB_OK);
			break;
		case 7:
			if (!CommConfigDialog (TEXT("COM7"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 7;
			MessageBox (NULL, TEXT("Port set to COM7"), TEXT("COM7"), MB_OK);
			break;
		case 8:
			if (!CommConfigDialog (TEXT("COM8"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 8;
			MessageBox (NULL, TEXT("Port set to COM8"), TEXT("COM8"), MB_OK);
			break;
		case 9:
			if (!CommConfigDialog (TEXT("COM9"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 9;
			MessageBox (NULL, TEXT("Port set to COM9"), TEXT("COM9"), MB_OK);
			break;

	}
	portSet = 1;
	return TRUE;
}

//*************************************************************
// Function: void Purge()
//
// Description: Clears main comm port of buffer streams
//*************************************************************
void Purge(){
	PurgeComm(hComm, PURGE_RXCLEAR);
}

//*************************************************************
// Function: void SendByte(LPCVOID lpbuffer, int bufLength)
//
// Description: Sends buffer over comm port
//*************************************************************
void SendByte(LPCVOID lpbuffer, int bufLength){
	WriteFile (hComm, lpbuffer, bufLength, &nBytesRead, NULL);	
}

//*************************************************************
// Function: void SetProgramWindow(HWND hwnd)
//
// Description: Sets the program window
//*************************************************************
void SetProgramWindow(HWND hwnd) {
	programWnd = hwnd;
}