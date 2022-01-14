//---------------------------------------------------------------------------
//
//  Module: ttychic.c
//
//  Purpose:
//     This sample application demonstrates the usage of the COMM
//     API.  It implements the new Win32c API of Windows 95.
//
//     This application demonstrates how to read data by Polling.
//
//     NOTE:  no escape sequences are translated, only
//            the necessary control codes (LF, CR, BS, etc.)
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//---------------------------------------------------------------------------
//
//  Written by Microsoft Product Support Services, Windows Developer Support.
//  Copyright (c) 1991 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "tty.h"

HINSTANCE ghInst;		//global instance handle, for now.
HWND  hTTYWnd ;			//global window handle
BOOL bReading;
PTTYINFO pTTYInfo ;
COMMCONFIG cc;
DCB dcb;
COMMTIMEOUTS to;
COMMPROP cp;
HANDLE hComm, hThread;
LPCSTR lpszCommName = "COM2"; //global variable to define the serial
							  //   COM port you will be using

DWORD ReadThread (LPDWORD lpdwParam1);
void locProcessCommError (DWORD dwError);
void locProcessBytes (LPBYTE buf, DWORD dwBytes);


#if !defined(_WIN32)
#include <ver.h>
#endif

// Windows NT defines APIENTRY, but 3.x doesn't
#if !defined (APIENTRY)
#define APIENTRY far pascal
#endif

// Windows 3.x uses a FARPROC for dialogs
#if !defined(_WIN32)
#define DLGPROC FARPROC
#endif

char szDebug[100];

//
//   FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//
//   PURPOSE: calls initialization function, processes message loop
//
//   COMMENTS:
//
//      Windows recognizes this function by name as the initial entry point
//      for the program.  This function calls the application initialization
//      routine, if no other instance of the program is running, and always
//      calls the instance initialization routine.  It then executes a
//      message retrieval and dispatch loop that is the top-level control
//      structure for the remainder of execution.  The loop is terminated
//      when a WM_QUIT	message is received, at which time this function
//      exits the application instance by returning the value passed by
//      PostQuitMessage().
//
//      If this function must abort before entering the message loop, it
//      returns the conventional value NULL.
//

