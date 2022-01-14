//---------------------------------------------------------------------------
//
//  Module: tty.h
//
//  Purpose:
//     This is the header file for the TTY sample.
//
//---------------------------------------------------------------------------

#define USECOMM      // yes, we need the COMM API
#define STRICT       // be bold!

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>

#include "version.h"
#include "resource.h"

// constant definitions

#define GWL_PTTYINFO       0
#define ABOUTDLG_USEBITMAP  1

#define ATOM_TTYINFO       0x100

// terminal size

#define MAXROWS         25
#define MAXCOLS         80

#define MAXBLOCK        80

#define MAXLEN_TEMPSTR  81

#define RXQUEUE         4096
#define TXQUEUE         4096

// cursor states

#define CS_HIDE         0x00
#define CS_SHOW         0x01

// Flow control flags

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions

#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

// data structures

typedef struct tagTTYINFO
{
   HANDLE  idComDev ;
   BYTE    bPort, abScreen[ MAXROWS * MAXCOLS ] ;
   BOOL    fConnected, fXonXoff, fLocalEcho, fNewLine, fAutoWrap,
           fUseCNReceive, fDisplayErrors;
   BYTE    bByteSize, bFlowCtrl, bParity, bStopBits ;
   WORD    wBaudRate, wCursorState ;
   HFONT   hTTYFont ;
   LOGFONT lfTTYFont ;
   DWORD   rgbFGColor ;
   int     xSize, ySize, xScroll, yScroll, xOffset, yOffset,
           nColumn, nRow, xChar, yChar ;

} TTYINFO, *PTTYINFO ;

// macros ( for easier readability )

#define GETHINST( hWnd )  ((HINSTANCE) GetWindowLong( hWnd, GWL_HINSTANCE ))

#define COMDEV( x ) (x -> idComDev)
#define PORT( x )   (x -> bPort)
#define SCREEN( x ) (x -> abScreen)
#define CONNECTED( x ) (x -> fConnected)
#define XONXOFF( x ) (x -> fXonXoff)
#define LOCALECHO( x ) (x -> fLocalEcho)
#define NEWLINE( x ) (x -> fNewLine)
#define AUTOWRAP( x ) (x -> fAutoWrap)
#define BYTESIZE( x ) (x -> bByteSize)
#define FLOWCTRL( x ) (x -> bFlowCtrl)
#define PARITY( x ) (x -> bParity)
#define STOPBITS( x ) (x -> bStopBits)
#define BAUDRATE( x ) (x -> wBaudRate)
#define CURSORSTATE( x ) (x -> wCursorState)
#define HTTYFONT( x ) (x -> hTTYFont)
#define LFTTYFONT( x ) (x -> lfTTYFont)
#define FGCOLOR( x ) (x -> rgbFGColor)
#define XSIZE( x ) (x -> xSize)
#define YSIZE( x ) (x -> ySize)
#define XSCROLL( x ) (x -> xScroll)
#define YSCROLL( x ) (x -> yScroll)
#define XOFFSET( x ) (x -> xOffset)
#define YOFFSET( x ) (x -> yOffset)
#define COLUMN( x ) (x -> nColumn)
#define ROW( x ) (x -> nRow)
#define XCHAR( x ) (x -> xChar)
#define YCHAR( x ) (x -> yChar )
#define USECNRECEIVE( x ) (x -> fUseCNReceive)
#define DISPLAYERRORS( x ) (x -> fDisplayErrors)

#define SET_PROP( x, y, z )  SetProp( x, MAKEINTATOM( y ), z )
#define GET_PROP( x, y )     GetProp( x, MAKEINTATOM( y ) )
#define REMOVE_PROP( x, y )  RemoveProp( x, MAKEINTATOM( y ) )

// global stuff

char     gszTTYClass[] = "TTYWndClass" ;
char     gszAppName[] = "TTY" ;
HANDLE   ghAccel ;
DWORD     gawBaudTable[] = { CBR_110,
                            CBR_300,
                            CBR_600,
                            CBR_1200,
                            CBR_2400,
                            CBR_4800,
                            CBR_9600,
                            CBR_14400,
                            CBR_19200,
                            CBR_38400,
                            CBR_56000,
                            CBR_128000,
                            CBR_256000   } ;

DWORD     gawParityTable[] = { NOPARITY,
                              EVENPARITY,
                              ODDPARITY,
                              MARKPARITY,
                              SPACEPARITY } ;

DWORD     gawStopBitsTable[] = { ONESTOPBIT,
                                ONE5STOPBITS,
                                TWOSTOPBITS } ;

// function prototypes (private)

BOOL InitApplication( HANDLE ) ;
HWND InitInstance( HANDLE, int ) ;
LRESULT CreateTTYInfo( HWND ) ;
BOOL DestroyTTYInfo( HWND ) ;
BOOL  ResetTTYScreen( HWND, PTTYINFO ) ;
BOOL  KillTTYFocus( HWND ) ;
BOOL  PaintTTY( HWND ) ;
BOOL  SetTTYFocus( HWND ) ;
BOOL  ScrollTTYHorz( HWND, WORD, WORD ) ;
BOOL  ScrollTTYVert( HWND, WORD, WORD ) ;
BOOL  SizeTTY( HWND, WORD, WORD ) ;
BOOL  ProcessTTYCharacter( HWND, BYTE ) ;
BOOL  WriteTTYBlock( HWND, LPSTR, int ) ;
int  ReadCommBlock( HWND, LPSTR, int ) ;
BOOL  WriteCommByte( HWND, BYTE ) ;
BOOL  MoveTTYCursor( HWND ) ;
BOOL  OpenConnection( HWND ) ;
BOOL  SetupConnection() ;
BOOL  CloseConnection( HWND ) ;
BOOL  ProcessCOMMNotification( HWND, WORD, LONG ) ;
VOID  GoModalDialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM ) ;
VOID  FillComboBox( HINSTANCE, HWND, int, DWORD  *, WORD, WORD ) ;
BOOL  SelectTTYFont( HWND ) ;
BOOL  SettingsDlgInit( HWND ) ;
BOOL  SettingsDlgTerm( HWND ) ;

// function prototypes (public)

LRESULT FAR PASCAL TTYWndProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL AboutDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL SettingsDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL DCBDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL CommpropDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL TimeoutDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL PhoneNumberDlgProc( HWND hDlg, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam );
BOOL FAR PASCAL SendFileDlgProc( HWND hDlg, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam );

//---------------------------------------------------------------------------
//  End of File: tty.h
//---------------------------------------------------------------------------

