#include "presentation.h"

using namespace std;
//***********************************************************
// Function: void DisplayHelpBox()
//
// Description: Displays a help box for the user
//***********************************************************
void DisplayHelpBox() {
	MessageBox (NULL, TEXT("1) Select a port number from 'Settings' -> 'Set Port'\n2) Select the desired port settings and push OK\n3) Select 'Connect Mode' in 'Change Mode'\n4) Select 'Command Mode' or press ESC to stop connecting"), TEXT("How To Use"), MB_OK);
}

//***********************************************************
// Function: void DisplayHelpBox()
//
// Description: Displays a help box for the user
//***********************************************************
void SaveMemDC(HDC hdc, HWND hwnd) {
	GetLargestDisplayMode (&cxBitmap, &cyBitmap);	
	hBitmap = CreateCompatibleBitmap (hdc, cxBitmap, cyBitmap);
	hdcMem  = CreateCompatibleDC (hdc);
	ReleaseDC (hwnd, hdc);
	if (!hBitmap){    // no memory for bitmap			
		DeleteDC (hdcMem);
		return;
	}
	SelectObject (hdcMem, hBitmap);
	PatBlt (hdcMem, 0, 0, cxBitmap, cyBitmap, WHITENESS);
}

//***********************************************************
// Function: void DisplayDC(HDC hdc)
//
// Description: Paints the memory DC to the regular DC
//***********************************************************
void DisplayDC(HDC hdc) {
	BitBlt (hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
}

//***********************************************************
// Function: void DestroyDC()
//
// Description: Destroys the memory DC and bitmap
//***********************************************************
void DestroyDC() {
	DeleteDC (hdcMem);
	DeleteObject (hBitmap);
}

//***********************************************************
// Function: void StoreXYClient(int xx, int yy)
//
// Description: Saves the size of the client window
//***********************************************************
void StoreXYClient(int xx, int yy) {
	cxClient = xx;
	cyClient = yy;
}

//***********************************************************
// Function: void ResetBytesReceived
//
// Description: Resets the number of bytes received.
//***********************************************************
void ResetBytesReceived() {
	bytesReceived = 0;
}

//***********************************************************
// Function: void ResetBytesReceived
//
// Description: Resets the number of bytes received.
//***********************************************************
void UpdateStatistics(HWND hwnd) {
	RECT rect;
	HDC hdc;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	GetTextMetrics(hdc, &tm);
	x = 10;
	y = 10;

	sprintf_s(str, "[ Statistics ]");
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "Number of ACKs received: %d", GetAcks());
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "Number of NAKs received: %d", GetNaks());
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "Number of frames received: %d", GetFramesReceived());
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "-----");
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "Number of frames sent: %d", GetFramesSent());
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	sprintf_s(str, "Bit Error Rate %%: %d", GetBitErrorRate());
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	y += tm.tmHeight + tm.tmExternalLeading;

	ReleaseDC(hwnd, hdc);
	y = 0;
}
