#include "Wiegand.h"

volatile unsigned long WIEGAND::_lastWiegand=0;
volatile int WIEGAND::_bitCount=0;
volatile int WIEGAND::_bitinbyte = 0;
volatile int WIEGAND::_byteindex=0;
volatile int WIEGAND::_readerid=0;
char WIEGAND::_hexmsg[];
unsigned char WIEGAND::_bytes[];
WIEGAND::WIEGAND()
{
}


char* WIEGAND::getHexCode()
{
	return _hexmsg;
}

int WIEGAND::getCardReaderId()
{
	return _readerid;
}

bool WIEGAND::available()
{
	bool ret;
  noInterrupts();
	ret = DoWiegandConversion();
	interrupts();
	return ret;
}

void WIEGAND::begin()
{
#ifdef digitalPinToInterrupt
  // newer versions of Arduino provide pin to interrupt mapping
  begin(2,digitalPinToInterrupt(2),3,digitalPinToInterrupt(3));
#else
  begin(2,0,3,1);
#endif
}

void WIEGAND::begin(int pinD0, int pinIntD0, int pinD1, int pinIntD1)
{
	_lastWiegand = 0;
	_bitCount = 0;
	_byteindex = 0;
	_bitinbyte = 0;
	pinMode(pinD0, INPUT);					// Set D0 pin as input
	pinMode(pinD1, INPUT);					// Set D1 pin as input
	attachInterrupt(pinIntD0, ReadD0, FALLING);	// Hardware interrupt - high to low pulse
	attachInterrupt(pinIntD1, ReadD1, FALLING);	// Hardware interrupt - high to low pulse
}

// TODO medir tiempo que tarda con funcion BitReceive y con codigo aquí
void WIEGAND::ReadD0 (){
	BitReceive(0);
}

void WIEGAND::ReadD1(){
	BitReceive(1);
}

void WIEGAND::BitReceive(bool bitrec){
	// _bites[_bitCount] = char(bitrec + 48); // pasa a ascii y mete // BORRAR
	_bytes[_byteindex] <<= 1; //desplaza bits una posicion metiendo cero
	_bytes[_byteindex] += bitrec; // si es 1, lo suma
	_bitCount++;				// incremente contador de bits recibidos
	_bitinbyte++;		// la posicion que ocupa este bit dentro del byte
	if (_bitinbyte > 7){
		_bitinbyte = 0;
		_byteindex++;
		_bytes[_byteindex] = 0; //pone el siguiente byte a cero para intoducir
	}
	_lastWiegand = millis();	// Keep track of last wiegand bit received
}

bool WIEGAND::DoWiegandConversion () {
	unsigned long sysTick = millis();
	// if no more signal coming through after 25ms
	if ((sysTick - _lastWiegand) > 25 && _bitCount > 0) {
		// _bites[_bitCount] = 0; // BORRAR añade EOL al final
		bool codeok = false;
		if (_bitCount > 50){ //Minimo n de bits recibidos para considerarlo valido
			extracthex();
			codeok = true;
		}
		_bitCount = 0;
		_byteindex = 0;
		_bitinbyte = 0;
		return codeok;
	}
	else
		return false;
}

void WIEGAND::extracthex(){
	// TODO hay formas mas eficientes de hacer esto, pero funciona
	_readerid = _bytes[0]; // El primer byte es el ID del lector
	_hexmsg[0] = 0;
	// Iterar de fin a principio (saltando el primero)
	for(int i = _byteindex-1; i > 0; i--){
		sprintf (_hexmsg, "%s%02X", _hexmsg, _bytes[i]);
	}
}
