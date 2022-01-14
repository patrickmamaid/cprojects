#include "application.h"

//***********************************************************
// Function: int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
//						  LPSTR lspszCmdParam, int nCmdShow)
//
// Description: Main-entry point
//***********************************************************
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
						  LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;
	HWND hwnd;

	Wcl.cbSize = sizeof (WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wcl.hIconSm = NULL;
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = TEXT("TERMINALMENU");
	Wcl.cbClsExtra = 0;    
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx (&Wcl))
		return 0;

	hwnd = CreateWindow (Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
   							670, 500, NULL, NULL, hInst, NULL);
	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);	

	SetProgramWindow(hwnd);

	while (GetMessage (&Msg, NULL, 0, 0))
	{
   		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}
	return (int)Msg.wParam;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (Message)
	{
		case WM_CREATE:
			hdc = GetDC(hwnd);
			SaveMemDC(hdc, hwnd);
			SetTimer(hwnd, 1, 750, NULL);
			return 0;

		case WM_SIZE:
			StoreXYClient(LOWORD(lParam), HIWORD (lParam));			
			return 0;

		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case IDM_CONN:												
					EnterConnectMode();					
				break;
				case IDM_CMD:
					EnterCommandMode();
				break;					
				case IDM_COM1:
					SetPort(hwnd, 1);
				break;
				case IDM_COM2:
					SetPort(hwnd, 2);
				break;
				case IDM_COM3:
					SetPort(hwnd, 3);
				break;
				case IDM_COM4:
					SetPort(hwnd, 4);
				break;
				case IDM_COM5:
					SetPort(hwnd, 5);
				break;
				case IDM_COM6:
					SetPort(hwnd, 6);
				break;
				case IDM_COM7:
					SetPort(hwnd, 7);
				break;
				case IDM_COM8:
					SetPort(hwnd, 8);
				break;
				case IDM_COM9:
					SetPort(hwnd, 9);
				break;
				case IDM_HELP:
					DisplayHelpBox();
				break;
				case IDM_EXIT:
					EnterCommandMode();
				    PostQuitMessage (0);
				break;
			}
		break;

		case WM_TIMER:			
			SendByte((LPBYTE)"\x01", 1);
			SendByte((LPBYTE)"\x09", 1);
			SendByte((LPBYTE)"\x00", 1);
			SendByte((LPBYTE)"\x03", 1);
			SendByte((LPBYTE)"\x01", 1);
			SendByte((LPBYTE)"\x41", 1);
			SendByte((LPBYTE)"\x0A", 1);
			SendByte((LPBYTE)"\x41", 1);
			SendByte((LPBYTE)"\xBE", 1);
		break;
		
		case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps);
			DisplayDC(hdc);			
			EndPaint (hwnd, &ps);
		break;

		case WM_DESTROY:
			DestroyDC();			
			PostQuitMessage (0);
		break;

		case WM_KEYDOWN:
			switch (wParam){
				case VK_UP:
					SendByte("\033[A", 3);					
					break;
				case VK_DOWN:
					SendByte("\033[B", 3);					
					break;
			}
		break;
		default:
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;
}
