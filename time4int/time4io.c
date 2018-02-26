#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(){
	int switches = (PORTD>>8) & (0xF);
	return switches;
}

int getbtns(){
	int buttons = (PORTD>>5) & (0x7);
	return buttons;
}