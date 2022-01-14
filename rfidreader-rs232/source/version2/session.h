#include "header.h"

//globals
static int commNo = 0;
static int connectMode = 0;
static int portSet = 0;
static HWND programWnd;
static COMMCONFIG cc;
static COMMTIMEOUTS to;
static HANDLE hThread;
static DCB dcb;
static BOOL bReading;
static DWORD id;
static DWORD nBytesRead;
static TCHAR Name[] = TEXT("Dumb Terminal");
static HANDLE hComm;