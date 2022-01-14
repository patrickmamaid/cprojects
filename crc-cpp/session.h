#include "header.h"

#define BYTE_POS_STX					0
#define BYTE_POS_STX_FRAME				1
#define BYTE_POS_FILENAME				2
#define BYTE_POS_ETX_FRAME				1000
#define BYTE_POS_ETX					1001
#define BYTE_POS_CHECKSUM				1002
#define ASCII_DC2						'\x12'
#define ASCII_DC3						'\x13'
#define ASCII_STX						'\x02'
#define ASCII_ETX						'\x03'
#define ASCII_NULL						'\x00'
#define DIR_OUTPUT						"H:/dcomm2/dcomm/dcomm/output.txt"
#define DIR_INPUT						"C:/Documents and Settings/df/Desktop/a4wire/debug/test.txt"

using std::vector;
using std::string;
using std::ifstream;

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
static int state;
