/********************************
Patrick Mamaid
A00609663-3F

Assignment 1 "Dumb Terminal"

********************************/




// Program main.c
///////////////////////////////////////////////////APPLICATION LAYER////////////////////////////////////////////////////
#define STRICT

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "winmenu2.h"

TCHAR Name[] = TEXT("Comm Shell");
TCHAR str[80] = TEXT("");
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
VOID RunCmd(PVOID);
BOOL WriteChar();
VOID ReadSerialInput(PVOID);
HDC hdc;
#pragma warning (disable: 4096)

COMMPROP cProp;
LPCSTR	lpszCommName = TEXT("COM5");
COMMCONFIG	cc;
HANDLE hComm;
HWND hwnd;
DCB dcb;

//forward declaration for reading:
DWORD dwRead;
BOOL fWaitingOnRead = FALSE;
//OVERLAPPED osReader = {0};

//forward declarations for writing:
OVERLAPPED osWrite = {0};


//timeout structures
COMMTIMEOUTS timeouts;


unsigned k = 0;
unsigned h = 0;




int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 						  LPSTR lspszCmdParam, int nCmdShow)
{
	//HWND hwnd;
	MSG Msg;
	WNDCLASSEX Wcl;
	

	Wcl.cbSize = sizeof (WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style
	
	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name;
	
	Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0; 
	


	timeouts.ReadIntervalTimeout = 1000;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 1000;
	timeouts.WriteTotalTimeoutConstant = 1000;
	timeouts.WriteTotalTimeoutMultiplier= 1000;

	
	

	if (!RegisterClassEx (&Wcl))
		return 0;

	hwnd = CreateWindow (Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
   							600, 400, NULL, NULL, hInst, NULL);

	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);


	//attempt to default config & initialize a serial port:
	hComm = CreateFile (lpszCommName, 
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							0,
							NULL);


	if(!SetupComm(hComm, 500, 500)){
		MessageBox(hwnd, TEXT("SETUPCOMM ERROR"), TEXT("ERROR"), MB_OK);
		return FALSE;
	}

	if(!GetCommProperties(hComm, &cProp)){
		MessageBox(hwnd, TEXT("COMMPROP ERROR"), TEXT("ERROR"), MB_OK);
	}

	BuildCommDCB(TEXT("96,N,8,1"), &dcb);
	
	
	if(!SetCommTimeouts(hComm, &timeouts)){
		MessageBox(hwnd, TEXT("COMM TIMEOUTS"), TEXT("ERROR"), MB_OK);
	}


	while (GetMessage (&Msg, NULL, 0, 0))
	{
   		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	return Msg.wParam;
}


/*
this thread is spawned and it runs cmd.exe
*/
VOID RunCmd(PVOID pvoid){
	system("cmd.exe");
	_endthread();

}


LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{

	int response;
	int commPortSelected = 1;

	
	PAINTSTRUCT paintstruct;
	

	
	switch (Message)
	{
		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case IDM_CONNECT:


					if(!GetCommState(hComm, &dcb)){
						MessageBox(hwnd, TEXT("GET CONFIGURATION PORT HAS A PROBLEM"), TEXT("ERROR"), MB_OK);
						return FALSE;
					}
					_beginthread(ReadSerialInput , 0, NULL) ;
				
					break;

				
          		case IDM_COM1://selects com1
					lpszCommName = TEXT("COM1");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM2://selects com2
					lpszCommName = TEXT("COM2");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM3:
					lpszCommName = TEXT("COM3");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM4:
					lpszCommName = TEXT("COM4");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
					
				case IDM_COM5:
					lpszCommName = TEXT("COM5");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM6:
					lpszCommName = TEXT("COM6");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM7:
					lpszCommName = TEXT("COM7");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM8:
					lpszCommName = TEXT("COM8");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM9:
					lpszCommName = TEXT("COM9");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;
				case IDM_COM10:
					lpszCommName = TEXT("COM10");
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
					break;

				case IDM_9600://should set speed to 9600
					cProp.dwMaxBaud = 9600;
					response = MessageBox(
						hwnd,
						TEXT("9600 selected"),
						TEXT("Settings Saved"),
						MB_OK
						);
					break;
				case IDM_32000://should set speed to 32000
					cProp.dwMaxBaud = 32000;
					response = MessageBox(
						hwnd,
						TEXT("32000 selected"),
						TEXT("Settings Saved"),
						MB_OK
						);
					break;
				case IDM_110000://should set speed to 110000
					response = MessageBox(
						hwnd,
						TEXT("110000 selected"),
						TEXT("Settings Saved"),
						MB_OK
						);
					break;
				case IDM_EXIT:
						exit(IDM_EXIT);
					break;
			
				case IDM_EXIT_TO_COMMAND_PROMPT:
						
					_beginthread(RunCmd , 0, NULL) ;
					Sleep(1000);
					exit(IDM_EXIT_TO_COMMAND_PROMPT);
					break;

				case IDM_ABOUT:
					response = MessageBox(
						hwnd,
						TEXT("PATRICK MAMAID\nA00609663"),
						TEXT("About Box"),
						MB_OK
						);
					break;
				break;
			}
		break;
		
		case WM_CHAR:	// Process keystroke
			
			
			hdc = GetDC (hwnd);			 // get device context
			sprintf (str, "%c", (char) wParam); // Convert char to string
			
			
			

			TextOut (hdc, 8*k, 6*h, str, strlen (str)); // output character	
			WriteChar();
			//h++;
			
			k++; // increment the screen x-coordinate
			


			ReleaseDC (hwnd, hdc); // Release device context
		break;
		
		case WM_PAINT:		// Process a repaint message
			hdc = BeginPaint (hwnd, &paintstruct); // Acquire DC
			TextOut (hdc, 0, 0, str, strlen (str)); // output character
			
			EndPaint (hwnd, &paintstruct); // Release DC
		break;

		case WM_DESTROY:	// Terminate program
			CloseHandle(hComm);
      		PostQuitMessage (0);
		break;
		default:
			
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;
}




//////////////////////////////////      SESSION LAYER      ////////////////////////////////

BOOL WriteChar()

{
	int numBytesWritten = 0;
	WriteFile(hComm,str,2,&numBytesWritten, NULL);
}




VOID ReadSerialInput(PVOID pvoid){
	int read;
	while(1){
		Sleep(10);
		if(ReadFile(hComm, str, 2, &read, NULL) != 0){
			hdc = GetDC (hwnd);		
			TextOut (hdc, 8*k, 6*h, str, strlen (str));
			k++;
			ReleaseDC(hwnd, hdc);
		}
		else{
			PurgeComm(hComm, PURGE_RXABORT);
		}

	}
	PurgeComm(hComm, PURGE_RXABORT);
}






