#include "header.h"
#include <vector>
#include <string>
#include <iterator>

using namespace std;

//globals
static TCHAR Name[] = TEXT("Wireless");
static std::vector<std::string> frames;
static bool hasFramesToSend = false;
static std::vector<std::string>::iterator it;
static char path[MAX_PATH];
static bool urgent = false;

BOOL PopFileOpenDlg    (HWND, PTSTR, PTSTR);
BOOL PopFileSaveDlg    (HWND, PTSTR, PTSTR);
BOOL PopFileRead       (HWND, PTSTR);
BOOL PopFileWrite      (HWND, PTSTR);
VOID checkedItem(int check, BOOL disconnect);
VOID OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName);
VOID DoCaption (HWND hwnd, TCHAR * szTitleName);
VOID PopFileInitialize (HWND);
short AskAboutSave (HWND hwnd, TCHAR * szTitleName);