int APIENTRY WinMain(
               HINSTANCE hInstance,
               HINSTANCE hPrevInstance,
               LPSTR lpCmdLine,
               int nCmdShow
               )
{
   MSG msg;
   DWORD id;
   BOOL bOldState = FALSE;

	OutputDebugString ("TTY Demo Program\n");


   // Other instances of app running?
   if (!hPrevInstance) {
      // Initialize shared things
      if (!InitApplication(hInstance)) {
         return (FALSE);               // Exits if unable to initialize
      }
   }

   if (NULL == (hTTYWnd = InitInstance( hInstance, nCmdShow )))
      return ( FALSE ) ;

	ghInst = hInstance;

    CreateTTYInfo( hTTYWnd );
    if (NULL == (pTTYInfo =
          (PTTYINFO) GetWindowLong( hTTYWnd,GWL_PTTYINFO )))
	    return ( FALSE ) ;

	/* open COM port #2 for reading and writing */
	if ((hComm=CreateFile (lpszCommName, GENERIC_READ|GENERIC_WRITE, 
			0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
		/* handle error */
		MessageBox (NULL, "Error opening COM port","",MB_OK);
		return FALSE;
	} /* end if (error creating read thread) */

	/* get comm properties */
	cp.wPacketLength = sizeof(COMMPROP);
	GetCommProperties (hComm, &cp);

	/* create the thread for reading bytes */
	bReading = TRUE;
	if ((hThread=CreateThread (NULL /*def security */, 0 /* def stack size */, 
		(LPTHREAD_START_ROUTINE)ReadThread, NULL /* param to pass to thread */, 
			0, &id)) == INVALID_HANDLE_VALUE) {
		/* handle error */
		MessageBox (NULL, "Error creating READ thread","",MB_OK);
		CloseHandle (hComm);
		return FALSE;
	} /* end if (error creating read thread) */


	while (GetMessage(&msg, NULL, 0, 0)) {
   	    if (!TranslateAccelerator (msg.hwnd, ghAccel, &msg)) {
			TranslateMessage(&msg);// Translates virtual key codes
   	 		DispatchMessage(&msg); // Dispatches message to window
   	 	}
	} /* end while (not a quit message) */


	/* kill the read thread */
	bReading = FALSE;
	/* in case the thread is not running, resume it now */
	ResumeThread (hThread);
	/* wait for thread to die...  */
	while (GetExitCodeThread(hThread, &id)) {
		if (id == STILL_ACTIVE)
			continue;
		else
			break;
	} /* end while (no error reading thread exit code) */	
	CloseHandle (hThread);

	/* close COM port */
	CloseHandle (hComm);

	/* clean up */
	DestroyTTYInfo( hTTYWnd ) ;

   // Returns the value from PostQuitMessage
   return (msg.wParam);

   // This will prevent 'unused formal parameter' warnings
   lpCmdLine;
}

//---------------------------------------------------------------------------
//  BOOL NEAR InitApplication( HANDLE hInstance )
//
//  Description:
//     First time initialization stuff.  This registers information
//     such as window classes.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to this instance of the application.
//
//---------------------------------------------------------------------------

BOOL NEAR InitApplication( HANDLE hInstance )
{
   WNDCLASS  wndclass ;

   // register tty window class

   wndclass.style =         0 ;
   wndclass.lpfnWndProc =   TTYWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    sizeof( DWORD ) ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( hInstance, MAKEINTRESOURCE( TTYICON ) );
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
   wndclass.lpszMenuName =  MAKEINTRESOURCE( TTYMENU ) ;
   wndclass.lpszClassName = gszTTYClass ;

   return( RegisterClass( &wndclass ) ) ;

} // end of InitApplication()

//---------------------------------------------------------------------------
//  HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
//
//  Description:
//     Initializes instance specific information.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to instance
//
//     int nCmdShow
//        How do we show the window?
//
//---------------------------------------------------------------------------

HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
{
   HWND  hTTYWnd ;

   // load accelerators
   ghAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE( TTYACCEL ) ) ;

   // create the TTY window
   hTTYWnd = CreateWindow( gszTTYClass, gszAppName,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL ) ;

   if (NULL == hTTYWnd)
      return ( NULL ) ;

   ShowWindow( hTTYWnd, nCmdShow ) ;
   UpdateWindow( hTTYWnd ) ;

   return ( hTTYWnd ) ;

} // end of InitInstance()

//---------------------------------------------------------------------------
//  LRESULT FAR PASCAL  TTYWndProc( HWND hWnd, UINT uMsg,
//                                 WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the TTY Window Proc.  This handles ALL messages
//     to the tty window.
//
//  Parameters:
//     As documented for Window procedures.
//
//---------------------------------------------------------------------------

LRESULT FAR PASCAL  TTYWndProc( HWND hWnd, UINT uMsg,
                               WPARAM wParam, LPARAM lParam )
{
LONG lrc;

   switch (uMsg)
   {
      case WM_CREATE:
		break;

      case WM_COMMAND:
      {
         switch ((DWORD) wParam)
         {

            case ID_SETTINGS_CONFIGURE:

				/* get current port settings */
				lrc = sizeof(COMMCONFIG);				
				GetCommConfig (hComm, &cc, &lrc);
				/* display dialog to modify settings */
				if (!CommConfigDialog(lpszCommName, hWnd, &cc))
					break;
				/* write new settings */
				if (!SetCommState (hComm, &cc.dcb))
					break;
				break;

            case IDM_ABOUT:
               GoModalDialogBoxParam ( GETHINST( hWnd ),
                                       MAKEINTRESOURCE( ABOUTDLGBOX ),
                                       hWnd,
                                       AboutDlgProc, 0 ) ;
               break;

            case IDM_EXIT:
               PostMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;
         }
      }
      break ;

      case WM_PAINT:
         PaintTTY( hWnd ) ;
         break ;

      case WM_SIZE:
         SizeTTY( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         break ;

      case WM_HSCROLL:
         ScrollTTYHorz( hWnd, (WORD) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_VSCROLL:
         ScrollTTYVert( hWnd, (WORD) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_CHAR:

	/* echo to local window if echo turned on */
	if (LOCALECHO (pTTYInfo)) {
		WriteTTYBlock(hWnd, (LPBYTE)&wParam, 1 );
	}
	/* write out the character to the COM Port */
	if (!WriteFile(hComm, (LPBYTE)&wParam, 1, &lrc, NULL)) {
		/* handle error */
		locProcessCommError(GetLastError ());
	} /* end if (error reading bytes) */

	break ;

      case WM_SETFOCUS:
         SetTTYFocus( hWnd ) ;
         break ;

      case WM_KILLFOCUS:
         KillTTYFocus( hWnd ) ;
         break ;

      case WM_DESTROY:
         PostQuitMessage( 0 ) ;
         break ;

      case WM_CLOSE:
         if (IDOK != MessageBox( hWnd, "OK to close window?", "TTY Sample",
                                 MB_ICONQUESTION | MB_OKCANCEL ))
            break ;

         // fall through

      default:
         return( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;
   }
   return 0L ;

} // end of TTYWndProc()
//---------------------------------------------------------------------------
//  DWORD ReadThread(LPDWORD lpdwParam1)
//
//  Description:
//     Thread dedicated to reading bytes from the COM port
//     
//
//  Parameters:
//     LPDWORD lpdwParam1
//        Thread data (not used)
//
//---------------------------------------------------------------------------
DWORD ReadThread (LPDWORD lpdwParam1)
{
BYTE inbuff[100];
DWORD nBytesRead;

	/* The next two lines check to make sure that interval 
	timeouts are supported by the port */
	if (!(cp.dwProvCapabilities & PCF_INTTIMEOUTS))
		return 1L;  /* error;  can’t set interval timeouts */

	/* the next three lines tell the read function to return 
	immediately when there are no bytes waiting	in the
	port’s receive queue */
	memset (&to, 0, sizeof(to));
	to.ReadIntervalTimeout = MAXDWORD;
	SetCommTimeouts (hComm, &to);
	
	/* this loop polls the port reading bytes until	the
	control variable bReading is set to FALSE by the
	controlling process */
	while (bReading) {
		/* poll the port and read bytes if available */
		if (!ReadFile(hComm, inbuff, 100, &nBytesRead, NULL)) {
			/* handle error */
			locProcessCommError(GetLastError ());
		} /* end if (error reading bytes) */
		else {
			/* if there were bytes waiting,
			display them in TTY window */
			if (nBytesRead)
				locProcessBytes(inbuff, nBytesRead);
		}
	} /* end while (thread active) */

	/* clean out any pending bytes in the receive buffer */
	PurgeComm(hComm, PURGE_RXCLEAR);

	return 0L;
} /* end function (ReadThread) */

// local function to process COM errors - fill in your own error handler
void locProcessCommError (DWORD dwError)
{
DWORD lrc;
COMSTAT cs;
	/* clear error */
	ClearCommError (hComm, &lrc, &cs);
} /* end function (locProcessCommError) */

// local function to process bytes read from the COM port - just displays them in the terminal window;
// put your own handler code here
void locProcessBytes (LPBYTE buf, DWORD dwBytes)
{
	WriteTTYBlock(hTTYWnd, buf, dwBytes);
} /* end function (locProcessBytes) */


//---------------------------------------------------------------------------
//  LRESULT NEAR CreateTTYInfo( HWND hWnd )
//
//  Description:
//     Creates the tty information structure and sets
//     menu option availability.  Returns -1 if unsuccessful.
//
//  Parameters:
//     HWND  hWnd
//        Handle to main window.
//
//---------------------------------------------------------------------------

LRESULT NEAR CreateTTYInfo( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   if (NULL == (pTTYInfo =
                   (PTTYINFO) LocalAlloc( LPTR, sizeof( TTYINFO ) )))
      return ( (LRESULT) -1 ) ;

   // initialize TTY info structure

   COMDEV( pTTYInfo )        = 0 ;
   CONNECTED( pTTYInfo )     = FALSE ;
   CURSORSTATE( pTTYInfo )   = CS_HIDE ;
   LOCALECHO( pTTYInfo )     = TRUE ;
   AUTOWRAP( pTTYInfo )      = TRUE ;
   PORT( pTTYInfo )          = 1 ;
   BAUDRATE( pTTYInfo )      = CBR_9600 ;
   BYTESIZE( pTTYInfo )      = 8 ;
   FLOWCTRL( pTTYInfo )      = FC_RTSCTS ;
   PARITY( pTTYInfo )        = NOPARITY ;
   STOPBITS( pTTYInfo )      = ONESTOPBIT ;
   XONXOFF( pTTYInfo )       = FALSE ;
   XSIZE( pTTYInfo )         = 0 ;
   YSIZE( pTTYInfo )         = 0 ;
   XSCROLL( pTTYInfo )       = 0 ;
   YSCROLL( pTTYInfo )       = 0 ;
   XOFFSET( pTTYInfo )       = 0 ;
   YOFFSET( pTTYInfo )       = 0 ;
   COLUMN( pTTYInfo )        = 0 ;
   ROW( pTTYInfo )           = 0 ;
   HTTYFONT( pTTYInfo )      = NULL ;
   FGCOLOR( pTTYInfo )       = RGB( 0, 0, 0 ) ;
   USECNRECEIVE( pTTYInfo )  = TRUE ;
   DISPLAYERRORS( pTTYInfo ) = TRUE ;
   NEWLINE(pTTYInfo)		 = TRUE ;
	
   // clear screen space

   memset( SCREEN( pTTYInfo ), ' ', MAXROWS * MAXCOLS ) ;

   // setup default font information

   LFTTYFONT( pTTYInfo ).lfHeight =         12 ;
   LFTTYFONT( pTTYInfo ).lfWidth =          0 ;
   LFTTYFONT( pTTYInfo ).lfEscapement =     0 ;
   LFTTYFONT( pTTYInfo ).lfOrientation =    0 ;
   LFTTYFONT( pTTYInfo ).lfWeight =         0 ;
   LFTTYFONT( pTTYInfo ).lfItalic =         0 ;
   LFTTYFONT( pTTYInfo ).lfUnderline =      0 ;
   LFTTYFONT( pTTYInfo ).lfStrikeOut =      0 ;
   LFTTYFONT( pTTYInfo ).lfCharSet =        OEM_CHARSET ;
   LFTTYFONT( pTTYInfo ).lfOutPrecision =   OUT_DEFAULT_PRECIS ;
   LFTTYFONT( pTTYInfo ).lfClipPrecision =  CLIP_DEFAULT_PRECIS ;
   LFTTYFONT( pTTYInfo ).lfQuality =        DEFAULT_QUALITY ;
   LFTTYFONT( pTTYInfo ).lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
   LFTTYFONT( pTTYInfo ).lfFaceName[0] =    0;

   // set TTYInfo handle before any further message processing.

   SetWindowLong( hWnd, GWL_PTTYINFO, (WPARAM) pTTYInfo ) ;

   // reset the character information, etc.

   ResetTTYScreen( hWnd, pTTYInfo ) ;

   return ( (LRESULT) TRUE ) ;

} // end of CreateTTYInfo()

//---------------------------------------------------------------------------
//  BOOL NEAR DestroyTTYInfo( HWND hWnd )
//
//  Description:
//     Destroys block associated with TTY window handle.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//---------------------------------------------------------------------------

BOOL NEAR DestroyTTYInfo( HWND hWnd )
{
   PTTYINFO pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   DeleteObject( HTTYFONT( pTTYInfo ) ) ;

   LocalFree( pTTYInfo ) ;
   return ( TRUE ) ;

} // end of DestroyTTYInfo()

//---------------------------------------------------------------------------
//  BOOL NEAR ResetTTYScreen( HWND hWnd, PTTYINFO pTTYInfo )
//
//  Description:
//     Resets the TTY character information and causes the
//     screen to resize to update the scroll information.
//
//  Parameters:
//     PTTYINFO  pTTYInfo
//        pointer to TTY info structure
//
//---------------------------------------------------------------------------

BOOL NEAR ResetTTYScreen( HWND hWnd, PTTYINFO pTTYInfo )
{
   HDC         hDC ;
   TEXTMETRIC  tm ;
   RECT        rcWindow ;

   if (NULL == pTTYInfo)
      return ( FALSE ) ;

   if (NULL != HTTYFONT( pTTYInfo ))
      DeleteObject( HTTYFONT( pTTYInfo ) ) ;

   HTTYFONT( pTTYInfo ) = CreateFontIndirect( &LFTTYFONT( pTTYInfo ) ) ;

   hDC = GetDC( hWnd ) ;
   SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   GetTextMetrics( hDC, &tm ) ;
   ReleaseDC( hWnd, hDC ) ;

   XCHAR( pTTYInfo ) = tm.tmAveCharWidth  ;
   YCHAR( pTTYInfo ) = tm.tmHeight + tm.tmExternalLeading ;

   // a slimy hack to force the scroll position, region to
   // be recalculated based on the new character sizes

   GetWindowRect( hWnd, &rcWindow ) ;
   SendMessage( hWnd, WM_SIZE, SIZENORMAL,
                (LPARAM) MAKELONG( rcWindow.right - rcWindow.left,
                                   rcWindow.bottom - rcWindow.top ) ) ;

   return ( TRUE ) ;

} // end of ResetTTYScreen()

//---------------------------------------------------------------------------
//  BOOL NEAR PaintTTY( HWND hWnd )
//
//  Description:
//     Paints the rectangle determined by the paint struct of
//     the DC.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window (as always)
//
//---------------------------------------------------------------------------

BOOL NEAR PaintTTY( HWND hWnd )
{
   int          nRow, nCol, nEndRow, nEndCol, nCount, nHorzPos, nVertPos ;
   HDC          hDC ;
   HFONT        hOldFont ;
   PTTYINFO    pTTYInfo ;
   PAINTSTRUCT  ps ;
   RECT         rect ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   hDC = BeginPaint( hWnd, &ps ) ;
   hOldFont = SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   SetTextColor( hDC, FGCOLOR( pTTYInfo ) ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
   rect = ps.rcPaint ;
   nRow =
      min( MAXROWS - 1,
           max( 0, (rect.top + YOFFSET( pTTYInfo )) / YCHAR( pTTYInfo ) ) ) ;
   nEndRow =
      min( MAXROWS - 1,
           ((rect.bottom + YOFFSET( pTTYInfo ) - 1) / YCHAR( pTTYInfo ) ) ) ;
   nCol =
      min( MAXCOLS - 1,
           max( 0, (rect.left + XOFFSET( pTTYInfo )) / XCHAR( pTTYInfo ) ) ) ;
   nEndCol =
      min( MAXCOLS - 1,
           ((rect.right + XOFFSET( pTTYInfo ) - 1) / XCHAR( pTTYInfo ) ) ) ;
   nCount = nEndCol - nCol + 1 ;
   for (; nRow <= nEndRow; nRow++)
   {
      nVertPos = (nRow * YCHAR( pTTYInfo )) - YOFFSET( pTTYInfo ) ;
      nHorzPos = (nCol * XCHAR( pTTYInfo )) - XOFFSET( pTTYInfo ) ;
      rect.top = nVertPos ;
      rect.bottom = nVertPos + YCHAR( pTTYInfo ) ;
      rect.left = nHorzPos ;
      rect.right = nHorzPos + XCHAR( pTTYInfo ) * nCount ;
      SetBkMode( hDC, OPAQUE ) ;
      ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE | ETO_CLIPPED, &rect,
                  (LPSTR)( SCREEN( pTTYInfo ) + nRow * MAXCOLS + nCol ),
                  nCount, NULL ) ;
   }
   SelectObject( hDC, hOldFont ) ;
   EndPaint( hWnd, &ps ) ;
   MoveTTYCursor( hWnd ) ;
   return ( TRUE ) ;

} // end of PaintTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR SizeTTY( HWND hWnd, WORD wVertSize, WORD wHorzSize )
//
//  Description:
//     Sizes TTY and sets up scrolling regions.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wVertSize
//        new vertical size
//
//     WORD wHorzSize
//        new horizontal size
//
//---------------------------------------------------------------------------

BOOL NEAR SizeTTY( HWND hWnd, WORD wVertSize, WORD wHorzSize )
{
   int        nScrollAmt ;
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   YSIZE( pTTYInfo ) = (int) wVertSize ;
   YSCROLL( pTTYInfo ) = max( 0, (MAXROWS * YCHAR( pTTYInfo )) -
                               YSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( YSCROLL( pTTYInfo ), YOFFSET( pTTYInfo ) ) -
                     YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;

   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL( pTTYInfo ), TRUE ) ;

   XSIZE( pTTYInfo ) = (int) wHorzSize ;
   XSCROLL( pTTYInfo ) = max( 0, (MAXCOLS * XCHAR( pTTYInfo )) -
                                XSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( XSCROLL( pTTYInfo ), XOFFSET( pTTYInfo )) -
                     XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL( pTTYInfo ), TRUE ) ;

   InvalidateRect( hWnd, NULL, TRUE ) ;

   return ( TRUE ) ;

} // end of SizeTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
//
//  Description:
//     Scrolls TTY window vertically.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wScrollCmd
//        type of scrolling we're doing
//
//     WORD wScrollPos
//        scroll position
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int        nScrollAmt ;
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET( pTTYInfo ) ;
         break ;

      default:
         return ( FALSE ) ;
   }
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) > YSCROLL( pTTYInfo ))
      nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), TRUE ) ;

   return ( TRUE ) ;

} // end of ScrollTTYVert()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
//
//  Description:
//     Scrolls TTY window horizontally.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wScrollCmd
//        type of scrolling we're doing
//
//     WORD wScrollPos
//        scroll position
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int        nScrollAmt ;
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET( pTTYInfo ) ;
         break ;

      default:
         return ( FALSE ) ;
   }
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) > XSCROLL( pTTYInfo ))
      nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, -nScrollAmt, 0, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), TRUE ) ;

   return ( TRUE ) ;

} // end of ScrollTTYHorz()

