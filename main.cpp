#include "mbed.h"
#include "MIDIInput.h"
#include "MIDIMessage.h"
#include <stdio.h>

Serial pc(USBTX, USBRX, NULL, 115200);
RawSerial midiPort(D1, D0, 31250);


MIDIInput myInput(&midiPort);
DigitalOut myled(LED1);
DigitalOut myledD6(D6);
DigitalOut myledD7(D7);

AnalogIn knobA0(A0);
AnalogIn knobA1(A1);


DigitalIn buttonD2(D2);
DigitalIn buttonD3(D3);
DigitalIn buttonD4(D4);



#define TDELAY (0.2)
void showButtons(void) {
	int temp;
	float tempPot;

	temp = buttonD2;
	pc.printf("Button D2 %d\r\n", temp);
	temp = buttonD3;
	pc.printf("Button D3 %d\r\n", temp);
	temp = buttonD4;
	pc.printf("Button D4 %d\r\n", temp);

	tempPot = knobA0;
	pc.printf("KnobA0 = %f\r\n", tempPot);
	tempPot = knobA1;
	pc.printf("KnobA1 = %f\r\n", tempPot);
}


int main() {

	bool LEDState = 0;
	myledD6 = 1;
	myledD7 = LEDState;

    while(1) {
		osEvent evt = myInput.get(100);
		if (evt.status == osEventMail) {
			MIDIMessage *MIDIEvent = (MIDIMessage*)evt.value.p;

			switch (MIDIEvent->type())
			{
				case MIDIMessage::NoteOnType:
					pc.printf("Note on - channel:%d key:%d velocity:%d\r\n",
							MIDIEvent->channel(),
							MIDIEvent->key(),
							MIDIEvent->velocity());
					myledD6 = (MIDIEvent->velocity() == 0);
					break;
					
				case MIDIMessage::NoteOffType:
					pc.printf("Note off - channel:%d key:%d velocity:%d\r\n",
							MIDIEvent->channel(),
							MIDIEvent->key(),
							MIDIEvent->velocity());
					break;
				case MIDIMessage::ControlChangeType:
					pc.printf("Control Change- channel:%d controller:%d value:%d\r\n",
							MIDIEvent->channel(),
							MIDIEvent->controller(),
							MIDIEvent->value());
					break;
				case MIDIMessage::AllNotesOffType:
					pc.printf("ALL notes off - channel:%d\r\n",
							MIDIEvent->channel());
					break;
				case MIDIMessage::PitchWheelType:
					pc.printf("Pitch Wheel - channel:%d pitch:%d\r\n",
							MIDIEvent->channel(),
							MIDIEvent->pitch());
					break;
				default:
					break;
			}
			myInput.free(MIDIEvent);
		}
		LEDState = !LEDState;
		myledD7 = LEDState;
		
	}
}
