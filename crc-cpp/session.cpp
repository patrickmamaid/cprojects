#include "session.h"

//**********************************************************
// Function: SetState
//
// Description: Sets the current state.
//
// Returns: void
//**********************************************************
void SetState(int newState) {
	InvalidateRect(programWnd, NULL, TRUE);
	state = newState;
	//if (state == STATE_WAIT_FOR_ENQ && HasFramesToSend()) { // idle
	//	SendingData(STATE_SEND_ENQ);
	//}
}

//**********************************************************
// Function: SplitFileToFrames
//
// Description: Splits a file into proper 2kB protocol frames
//				to be sent over the communication port.
//
// Returns: a vector of the split frames of the file,
//			in bytes
//**********************************************************
vector<string> SplitFileToFrames(TCHAR* fileName) {
	FILE *input;
	
	vector<string> vec;
	int numberOfFrames = 0, pos = 0, ch, count = 0;
	char buffer[NUMBER_OF_BYTES_IN_FRAME];
	vector<string>::iterator it;

	buffer[pos++] = ASCII_STX;		//start of text	
	buffer[pos++] = ASCII_DC2;		//begin filename with dc2 char	
	for (int i = 0; fileName[i] != '\0'; i++) {
		buffer[pos++] = fileName[i];
	}
	buffer[pos++] = ASCII_DC2;		//end filename with dc2 char

	fopen_s(&input, fileName, "r" );

	if (input == NULL)
		return vec;

	ch = getc( input );
	while( ch != EOF ) {
		buffer[pos++] = ch;		
		ch = getc( input );
		if (pos == BYTE_POS_ETX) {			
			buffer[BYTE_POS_ETX]		= ASCII_ETX; // end of text
			buffer[BYTE_POS_CHECKSUM]	= '\x35';	// checksum
			buffer[BYTE_POS_CHECKSUM+1] = '\x35';	// checksum
			string s(buffer, 1004);
			vec.push_back(s);
			numberOfFrames++;
			pos = 0;
			buffer[pos++] = ASCII_STX;	// start of text			
		}
	}

	//eof reached
	while (pos != BYTE_POS_ETX) {
		buffer[pos++] = ASCII_NULL;		// pad frame with null bytes		
		if (pos == BYTE_POS_ETX) {			// frame fully padded			
			buffer[BYTE_POS_ETX]		= ASCII_ETX; // end of text
			buffer[BYTE_POS_CHECKSUM]	= '\x35';	// checksum
			buffer[BYTE_POS_CHECKSUM+1] = '\x35';	// checksum
			string s(buffer, 1004);
			vec.push_back(s);
			numberOfFrames++;
		}
	}
	fclose(input);

	return vec;
}

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
	BYTE inbuff[1004];
	DWORD nBytesRead, dwEvent, dwError;
	HANDLE hEvent = CreateEvent(0, 0, 0, "read");
	COMSTAT cs;
	OVERLAPPED ostruct;
	DWORD timeout;

	ostruct.Offset = 0;
	ostruct.OffsetHigh = 0;
	ostruct.hEvent = hEvent;

	// generate event whenever a byte arives 
	SetCommMask (hComm, EV_RXCHAR);
	
	// read a byte whenever one is received
	while (bReading) {
		WaitCommEvent (hComm, &dwEvent, &ostruct);
		// wait for event 
		WaitForSingleObject(hEvent, 2500);
		timeout = GetOverlappedResult(hComm, &ostruct, &nBytesRead, TRUE);
		if (timeout != WAIT_TIMEOUT){
			// read all available bytes 			
			ClearCommError (hComm, &dwError, &cs);
			if ((dwEvent & EV_RXCHAR) && cs.cbInQue) {
				if (!ReadFile(hComm, inbuff, cs.cbInQue, 
							  &nBytesRead, &ostruct)){
					// handle error 
				    ResetEvent(hEvent);
					return 1L;
				} // end if (error reading bytes)
				else {
					if (nBytesRead)
						IdleReceiving(state, (LPSTR)inbuff, nBytesRead);
				}
			}
		} else { // timeout
			int x = 0;
			x += 10;
			//return 2L;
		}
		ResetEvent(hEvent);
	} // end while (reading bytes in loop)
	// clean out any pending bytes in the receive buffer 
	PurgeComm(hComm, PURGE_RXCLEAR);
	return 0L;
} //end function (ReadThread)

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
	if (HasFramesToSend()) {
		SetRetriesLeft(4);
		SetState(STATE_SEND_ENQ);
		SendingData(STATE_SEND_ENQ);
	} else {
		SetState(STATE_WAIT_FOR_ENQ);
	}
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
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}			
		break;
		case 2: 
			if ((hComm = CreateFile (TEXT("COM2"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 3: 
			if ((hComm = CreateFile (TEXT("COM3"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 4: 
			if ((hComm = CreateFile (TEXT("COM4"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}
		break;
		case 5: 
			if ((hComm = CreateFile (TEXT("COM5"), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
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
	//SetCommState (hComm, &dcb);            //does not work
	SetCommState (hComm, &cc.dcb);
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
			commNo = 5;
			MessageBox (NULL, TEXT("Port set to COM7"), TEXT("COM7"), MB_OK);
			break;
		case 8:
			if (!CommConfigDialog (TEXT("COM8"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 4;
			MessageBox (NULL, TEXT("Port set to COM8"), TEXT("COM8"), MB_OK);
			break;
		case 9:
			if (!CommConfigDialog (TEXT("COM9"), hWnd, &cc)) {
				MessageBox (NULL, TEXT("Cannot set port"), TEXT("Error"), MB_OK);
				return FALSE;
			}
			//if (!SetCommState (hComm, &cc.dcb))
			//	break;
			commNo = 5;
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
	OVERLAPPED ostruct;
	ostruct.Offset = 0;
	ostruct.OffsetHigh = 0;
	ostruct.hEvent = CreateEvent(0, 0, 0, "read");
	WriteFile (hComm, lpbuffer, bufLength, &nBytesRead, &ostruct);
}

//*************************************************************
// Function: void SetProgramWindow(HWND hwnd)
//
// Description: Sets the program window.
//
// Returns: void
//*************************************************************
void SetProgramWindow(HWND hwnd) {
	programWnd = hwnd;
}

//***********************************************************
// Function: int GetState()
//
// Description: Gets the state of program in the protocol.
//
// Returns: the state value
//***********************************************************
int GetState() {
	return state;
}

//***********************************************************
// Function: bool IsConnected()
//
// Description: Checks to see if the ports are connected.
//
// Returns: true if connected
//***********************************************************
bool IsConnected() {
	if (connectMode == 1) {
		return true;
	}
	return false;
}

//***********************************************************
// Function: void SetSendMoreDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void SetSendMoreDataTimer() {
	SetTimer(programWnd, TIMER_TO_SEND_MORE_DATA, 100, NULL);
}

//***********************************************************
// Function: void SetWaitForAckDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void SetWaitForAckDataTimer() {
	SetTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_DATA, 250, NULL);
}

//***********************************************************
// Function: void SetWaitForDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void SetWaitForDataTimer() {
	SetTimer(programWnd, TIMER_TO_WAIT_FOR_DATA, 250, NULL);
}

//***********************************************************
// Function: void SetWaitForAckRviTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void SetWaitForAckRviTimer() {
	SetTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_RVI, 250, NULL);
}

//***********************************************************
// Function: void SetWaitForAckEnqTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void SetWaitForAckEnqTimer() {
	SetTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_ENQ, 250, NULL);
}

//***********************************************************
// Function: void KillSendMoreDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void KillSendMoreDataTimer() {
	KillTimer(programWnd, TIMER_TO_SEND_MORE_DATA);
}

//***********************************************************
// Function: void KillWaitForAckDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void KillWaitForAckDataTimer() {
	KillTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_DATA);
}

//***********************************************************
// Function: void KillWaitForDataTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void KillWaitForDataTimer() {
	KillTimer(programWnd, TIMER_TO_WAIT_FOR_DATA);
}

//***********************************************************
// Function: void KillWaitForAckRviTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void KillWaitForAckRviTimer() {
	KillTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_RVI);
}

//***********************************************************
// Function: void KillWaitForAckEnqTimer()
//
// Description: Creates a timer to create a timeout
//***********************************************************
void KillWaitForAckEnqTimer() {
	KillTimer(programWnd, TIMER_TO_WAIT_FOR_ACK_ENQ);
}