#define STRICT

#pragma warning (disable: 4096)

//header files
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <string>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

//macros
#define ID_CONNECT_COM1                 1
#define ID_CONNECT_COM2                 2
#define ID_CONNECT_COM3                 3
#define ID_CONNECT_COM4                 4
#define ID_CONNECT_COM5                 5
#define ID_CONNECT_COM6                 6
#define ID_CONNECT_COM7                 7
#define ID_CONNECT_COM8                 8
#define ID_CONNECT_COM9                 9
#define ID_MODE_URGENT                  11
#define ID_MODE_NORMAL                  12
#define ID_FILE_DISCONNECT              13
#define ID_FILE_OPENFILE                15
#define ID_FILE_CONNECT                 20
#define ID_FILE_SEND                    21
#define ID_FILE_SAVE		            1411
#define ID_FILE_EXIT                    18
#define ID_SEND_SENDFILE                16
#define ID_SEND_EMERGENCYSTOP           17

#define STATE_WAIT_FOR_ACK_ENQ		200
#define STATE_WAIT_FOR_ACK_DATA		201
#define STATE_WAIT_FOR_DATA			202
#define STATE_CHECK_DATA			203
#define STATE_WAIT_FOR_ACK_RVI		204
#define STATE_WAIT_FOR_ENQ			205
#define STATE_SEND_ACK_ENQ			206
#define STATE_SEND_ACK_DATA			207
#define STATE_SEND_RVI				208
#define STATE_SEND_ENQ				209
#define STATE_SEND_DATA				210
#define ASCII_RVI					'\x11'
#define ASCII_ENQ					'\x05'
#define ASCII_NAK					'\x15'
#define ASCII_ACK					'\x06'
#define ASCII_EOT					'\x04'
#define ASCII_DC1					'\x11'
#define NUMBER_OF_BYTES_IN_FRAME	1004

#define EDITID 1
#define UNTITLED TEXT ("(untitled)")

#define TIMER_TO_SEND_MORE_DATA			211
#define TIMER_TO_WAIT_FOR_ACK_DATA		212
#define TIMER_TO_WAIT_FOR_ACK_ENQ		213
#define TIMER_TO_WAIT_FOR_ACK_RVI		214
#define TIMER_TO_WAIT_FOR_DATA			215

//methods
std::vector<std::string> SplitFileToFrames(TCHAR*);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
DWORD ReadThread (LPDWORD lpdwParam1);
BOOL EnterConnectMode();
BOOL EnterCommandMode();
BOOL InitializeComm();
BOOL StartComm();
BOOL StopComm();
BOOL SetPort(HWND hWnd, int portNo);
void DisplayHelpBox();
void GetLargestDisplayMode (int *pcxBitmap, int *pcyBitmap);
void Purge();
void SaveMemDC(HDC hdc, HWND hwnd);
void DisplayDC(HDC hdc);
void DestroyDC();
void StoreXYClient(int xx, int yy);
void SendByte(LPCVOID lpbuffer, int bufLength);
void SetProgramWindow(HWND hwnd);
void SetState(int newState);
BOOL IdleReceiving (int state, LPSTR lpBlock, int nBytesRead);
BOOL WaitForAckEnq(LPSTR lpBlock);
BOOL SendData();
BOOL SendEnq();
BOOL SendRvi();
BOOL SendAckData();
BOOL SendAckEnq();
BOOL SendingData(int state);
BOOL WaitForEnq(LPSTR lpBlock);
BOOL WaitForAckRvi(LPSTR lpBlock);
BOOL CheckData(LPSTR lpBlock);
BOOL WaitForData (LPSTR lpBlock, int nBytesRead);
BOOL WaitForAckData(LPSTR lpBlock);
std::vector<std::string> GetFrames();
std::vector<std::string>::const_iterator GetIterator();
void SetRetriesLeft(int);
void MoveIterator();
void SetFramesToSend(bool);
void ResetBytesReceived();
bool HasFramesToSend();
bool EndFrames();
bool IsConnected();
const char* GetFrame();
const char* GetPath();
int GetRetriesLeft();
int GetState();
int GetAcks();
int GetNaks();
int GetFramesReceived();
int GetFramesSent();
double GetBitErrorRate();
void UpdateStatistics(HWND);
void SetSendMoreDataTimer();
void SetWaitForAckEnqTimer();
void SetWaitForAckRviTimer();
void SetWaitForDataTimer();
void SetWaitForAckDataTimer();
void KillSendMoreDataTimer();
void KillWaitForAckEnqTimer();
void KillWaitForAckRviTimer();
void KillWaitForDataTimer();
void KillWaitForAckDataTimer();
bool IsUrgent();

