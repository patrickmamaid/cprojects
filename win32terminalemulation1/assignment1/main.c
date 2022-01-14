// Program WinMenu.cpp
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
void captureCommandMode(char);

#pragma warning (disable: 4096)

LPCSTR	lpszCommName = TEXT("com1");
COMMCONFIG	cc;
HANDLE hComm;
HWND hwnd;

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
	
	if (!RegisterClassEx (&Wcl))
		return 0;

	hwnd = CreateWindow (Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
   							600, 400, NULL, NULL, hInst, NULL);
	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	//The CreateFile function opens a communications port. There are two ways 
	//to call CreateFile to open the communications port: overlapped and nonoverlapped.
	if ((hComm = CreateFile (lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
   							NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
                        	== INVALID_HANDLE_VALUE)
	{
   		MessageBox (NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
		//return FALSE;
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

/*
program entry point
*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{

	int response;
	HDC hdc;
	PAINTSTRUCT paintstruct;
	static unsigned k = 0;

	
	switch (Message)
	{
		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
          		case IDM_COM1://selects com1
        			cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
        			if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
				break;
				case IDM_COM2://selects com2
					cc.dwSize = sizeof(COMMCONFIG);
					cc.wVersion = 0x100;
					GetCommConfig (hComm, &cc, &cc.dwSize);
            		if (!CommConfigDialog (lpszCommName, hwnd, &cc))
               			break;
				case IDM_9600://should set speed to 9600
					response = MessageBox(
						hwnd,
						TEXT("9600 selected"),
						TEXT("Settings Saved"),
						MB_OK
						);
					break;
				case IDM_32000://should set speed to 32000
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
   		//case WM_RBUTTONDOWN:		// Process right button
      	//	response = MessageBox (hwnd, TEXT("Press One:"), TEXT("Right Button"),
        //							  MB_ABORTRETRYIGNORE);
		//	switch (response)
		//	{
        // 		case IDABORT:
        //    		MessageBox (hwnd, TEXT(""), TEXT("Abort"), MB_OK);
		//		break;
		//		case IDRETRY:
        //    		MessageBox (hwnd, TEXT(""), TEXT("Retry"), MB_OK);
		//		break;
		//		case IDIGNORE:
		//			MessageBox (hwnd, TEXT(""), TEXT("Ignore"), MB_OK);
		//		break;
		//	}
		//break;

		//case WM_LBUTTONDOWN:		// Process left button
      //		response = MessageBox (hwnd, TEXT("Conitnue?"), TEXT("Left Button"),
        // 							  MB_ICONHAND | MB_YESNO);
		//	switch (response)
		//	{
         //		case IDYES:
           // 		MessageBox (hwnd, TEXT("Press Button"), TEXT("Yes"), MB_OK);
			//	break;
			//	case IDNO:
            //		MessageBox (hwnd, TEXT("Press Button"), TEXT("No"), MB_OK);
			//	break;
			//}
		//break;
		
		case WM_CHAR:	// Process keystroke
			
			

			hdc = GetDC (hwnd);			 // get device context
			sprintf (str, "%c", (char) wParam); // Convert char to string
			TextOut (hdc, 8*k, 0, str, strlen (str)); // output character	
			k++; // increment the screen x-coordinate
			captureCommandMode((char)wParam);
			//captureConnectMode((char)wParam);
			ReleaseDC (hwnd, hdc); // Release device context
		break;
		
		case WM_PAINT:		// Process a repaint message
			hdc = BeginPaint (hwnd, &paintstruct); // Acquire DC
			TextOut (hdc, 0, 0, str, strlen (str)); // output character
			EndPaint (hwnd, &paintstruct); // Release DC
		break;

		case WM_DESTROY:	// Terminate program
      		PostQuitMessage (0);
		break;
		default:
			
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;
}


void captureCommandMode(char a){


	static int i=0;
	char strCommandMode[100] = "command mode";
	

	if(i >= 11){
		MessageBox(hwnd, TEXT("command mode entered"), TEXT("asdfas"), MB_OK);
	}
	if(a != strCommandMode[i])
	{
		i = 0;
	}
	else{
		i++;
	}


}

/*
void captureConnectMode(char a){


	static int i=0;
	char strCommandMode[100] = "connect mode";
	

	if(i >= 11){
		MessageBox(hwnd, TEXT("connect mode entered"), TEXT("asdfas"), MB_OK);
	}
	if(a != strCommandMode[i])
	{
		i = 0;
	}
	else{
		i++;
	}


}

*/
//////////////////////////////////      SESSION LAYER      ////////////////////////////////