//---------------------------------------------------------------------------
//  BOOL NEAR SetTTYFocus( HWND hWnd )
//
//  Description:
//     Sets the focus to the TTY window also creates caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//---------------------------------------------------------------------------

BOOL NEAR SetTTYFocus( HWND hWnd )
{
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   if (CURSORSTATE( pTTYInfo ) != CS_SHOW)
   {
      CreateCaret( hWnd, NULL, XCHAR( pTTYInfo ), YCHAR( pTTYInfo ) ) ;
      ShowCaret( hWnd ) ;
      CURSORSTATE( pTTYInfo ) = CS_SHOW ;
   }
   MoveTTYCursor( hWnd ) ;
   return ( TRUE ) ;

} // end of SetTTYFocus()

//---------------------------------------------------------------------------
//  BOOL NEAR KillTTYFocus( HWND hWnd )
//
//  Description:
//     Kills TTY focus and destroys the caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//---------------------------------------------------------------------------

BOOL NEAR KillTTYFocus( HWND hWnd )
{
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   if (CURSORSTATE( pTTYInfo ) != CS_HIDE)
   {
      HideCaret( hWnd ) ;
      DestroyCaret() ;
      CURSORSTATE( pTTYInfo ) = CS_HIDE ;
   }
   return ( TRUE ) ;

} // end of KillTTYFocus()

