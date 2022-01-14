#include "application.h"
#include <direct.h>
#include <vector>
//***********************************************************
// Function: int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
//						  LPSTR lspszCmdParam, int nCmdShow)
//
// Description: Main-entry point
//***********************************************************

HMENU hMenu;
TCHAR szCaption[64 + MAX_PATH];		// to get name
TCHAR szPath[64 + MAX_PATH];		// to get path
TCHAR szPathName[64 + MAX_PATH];	// combine path and name

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
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
	int response ;
    TEXTMETRIC tm;
	PAINTSTRUCT paintstruct;
    static BOOL bNeedSave = FALSE;  //to do the save	
	static HWND hwndEdit ;
	static HINSTANCE hInst;
	HDC hdc;
	POINT point;

	switch (Message)
	{
		case WM_TIMER:
			switch (wParam) {
				case TIMER_TO_SEND_MORE_DATA:
					KillTimer(hwnd, TIMER_TO_SEND_MORE_DATA);
					SetRetriesLeft(4);
					SendingData(STATE_SEND_ENQ);
					break;
				case TIMER_TO_WAIT_FOR_ACK_DATA:					
					KillTimer(hwnd, TIMER_TO_WAIT_FOR_ACK_DATA);
					if (GetRetriesLeft() > 0) {
						SetRetriesLeft(GetRetriesLeft()-1);
						SetState(STATE_SEND_DATA);
						SendingData(STATE_SEND_DATA);
					} else {
						SetState(STATE_WAIT_FOR_ENQ);
						SetSendMoreDataTimer();
					}
					break;
				case TIMER_TO_WAIT_FOR_DATA:
					KillTimer(hwnd, TIMER_TO_WAIT_FOR_DATA);
					if (GetRetriesLeft() > 0) {
						SetRetriesLeft(GetRetriesLeft()-1);
						SendingData(STATE_SEND_ACK_ENQ);
					}
					break;
				case TIMER_TO_WAIT_FOR_ACK_RVI:
					KillTimer(hwnd, TIMER_TO_WAIT_FOR_ACK_RVI);
					if (GetRetriesLeft() > 0) {
						SetRetriesLeft(GetRetriesLeft()-1);
						SendingData(STATE_SEND_RVI);
					}
					break;
				case TIMER_TO_WAIT_FOR_ACK_ENQ:
					KillTimer(hwnd, TIMER_TO_WAIT_FOR_ACK_ENQ);
					if (GetRetriesLeft() > 0) {
						SetRetriesLeft(GetRetriesLeft()-1);
						SetState(STATE_SEND_ENQ);
						SendingData(STATE_SEND_ENQ);
					} else {
						SetState(STATE_WAIT_FOR_ENQ);
						SetSendMoreDataTimer();
					}
					break;
			}
			return 0;

		case WM_CREATE:
			hMenu = GetMenu(hwnd);
			SendMessage(hwnd, EM_LIMITTEXT, 32000,0L);
			hMenu = GetSubMenu(hMenu,0);
			PopFileInitialize (hwnd) ;
			hdc = GetDC(hwnd);	
			GetTextMetrics (hdc, &tm) ;			
			SaveMemDC(hdc, hwnd);
			ReleaseDC(hwnd, hdc);
			_getcwd(path, MAX_PATH);			
			return 0;

		case WM_SIZE:
			StoreXYClient(LOWORD(lParam), HIWORD (lParam));			
			return 0;

		case WM_COMMAND:
               // Messages from edit control  
			if (lParam && LOWORD (wParam) == EDITID) {
				switch (HIWORD (wParam)) {
					case EN_UPDATE :
						bNeedSave = TRUE ;
						return 0 ;     
					case EN_ERRSPACE :
					case EN_MAXTEXT :
						MessageBox (hwnd, TEXT ("Edit control out of space."),
									Name, MB_OK | MB_ICONSTOP) ;
						return 0 ;
				}
				break ;
			}
			switch (LOWORD (wParam)) {				
				case ID_CONNECT_COM1:
					checkedItem(ID_CONNECT_COM1, FALSE);
					SetPort(hwnd, 1);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM2:
					checkedItem(ID_CONNECT_COM2, FALSE);
					SetPort(hwnd, 2);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM3:
					checkedItem(ID_CONNECT_COM3, FALSE);
					SetPort(hwnd, 3);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM4:
					checkedItem(ID_CONNECT_COM4, FALSE);
					SetPort(hwnd, 4);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM5:
					checkedItem(ID_CONNECT_COM5, FALSE);
					SetPort(hwnd, 5);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM6:
					checkedItem(ID_CONNECT_COM6, FALSE);
					SetPort(hwnd, 6);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM7:
					checkedItem(ID_CONNECT_COM7, FALSE);
					SetPort(hwnd, 7);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM8:
					checkedItem(ID_CONNECT_COM8, FALSE);
					SetPort(hwnd, 8);
					EnterConnectMode();
				break;
				case ID_CONNECT_COM9:
					checkedItem(ID_CONNECT_COM9, FALSE);
					SetPort(hwnd, 9);
					EnterConnectMode();
				break;

				
				case ID_MODE_URGENT:	// check urgent
					response = MessageBox(hwnd, TEXT("Sure that you wanna change to URGENT Mode"), 
						TEXT("Warning") , MB_YESNO | MB_ICONQUESTION);

					if (response==IDYES){
						EnableMenuItem(hMenu, ID_MODE_NORMAL, FALSE);
						EnableMenuItem(hMenu, ID_MODE_URGENT, TRUE);
						urgent = true;
					}
					break;
				case ID_MODE_NORMAL:	// check normal
					response = MessageBox(hwnd, TEXT("Sure that you wanna change to NORMAL Mode"), 
						TEXT("Warning") , MB_YESNO| MB_ICONQUESTION);

					if (response==IDYES){
						EnableMenuItem(hMenu, ID_MODE_NORMAL, TRUE);
						EnableMenuItem(hMenu, ID_MODE_URGENT, FALSE);
						urgent = false;
					}
					break;

				case ID_FILE_DISCONNECT:	// to disconnect
					EnableMenuItem(hMenu, ID_FILE_DISCONNECT, TRUE);
					checkedItem(1, TRUE);
											// set the mode to default
					EnableMenuItem(hMenu, ID_MODE_NORMAL, TRUE);
					EnableMenuItem(hMenu, ID_MODE_URGENT, FALSE);

					break;

				case ID_FILE_SAVE:		// to save the file
				   if (PopFileSaveDlg (hwnd, szFileName, szTitleName)) {
						DoCaption (hwnd, szTitleName) ;
						if (PopFileWrite (hwndEdit, szFileName)) {
							 bNeedSave = FALSE ;
							 return 1 ;
						}
						else {
							 OkMessage (hwnd, TEXT ("Could not write file %s"),
										szTitleName) ;
							 return 0 ;
						}
				   }

				   EnableMenuItem(hMenu, ID_FILE_SAVE, TRUE);
				   return 0 ;

				case ID_FILE_OPENFILE:		// to Open File
				   if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName))
						return 0 ;

				   if (PopFileOpenDlg (hwnd, szFileName, szTitleName)) {
						if (!PopFileRead (hwndEdit, szFileName)) {
							 OkMessage (hwnd, TEXT ("Could not read file %s!"),
										szTitleName) ;
							 szFileName[0]  = '\0' ;
							 szTitleName[0] = '\0' ;
						} else {
							wsprintf (szCaption, TEXT ("%s"),
								szTitleName[0] ? szTitleName : UNTITLED) ;		// get name of the choosing file
						
							GetCurrentDirectory((DWORD)hwnd,szPath);		// get path of the choosing file
							
							wsprintf (szPathName, TEXT ("%s\\%s"),szPath,szCaption);
							SetWindowText(hwnd,szPathName);

							frames = SplitFileToFrames(szCaption);
							hasFramesToSend = true;
							it = frames.begin();
							if (IsConnected() && HasFramesToSend() && GetState() == STATE_WAIT_FOR_ENQ) {
								SendingData(STATE_SEND_ENQ);
							}
						}
				   }	               
				   DoCaption (hwnd, szTitleName) ;
				   bNeedSave = FALSE ;
				   return 0 ;

				case ID_SEND_SENDFILE:		// to Send File
					MessageBox(hwnd, TEXT("Send File"), TEXT("Warning") , MB_OK);
					break;

				case ID_SEND_EMERGENCYSTOP:		// Emergency Stop
					MessageBox(hwnd, TEXT("Emergency Stop"), TEXT("Warning") , MB_OK);
					break;

				case ID_FILE_EXIT:			// to exit
					exit(ID_FILE_EXIT);
					break;
			}
		break;

		case WM_CHAR:
		break;
		
		case WM_PAINT:
			hdc = BeginPaint (hwnd, &paintstruct);
			UpdateStatistics(hwnd);
			DisplayDC(hdc);
			EndPaint (hwnd, &paintstruct);
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
		case WM_RBUTTONUP:
		 point.x = LOWORD (lParam) ;
         point.y = HIWORD (lParam) ;
         ClientToScreen (hwnd, &point) ;   
         TrackPopupMenu (hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
         return 0 ;
	
		default:
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
std::vector<std::string> GetFrames() {
	return frames;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
bool HasFramesToSend() {
	return hasFramesToSend;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
std::vector<std::string>::const_iterator GetIterator() {
	return it;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
bool EndFrames() {
	if (it == frames.end()) {
		return true;
	}
	return false;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
const char* GetFrame() {
	return (*it).c_str();
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
void MoveIterator() {
	it++;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
void SetFramesToSend(bool hasFrames) {
	hasFramesToSend = hasFrames;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
const char* GetPath() {
	return path;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
bool IsUrgent() {
	return urgent;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
VOID checkedItem(int check, BOOL disconnect){
	int i; // the temporay number to uncheck the item

		// to uncheck all the item
	for(i = 1; i <= 9 ; i++)
		CheckMenuItem(hMenu, i, MF_UNCHECKED);

	if (disconnect == FALSE){
			// to check the specific item 
		CheckMenuItem(hMenu, check, MF_CHECKED);
		EnableMenuItem(hMenu, ID_FILE_DISCONNECT, FALSE);
	}
}

VOID DoCaption (HWND hwnd, TCHAR * szTitleName) {}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
VOID OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName) {
     TCHAR szBuffer[64 + MAX_PATH] ;     
     wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;     
     MessageBox (hwnd, szBuffer, Name, MB_OK | MB_ICONEXCLAMATION) ;
}

//***********************************************************
// Function: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
//                          WPARAM wParam, LPARAM lParam)
//
// Description: Message processor for the window
//***********************************************************
short AskAboutSave (HWND hwnd, TCHAR * szTitleName) {
     TCHAR szBuffer[64 + MAX_PATH] ;
     int   iReturn ;     

     wsprintf (szBuffer, TEXT ("Save current changes in %s?"),
               szTitleName[0] ? szTitleName : UNTITLED) ;
     
     iReturn = MessageBox (hwnd, szBuffer, Name,
                           MB_YESNOCANCEL | MB_ICONQUESTION) ;
     
     if (iReturn == IDYES)
          if (!SendMessage (hwnd, WM_COMMAND, ID_FILE_SAVE, 0))
               iReturn = IDCANCEL ;
          
     return iReturn ;
}