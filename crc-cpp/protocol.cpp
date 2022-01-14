#include "protocol.h"

using namespace std;
//***********************************************************
// Function: BOOL IdleReceiving (int state, 
//								 LPSTR lpBlock,
//								 int nBytesRead)
//
// Description: Handles all waiting states in the protocol.
//***********************************************************
BOOL IdleReceiving (int state, LPSTR lpBlock, int nBytesRead) {
	switch (state) {
		case STATE_WAIT_FOR_ACK_ENQ:
			WaitForAckEnq(lpBlock);
			break;
		case STATE_WAIT_FOR_ACK_DATA:
			WaitForAckData(lpBlock);
			break;
		case STATE_WAIT_FOR_DATA:
			WaitForData(lpBlock, nBytesRead);
			break;
		case STATE_CHECK_DATA:
			CheckData(lpBlock);
			break;
		case STATE_WAIT_FOR_ACK_RVI:
			WaitForAckRvi(lpBlock);
			break;
		case STATE_WAIT_FOR_ENQ:
			WaitForEnq(lpBlock);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL WaitForAckEnq(LPSTR lpBlock) {
	KillWaitForAckEnqTimer();
	if (lpBlock[0] == ASCII_ACK) {
		acksReceived++;
		SetRetriesLeft(4);
		frameCount = 5;
		SetState(STATE_SEND_DATA);
		SendingData(STATE_SEND_DATA);
		return TRUE;
	} else {
		if (retriesLeft == 0) {
			SetState(STATE_WAIT_FOR_ENQ);
		} else {
			retriesLeft--;
			SetState(STATE_SEND_ENQ);
			SendingData(STATE_SEND_ENQ);
		}
	}
	return FALSE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL WaitForAckData(LPSTR lpBlock) {
	if (lpBlock[0] == ASCII_ACK) {
		acksReceived++;
		frameCount--;
		MoveIterator();
		if (EndFrames())
			SetFramesToSend(false);		
		if (frameCount > 0 && HasFramesToSend()){
			SetState(STATE_SEND_DATA);
			SendingData(STATE_SEND_DATA);
		} else {
			if (HasFramesToSend())
				SetSendMoreDataTimer();
			SetState(STATE_WAIT_FOR_ENQ);
			SendByte("\x04", 1);
		}
	} else if (lpBlock[0] == ASCII_DC1) {
		MoveIterator();
		SetState(STATE_WAIT_FOR_ENQ);
		SendByte("\x06", 1);
		SetSendMoreDataTimer();
	} else { //NAK
		naksReceived++;
		if (retriesLeft > 0) {
			retriesLeft--;
			SetState(STATE_SEND_DATA);
			SendingData(STATE_SEND_DATA);
		} else {
			if (HasFramesToSend())
				SetSendMoreDataTimer();
			SetState(STATE_WAIT_FOR_ENQ);
			SendByte("\x04", 1);
		}
	}
	return FALSE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL WaitForData (LPSTR lpBlock, int nBytesRead) {
	if (lpBlock[0] == ASCII_EOT) {
		if (HasFramesToSend()) {
			SetRetriesLeft(4);
			SetState(STATE_SEND_ENQ);
			SendingData(STATE_SEND_ENQ);
		} else {
			SetState(STATE_WAIT_FOR_ENQ);
		}
		return TRUE;
	}
	for (int i = 0; i < nBytesRead; i++) {
		frame[bytesRead++] = lpBlock[i];
	}
	bytesRead2 += nBytesRead;
	if (bytesRead2 >= NUMBER_OF_BYTES_IN_FRAME-1) {
		SetState(STATE_CHECK_DATA);
		IdleReceiving(STATE_CHECK_DATA, frame, NUMBER_OF_BYTES_IN_FRAME);
		framesReceived++;
		sprintf_s(frame, NUMBER_OF_BYTES_IN_FRAME, "");
		bytesRead = 0;
		bytesRead2 = 0;
	}
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL CheckData(LPSTR lpBlock) {
	// perform checksum

	if (!1) { // invalid
		SendByte("\x15", 1);
		SetState(STATE_WAIT_FOR_DATA);
		return FALSE;
	} else { // valid
		int i = 1;
		FILE *output;

		if (frame[i] == '\x12') { // filename
			i++;
			int j = 0;
			while (frame[i] != '\x12') {
				filename[j++] = frame[i++];
			}
			filename[j] = '\0';
			i++;
		}
		char filenamepath[MAX_PATH];
		sprintf_s(filenamepath, sizeof(filenamepath), "%s\\%s", GetPath(), filename);
		fopen_s(&output, filenamepath, "a+" );
		if (!output)
			return FALSE;

		while (frame[i] != '\0' && i != 1001) {
			fputc(frame[i++], output); 
		}
		fclose(output);
		if (IsUrgent() && HasFramesToSend()) {
			SetState(STATE_SEND_RVI);
			SendingData(STATE_SEND_RVI);
		} else {
			SendingData(STATE_SEND_ACK_DATA);
			SetState(STATE_WAIT_FOR_DATA);
		}
		return TRUE;
	}
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL WaitForAckRvi(LPSTR lpBlock) {
	if (lpBlock[0] == ASCII_ACK) {
		acksReceived++;
		SetRetriesLeft(4);
		SetState(STATE_SEND_ENQ);
		SendingData(STATE_SEND_ENQ);
		return TRUE;
	}
	return FALSE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL WaitForEnq(LPSTR lpBlock) {
	KillSendMoreDataTimer();
	if (lpBlock[0] == ASCII_ENQ) {
		SetState(STATE_SEND_ACK_ENQ);
		SendingData(STATE_SEND_ACK_ENQ);
		return TRUE;
	}
	return FALSE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendingData(int state) {
	switch (state) {
		case STATE_SEND_ACK_ENQ:
			SendAckEnq();
			break;
		case STATE_SEND_ACK_DATA: //checksummed and no errors found
			SendAckData();
			break;
		case STATE_SEND_RVI:
			SendRvi();
			break;
		case STATE_SEND_ENQ:
			SendEnq();
			break;
		case STATE_SEND_DATA:
			SendData();
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendAckEnq() {
	SetState(STATE_WAIT_FOR_DATA);
	SendByte("\x06", 1);
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendAckData() {
	SetState(STATE_WAIT_FOR_DATA);
	SendByte("\x06", 1);
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendRvi() {
	SetState(STATE_WAIT_FOR_ACK_RVI);
	SendByte("\x11", 1);
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendEnq() {
	SetState(STATE_WAIT_FOR_ACK_ENQ);
	SendByte("\05", 1);
	SetWaitForAckEnqTimer();
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
BOOL SendData() {
	SetState(STATE_WAIT_FOR_ACK_DATA);
	SendByte(GetFrame(), NUMBER_OF_BYTES_IN_FRAME);
	framesSent++;
	return TRUE;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
void SetRetriesLeft(int retries) {
	retriesLeft = retries;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
int GetRetriesLeft() {
	return retriesLeft;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
int GetAcks() {
	return acksReceived;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
int GetNaks() {
	return naksReceived;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
int GetFramesReceived() {
	return framesReceived;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
int GetFramesSent() {
	return framesSent;
}

//***********************************************************
// Function: BOOL WaitForAckEnq(LPSTR)
//
// Description: Handles the waiting for ACK after receiving
//				an ENQ state.
//***********************************************************
double GetBitErrorRate() {
	return 0;
}