//---------------------------------------------------------------------------
//  BOOL NEAR MoveTTYCursor( HWND hWnd )
//
//  Description:
//     Moves caret to current position.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//---------------------------------------------------------------------------

BOOL NEAR MoveTTYCursor( HWND hWnd )
{
   PTTYINFO  pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   if (CURSORSTATE( pTTYInfo ) & CS_SHOW)
      SetCaretPos( (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                   XOFFSET( pTTYInfo ),
                   (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                   YOFFSET( pTTYInfo ) ) ;

   return ( TRUE ) ;

} // end of MoveTTYCursor()
//---------------------------------------------------------------------------
//  BOOL  WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
//
//  Description:
//     Writes block to TTY screen.  Nothing fancy - just
//     straight TTY.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpBlock
//        far pointer to block of data
//
//     int nLength
//        length of block
//
//---------------------------------------------------------------------------

BOOL  WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
{
   int        i ;
   PTTYINFO  pTTYInfo ;
   RECT       rect ;

   if (NULL == (pTTYInfo = (PTTYINFO) GetWindowLong( hWnd, GWL_PTTYINFO )))
      return ( FALSE ) ;

   for (i = 0 ; i < nLength; i++)
   {
      switch (lpBlock[ i ])
      {
         case ASCII_BEL:
            // Bell
            MessageBeep( 0 ) ;
            break ;

         case ASCII_BS:
            // Backspace
            if (COLUMN( pTTYInfo ) > 0)
               COLUMN( pTTYInfo ) -- ;
            MoveTTYCursor( hWnd ) ;
            break ;

         case ASCII_CR:
            // Carriage return
            COLUMN( pTTYInfo ) = 0 ;
            MoveTTYCursor( hWnd ) ;
            if (!NEWLINE( pTTYInfo ))
               break;

            // fall through

         case ASCII_LF:
            // Line feed
            if (ROW( pTTYInfo )++ == MAXROWS - 1)
            {
               memmove( (LPSTR) (SCREEN( pTTYInfo )),
                          (LPSTR) (SCREEN( pTTYInfo ) + MAXCOLS),
                          (MAXROWS - 1) * MAXCOLS ) ;
               memset( (LPSTR) (SCREEN( pTTYInfo ) + (MAXROWS - 1) * MAXCOLS),
                         ' ', MAXCOLS ) ;
               InvalidateRect( hWnd, NULL, FALSE ) ;
               ROW( pTTYInfo )-- ;
            }
            MoveTTYCursor( hWnd ) ;
            break ;

         default:
            *(SCREEN( pTTYInfo ) + ROW( pTTYInfo ) * MAXCOLS +
                COLUMN( pTTYInfo )) = lpBlock[ i ] ;
            rect.left = (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                        XOFFSET( pTTYInfo ) ;
            rect.right = rect.left + XCHAR( pTTYInfo ) ;
            rect.top = (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                       YOFFSET( pTTYInfo ) ;
            rect.bottom = rect.top + YCHAR( pTTYInfo ) ;
            InvalidateRect( hWnd, &rect, FALSE ) ;

            // Line wrap
            if (COLUMN( pTTYInfo ) < MAXCOLS - 1)
               COLUMN( pTTYInfo )++ ;
            else if (AUTOWRAP( pTTYInfo ))
               WriteTTYBlock( hWnd, "\r\n", 2 ) ;
            break;
      }
   }
   return ( TRUE ) ;

} // end of WriteTTYBlock()

//---------------------------------------------------------------------------
//  VOID  GoModalDialogBoxParam( HINSTANCE hInstance,
//                                   LPCSTR lpszTemplate, HWND hWnd,
//                                   DLGPROC lpDlgProc, LPARAM lParam )
//
//  Description:
//     It is a simple utility function that simply performs the
//     MPI and invokes the dialog box with a DWORD paramter.
//
//  Parameters:
//     similar to that of DialogBoxParam() with the exception
//     that the lpDlgProc is not a procedure instance
//
//---------------------------------------------------------------------------

VOID  GoModalDialogBoxParam( HINSTANCE hInstance, LPCSTR lpszTemplate,
                                 HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam )
{
   DLGPROC  lpProcInstance ;

   lpProcInstance = (DLGPROC) MakeProcInstance( (FARPROC) lpDlgProc,
                                                hInstance ) ;
   DialogBoxParam( hInstance, lpszTemplate, hWnd, lpProcInstance, lParam ) ;
   FreeProcInstance( (FARPROC) lpProcInstance ) ;

} // end of GoModalDialogBoxParam()

//---------------------------------------------------------------------------
//  BOOL FAR PASCAL  AboutDlgProc( HWND hDlg, UINT uMsg,
//                                WPARAM wParam, LPARAM lParam )
//
//  Description:
//     Simulates the Windows System Dialog Box.
//
//  Parameters:
//     Same as standard dialog procedures.
//
//---------------------------------------------------------------------------

BOOL FAR PASCAL  AboutDlgProc( HWND hDlg, UINT uMsg,
                              WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         int          idModeString ;
         char         szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
         DWORD        dwFreeMemory;
         WORD         wRevision, wVersion ;

#ifdef ABOUTDLG_USEBITMAP
         // if we are using the bitmap, hide the icon

         ShowWindow( GetDlgItem( hDlg, IDD_ABOUTICON ), SW_HIDE ) ;
#endif
         // sets up the version number for Windows

         wVersion = LOWORD( GetVersion() ) ;
         switch (HIBYTE( wVersion ))
         {
            case 10:
               wRevision = 1 ;
               break ;

            default:
               wRevision = 0 ;
               break;
         }
         wVersion &= 0xFF ;

         GetDlgItemText( hDlg, IDD_TITLELINE, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, wVersion, wRevision ) ;
         SetDlgItemText( hDlg, IDD_TITLELINE, szBuffer ) ;

         // sets up version number for TTY

         GetDlgItemText( hDlg, IDD_VERSION, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, VER_MAJOR, VER_MINOR, VER_BUILD ) ;
         SetDlgItemText( hDlg, IDD_VERSION, (LPSTR) szBuffer ) ;

         // get by-line

         LoadString( GETHINST( hDlg ), IDS_BYLINE, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_BYLINE, szBuffer ) ;

         LoadString( GETHINST( hDlg ), idModeString, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_WINDOWSMODE, szBuffer ) ;

         // get free memory information

         dwFreeMemory = GetFreeSpace( 0 ) / 1024L ;
         GetDlgItemText( hDlg, IDD_FREEMEM, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, dwFreeMemory ) ;
         SetDlgItemText( hDlg, IDD_FREEMEM, (LPSTR) szBuffer ) ;
      }
      return ( TRUE ) ;

#ifdef ABOUTDLG_USEBITMAP
      // used to paint the bitmap

      case WM_PAINT:
      {
         HBITMAP      hBitMap ;
         HDC          hDC, hMemDC ;
         PAINTSTRUCT  ps ;

         // load bitmap and display it

         hDC = BeginPaint( hDlg, &ps ) ;
         if (NULL != (hMemDC = CreateCompatibleDC( hDC )))
         {
            hBitMap = LoadBitmap( GETHINST( hDlg ),
                                  MAKEINTRESOURCE( TTYBITMAP ) ) ;
            hBitMap = SelectObject( hMemDC, hBitMap ) ;
            BitBlt( hDC, 10, 10, 64, 64, hMemDC, 0, 0, SRCCOPY ) ;
            DeleteObject( SelectObject( hMemDC, hBitMap ) ) ;
            DeleteDC( hMemDC ) ;
         }
         EndPaint( hDlg, &ps ) ;
      }
      break ;
#endif

      case WM_COMMAND:
         if ((WORD) wParam == IDD_OK)
         {
            EndDialog( hDlg, TRUE ) ;
            return ( TRUE ) ;
         }
         break;
   }
   return ( FALSE ) ;

} // end of AboutDlgProc()

//---------------------------------------------------------------------------
//  VOID  FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
//                          DWORD  *npTable, WORD wTableLen,
//                          WORD wCurrentSetting )
//
//  Description:
//     Fills the given combo box with strings from the resource
//     table starting at nIDString.  Associated items are
//     added from given table.  The combo box is notified of
//     the current setting.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     HWND hCtrlWnd
//        handle to combo box control
//
//     int nIDString
//        first resource string id
//
//     WORD  *npTable
//        near point to table of associated values
//
//     WORD wTableLen
//        length of table
//
//     WORD wCurrentSetting
//        current setting (for combo box selection)
//
//---------------------------------------------------------------------------

VOID  FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
                        DWORD  *npTable, WORD wTableLen,
                        WORD wCurrentSetting )
{
   char  szBuffer[ MAXLEN_TEMPSTR ] ;
   WORD  wCount, wPosition ;

   for (wCount = 0; wCount < wTableLen; wCount++)
   {
      // load the string from the string resources and
      // add it to the combo box

      LoadString( hInstance, nIDString + wCount, szBuffer, sizeof( szBuffer ) ) ;
      wPosition = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING, 0,
                                       (LPARAM) (LPSTR) szBuffer ) ) ;

      // use item data to store the actual table value

      SendMessage( hCtrlWnd, CB_SETITEMDATA, (WPARAM) wPosition,
                   (LPARAM) (LONG) *(npTable + wCount) ) ;

      // if this is our current setting, select it

      if (*(npTable + wCount) == wCurrentSetting)
         SendMessage( hCtrlWnd, CB_SETCURSEL, (WPARAM) wPosition, 0 ) ;
   }

} // end of FillComboBox()
//---------------------------------------------------------------------------
//  BOOL  SelectTTYFont( HWND hDlg )
//
//  Description:
//     Selects the current font for the TTY screen.
//     Uses the Common Dialog ChooseFont() API.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//---------------------------------------------------------------------------

