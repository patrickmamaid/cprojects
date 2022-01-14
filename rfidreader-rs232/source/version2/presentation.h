#include "header.h"

//globals
static char str[80] = "";
static char tag[80];
static int x = 0;
static int y = 0;
static int j = 0;
static int inverse = 0;
static int escapeInit = 0;
static int cxBitmap, cyBitmap, cxClient, cyClient;
static RECT scrollRect;
static HDC hdcMem;
static HBITMAP hBitmap;
static TEXTMETRIC tm;
static SIZE size;
