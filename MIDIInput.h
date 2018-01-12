#include "mbed.h"
#include "MIDIMessage.h"

#define MIDI_MSG_SIZE 4

class MIDIInput : public Mail<MIDIMessage, 16>
{
public:
	MIDIInput(RawSerial* MIDIPort);

protected:
	void gatherMessage(uint8_t byte);
	int GetMsgSize(uint8_t statusByte);

	void _handleMIDISerial(void);
	//
	// context for _handleMIDISerial()
	RawSerial* _MIDIPort;
	uint8_t newMsgBuf[MIDI_MSG_SIZE];	// buffer for new incoming message
	int newMsgIndex;					// Current Index into new message buffer

	int bytesRemaining;					// remaining number of bytes expected in current message
	bool dumpTillSentinal;				// Certain SysEx messages are teminated by a byte of 0xF7
	bool valid;							// mark bad packets invalid so they don't propagate
	uint8_t runningStatus;				// keep track of the latest status byte for "running status" mechanism
};

