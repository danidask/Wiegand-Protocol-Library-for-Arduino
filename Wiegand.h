#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class WIEGAND {

public:
	WIEGAND();
	void begin();
	void begin(int pinD0, int pinIntD0, int pinD1, int pinIntD1);
	bool available();
	unsigned long getCode();
	// char* getCodigoBinario();
	char* getHexCode();
	int getCardReaderId();


private:
	static void ReadD0();
	static void ReadD1();
	static void BitReceive(bool bitrec);
	static bool DoWiegandConversion ();
	static void extracthex ();

	static volatile unsigned long 	_lastWiegand;
	static volatile int				_bitCount;
	static volatile int				_bitinbyte;
	static volatile int				_byteindex;
	static volatile int				_readerid;
	// static char _bites[200]; // BORRAR
	static unsigned char _bytes[30];
	static char _hexmsg[60];
};

#endif
