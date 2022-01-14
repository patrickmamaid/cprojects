#include "presentation.h"
#include <stack>

#define ENTITY_ID_POS 8
using namespace std;

//***********************************************************
// Function: BOOL WriteToBlock(HWND hWnd, LPSTR lpBlock)
//
// Description: Writes a character straight to screen
//
// Returns: TRUE is successful
//***********************************************************
void  WriteToBlock(HWND hWnd, LPBYTE lpBlock, int bytesRead) {
	HDC hdc;
	RECT rect;
	
	static int soh = 0;
	static int length = 0;
	static int pos = 0;
	static int fei = 0;
	
	hdc = GetDC (hWnd);
	SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	SelectObject(hdcMem, GetStockObject(SYSTEM_FIXED_FONT));
	GetClientRect(hWnd, &rect);
	GetTextMetrics(hdc, &tm);	

	for (int i = 0; i < bytesRead; i++) {
		if (soh == 1) { //start of header flag
			length = (int)lpBlock[i] - 3;
			soh = 0;			
			switch (length) {
				case 17:
					pos = length;
					break;
				case 16:
					pos = length;
					break;
				case 11:
					pos = length;
					break;
				default:
					pos = -1;
					break;
			}
			/*if (length != 17 && length != 16 && length != 11) {
				lengthflag = 1;
				sprintf(str, "%d", length);
				GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
				TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
				TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
				x += size.cx;	
			}*/
			if (length > 17) {
				soh = 1;
			}
			continue;
		}
		switch((int)lpBlock[i]) {
			default: //char				
				if (lpBlock[i] == 1 && fei != 1) {					
					soh = 1;
					fei = 1;
					break;
				}
				/*if (lengthflag == 1 && length != 0) {
					length--;
					break;
				}*/	
				if (length != 0) {
					if (pos == -1 || length > pos - 5) {
						length--;
						break;
					}
					if (length == pos - 5) {
						PrintTokenId(hWnd, rect, (int)lpBlock[i]);
						length--;
						break;
					}					
					/*sprintf(str, ">%d<", length);
					GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
					TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
					TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
					x += size.cx;*/
					ProcessTagId(hWnd, (int)lpBlock[i], length, pos);
					/*sprintf(str, "%02X", lpBlock[i]);
					GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
					TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
					TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
					x += size.cx;*/
				} else {
					if (length == 0 && pos != -1) {
						ProcessTagId(hWnd, (int)lpBlock[i], length, pos);
						/*sprintf(str, "%02X", lpBlock[i]);
						GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
						TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
						TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
						x += size.cx;*/
						y += 16;
						x = 0;
					}					
					fei = 0;
				}				
				length--;
				break;
		}
		if (x >= rect.right) {
			y += tm.tmHeight + tm.tmExternalLeading;
			x = 0;
		}
		if (y >= rect.bottom) {
			InvalidateRect(hWnd, &rect, TRUE);
			y = 0;
		}
	}
	ReleaseDC (hWnd, hdc);
	Purge();	
}

//***********************************************************
// Function: PrintTokenId(hWnd, rect, (int)lpBlock[i])
//
// Description: Processes an escape code to the window
//
// Returns: TRUE always
//***********************************************************
BOOL PrintTokenId(HWND hwnd, RECT rect, int id) {
	HDC hdc;
	hdc = GetDC(hwnd);
	switch (id) {
		case 4:
			sprintf(str, "ISO 15693");
			break;
		case 5:
			sprintf(str, "TAG-IT HF");
			break;
		case 6:
			sprintf(str, "LF R/W");
			break;
		default:			
			return FALSE;
	}	
	GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
	TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
	TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
	x += size.cx;
	return TRUE;
}

//***********************************************************
// Function: BOOL ProcessEscCode(char* tag, int index, HDC hdc, HWND hWnd, char escChar)
//
// Description: Processes an escape code to the window
//
// Returns: TRUE always
//***********************************************************
BOOL ProcessTagId(HWND hwnd, int id, int length, int pos) {
	HDC hdc;
	bool print = FALSE;
	static stack<int> idStack;

	if (length == pos - 5) {
		while (!idStack.empty()) //clear stack
			idStack.pop();
	}
	hdc = GetDC(hwnd);
	if (idStack.empty()) {
		x = 88;
	}
	switch (pos) {
		case 17:
			if (length > pos - 8 || length < 2) {
				return FALSE;
			}
			idStack.push(id);
			if (length == 2)
				print = TRUE;
			break;
		case 16:
			if (length > pos - 8 || length < 2) {
				return FALSE;
			}
			idStack.push(id);
			if (length == 2)
				print = TRUE;
			break;
		case 11:
			if (length > pos - 8) {
				return FALSE;
			}
			idStack.push(id);
			if (length == 0)
				print = TRUE;
			break;
		default:
			return FALSE;
	}
	if (print) {
		while (!idStack.empty()) {
			sprintf(str, "%02X", idStack.top());
			GetTextExtentPoint32(hdc, (LPTSTR)str, (int)strlen(str), &size);
			TextOut(hdc, x, y, (LPTSTR)str, (int)strlen(str));
			TextOut(hdcMem, x, y, (LPTSTR)str, (int)strlen(str));
			x += size.cx;
			idStack.pop();
		}
	}
	return TRUE;
}

