#define STRICT

#pragma warning (disable: 4096)

//header files
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <string>
#include <sstream>

//macros
#define IDM_CONN		100
#define IDM_CMD			101
#define IDM_Port		102
#define IDM_Speed		103
#define IDM_Word		104
#define IDM_ZETA		105
#define IDM_Save		106
#define IDM_THETA		107
#define IDM_HELP		108
#define IDM_COM1		109
#define IDM_COM2		110
#define IDM_9600		111
#define IDM_32000		112
#define IDM_110000		113
#define IDM_EXIT		114
#define IDM_COM3		115
#define IDM_COM4		116
#define IDM_COM5		117
#define IDM_COM6		118
#define IDM_COM7		119
#define IDM_COM8		120
#define IDM_COM9		121

//methods
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
DWORD ReadThread (LPDWORD lpdwParam1);
BOOL EnterConnectMode();
BOOL EnterCommandMode();
BOOL InitializeComm();
BOOL StartComm();
BOOL StopComm();
BOOL SetPort(HWND hWnd, int portNo);
BOOL PrintTokenId(HWND hwnd, RECT rect, int id);
BOOL ProcessTagId(HWND hwnd, int id, int length, int pos);
void WriteToBlock(HWND hWnd, LPBYTE lpBlock, int nBytesRead);
void DisplayHelpBox();
void ProcessEscCode(char* tag, int index, HDC hdc, HWND hWnd, char escChar);
void GetLargestDisplayMode (int *pcxBitmap, int *pcyBitmap);
void Purge();
void SaveMemDC(HDC hdc, HWND hwnd);
void DisplayDC(HDC hdc);
void DestroyDC();
void StoreXYClient(int xx, int yy);
void SendByte(LPCVOID lpbuffer, int bufLength);
void SetProgramWindow(HWND hwnd);

