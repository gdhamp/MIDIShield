#include "mbed.h"
#include "MIDIInput.h"



MIDIInput::MIDIInput(RawSerial* MIDIPort) : _MIDIPort(MIDIPort),
											newMsgIndex(0),
											dumpTillSentinal(false),
											runningStatus(0x0F)
{
	_MIDIPort->attach(callback(this, &MIDIInput::_handleMIDISerial));
}


int MIDIInput::GetMsgSize(uint8_t statusByte) {
	switch ((statusByte >> 4) & 0x0F) {
		// 2 byte message
		case 0x0C:	// Program Change
		case 0x0D:	// Channel Aftertouch
			return 2;

		// 3 byte message
		case 0x08:	// Note Off
		case 0x09:	// Note On
		case 0x0A:	// Polyphonic Aftertouch
		case 0x0B:	// Control Change
		case 0x0E:	// Pitch Bend
			return 3;

		case 0x0F:
			switch (statusByte & 0x0F) {
				case 0x00:
				case 0x04:
				case 0x05:
				case 0x07:
				case 0x09:
				case 0x0D:
					return 0;

				// 1 byte messages
				case 0x06:
				case 0x08:
				case 0x0A:
				case 0x0B:
				case 0x0C:
				case 0x0E:
				case 0x0F:
					return 1;

				// 2 byte messages
				case 0x01:
				case 0x03:
					return 2;

				// 3 byte messages
				case 0x02:
					return 3;
			}
	}
	return 0;
}
	

void MIDIInput::gatherMessage(uint8_t byte) {
	int msgSize;
		

	// check to see if SysEx that is sentinal terminiate is done;
	if (dumpTillSentinal && (byte == 0xF7)) {
		newMsgIndex = 0;
		dumpTillSentinal = false;
	}
	else if (newMsgIndex == 0) {
		// start of new message check to see if its a different "status byte"
		// or it uses the "running status" mechanism to send new info for the
		// same status type
		if (byte & 0x80) {
			// if it's a status, stuff it in the buffer
			runningStatus = byte;
			newMsgBuf[newMsgIndex++] = byte;
			msgSize = GetMsgSize(byte);
			// if it's a SysEx of 0xF0, this is the variable length
			// message that is terminated by a sentinal
			dumpTillSentinal = (byte == 0xF0);

			// msgSize returns 0 for undefined packets
			valid = (msgSize != 0);
			if (valid) {
				bytesRemaining = msgSize - 1;
			}
			else {
				bytesRemaining = 0;
			}
		}
		else {
			newMsgBuf[newMsgIndex++] = runningStatus;
			newMsgBuf[newMsgIndex++] = byte;
			bytesRemaining = GetMsgSize(runningStatus) - 2;
		}
	}
	else {
		// just put the rest of the bytes in the buffer
		newMsgBuf[newMsgIndex++] = byte;
		--bytesRemaining;
	}

	if (bytesRemaining == 0) {
		if (valid) {
			MIDIMessage *_MIDIMessageMail = alloc();
			
			if (_MIDIMessageMail) {
				_MIDIMessageMail->FillFromBuff(newMsgBuf);
				put(_MIDIMessageMail);

				newMsgIndex = 0;
			}
		}
	}
}

void MIDIInput::_handleMIDISerial(void) {
	uint8_t byte;
	byte = _MIDIPort->getc();
	gatherMessage(byte);
}