//***********************************************************
// Function: BOOL ProcessEscCode(char* tag, int index, HDC hdc, HWND hWnd, char escChar)
//
// Description: Processes an escape code to the window
//
// Returns: TRUE always
//***********************************************************
void ProcessEscCode(char* tag, int index, HDC hdc, HWND hWnd, char escChar) {
	char *pch;
	RECT rect;
	int n = 0, m = 0;	

	GetClientRect(hWnd, &rect);	
	GetTextMetrics(hdc, &tm);

	if (tag[0] != '\0') {
		pch = strtok(tag, ";"); 
		if (pch != NULL)
			n = atoi(pch);
		pch = strtok(NULL, ";");
		if (pch != NULL)
			m = atoi(pch);
	}

	switch (escChar) { //process esc char
		case 'H': //set cursor pos
			x = m * 8 - 8;
			y = n * (tm.tmHeight + tm.tmExternalLeading) - (tm.tmHeight + tm.tmExternalLeading);
			break;
		case 'K': //erase to EOL
			while (x < rect.right) {
				TextOut(hdc, x, y, TEXT(" "), 1);
				TextOut(hdcMem, x, y, TEXT(" "), 1);
				x += 8;
			}
			x = 0;
			break;
		case 'r': //set scroll region
			scrollRect.bottom = (tm.tmHeight + tm.tmExternalLeading)*m;
			scrollRect.top = (tm.tmHeight + tm.tmExternalLeading)*n;			
			break;
		case 'M': //scroll up 1 row
			rect.bottom = scrollRect.bottom-(tm.tmHeight + tm.tmExternalLeading);
			rect.top = scrollRect.top-((tm.tmHeight + tm.tmExternalLeading)*2);			
			ScrollDC(hdc, 0, (tm.tmHeight + tm.tmExternalLeading), &rect, NULL, NULL, NULL);
			ScrollDC(hdcMem, 0, (tm.tmHeight + tm.tmExternalLeading), &rect, NULL, NULL, NULL);
			break;
		case 'D': //scroll down 1 row
			rect.bottom = scrollRect.bottom;
			rect.top = scrollRect.top;					
			ScrollDC(hdc, 0, -(tm.tmHeight + tm.tmExternalLeading), &rect, NULL, NULL, NULL);
			ScrollDC(hdcMem, 0, -(tm.tmHeight + tm.tmExternalLeading), &rect, NULL, NULL, NULL);
			while (x < rect.right) {
				TextOut(hdcMem, x, rect.bottom-(tm.tmHeight + tm.tmExternalLeading), TEXT(" "), 1);
				TextOut(hdc, x, rect.bottom-(tm.tmHeight + tm.tmExternalLeading), TEXT(" "), 1);
				x += 8;
			}
			x = 0;			
			break;
		case 'J': //erase to end of screen
			InvalidateRect(hWnd, &rect, TRUE);
			break;
		case 'm': //set rendition
			switch (n) {
				case 0: //normal
					SetTextColor(hdc, RGB(0, 0, 0));
					SetBkColor(hdc, RGB(255, 255, 255));
					SetTextColor(hdcMem, RGB(0, 0, 0));
					SetBkColor(hdcMem, RGB(255, 255, 255));
					inverse = 0;
					break;
				case 7: //inverse colours
					SetTextColor(hdc, RGB(255, 255, 255));
					SetBkColor(hdc, RGB(0, 0, 0));
					SetTextColor(hdcMem, RGB(255, 255, 255));
					SetBkColor(hdcMem, RGB(0, 0, 0));
					inverse = 1;
					break;
				case 27: //not inverse colours
					SetTextColor(hdc, RGB(0, 0, 0));
					SetBkColor(hdc, RGB(255, 255, 255));
					SetTextColor(hdcMem, RGB(0, 0, 0));
					SetBkColor(hdcMem, RGB(255, 255, 255));
					inverse = 0;
					break;
				default:
					SetTextColor(hdc, RGB(0, 0, 0));
					SetBkColor(hdc, RGB(255, 255, 255));
					SetTextColor(hdcMem, RGB(0, 0, 0));
					SetBkColor(hdcMem, RGB(255, 255, 255));
					inverse = 0;
					break;
			}
			break;
		case 'A': //move cursor up 1 row
			x = 0;
			y -= (tm.tmHeight + tm.tmExternalLeading);
			break;
		default:
			break;
	}
}

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