BOOL  SelectTTYFont( HWND hDlg )
{
   CHOOSEFONT  cfTTYFont ;
   PTTYINFO   pTTYInfo ;

   if (NULL == (pTTYInfo = (PTTYINFO) GET_PROP( hDlg, ATOM_TTYINFO )))
      return ( FALSE ) ;

   cfTTYFont.lStructSize    = sizeof( CHOOSEFONT ) ;
   cfTTYFont.hwndOwner      = hDlg ;
   cfTTYFont.hDC            = NULL ;
   cfTTYFont.rgbColors      = FGCOLOR( pTTYInfo ) ;
   cfTTYFont.lpLogFont      = &LFTTYFONT( pTTYInfo ) ;
   cfTTYFont.Flags          = CF_SCREENFONTS | CF_FIXEDPITCHONLY |
                              CF_EFFECTS | CF_INITTOLOGFONTSTRUCT ;
   cfTTYFont.lCustData      = 0 ;
   cfTTYFont.lpfnHook       = NULL ;
   cfTTYFont.lpTemplateName = NULL ;
   cfTTYFont.hInstance      = GETHINST( hDlg ) ;

   if (ChooseFont( &cfTTYFont ))
   {
     FGCOLOR( pTTYInfo ) = cfTTYFont.rgbColors ;
     ResetTTYScreen( GetParent( hDlg ), pTTYInfo ) ;
   }

   return ( TRUE ) ;

} // end of SelectTTYFont()

//---------------------------------------------------------------------------
//  End of File: tty.c
//---------------------------------------------------------------------------

