#include "header.h"

static int frameCount;
static int retriesLeft;
static int bytesRead = 0;
static char frame[1004];
static char frameCheck[1004];
static char filename[256];
static int bytesRead2 = 0;
static int acksReceived = 0;
static int naksReceived = 0;
static int framesReceived = 0;
static int framesSent = 0;
