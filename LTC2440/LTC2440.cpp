#include "Arduino.h"
#include "LTC2440.h"
#include "SPI.h"
//takes as parameters the port letter as a string (eg. 'A') 
//and pin number of the chip select pin
LTCadc::LTCadc(char port, it,n pin, char busyPort, byte busyPin)
{
	pinMode(pin, OUTPUT);
	switch (port) {
		#ifdef PORTA
		case 'A':
			ltcPort= &PORTA;
			break;
		#endif
		#ifdef PORTB
		case 'B':
			ltcPort= &PORTB;
			break;
		#endif
		#ifdef PORTC
		case 'C':
			ltcPort= &PORTC;
			break;
		#endif
		#ifdef PORTD
		case 'D':
			ltcPort= &PORTD;
			break;
		#endif
		#ifdef PORTE
		case 'E':
			ltcPort= &PORTE;
			break;
		#endif
		#ifdef PORTF
		case 'F':
			ltcPort= &PORTF;
			break;
		#endif
		#ifdef PORTG
		case 'G':
			ltcPort= &PORTG;
			break;
		#endif
		#ifdef PORTH
		case 'H':
			ltcPort= &PORTH;
			break;
		#endif
		#ifdef PORTJ
		case 'J':
			ltcPort= &PORTJ;
			break;
		#endif
		#ifdef PORTK
		case 'K':
			ltcPort= &PORTK;
			break;
		#endif
		#ifdef PORTL
		case 'L':
			ltcPort= &PORTL;
			break;
		#endif
		default:
			//exception handling not supported
			ltcPort= NULL;
			break; 
	}
	
	switch (port) {
		#ifdef PORTA
		case 'A':
			ltcBusyPort= &PORTA;
			break;
		#endif
		#ifdef PORTB
		case 'B':
			ltcBusyPort= &PORTB;
			break;
		#endif
		#ifdef PORTC
		case 'C':
			ltcBusyPort= &PORTC;
			break;
		#endif
		#ifdef PORTD
		case 'D':
			ltcBusyPort= &PORTD;
			break;
		#endif
		#ifdef PORTE
		case 'E':
			ltcBusyPort= &PORTE;
			break;
		#endif
		#ifdef PORTF
		case 'F':
			ltcBusyPort= &PORTF;
			break;
		#endif
		#ifdef PORTG
		case 'G':
			ltcBusyPort= &PORTG;
			break;
		#endif
		#ifdef PORTH
		case 'H':
			ltcBusyPort= &PORTH;
			break;
		#endif
		#ifdef PORTJ
		case 'J':
			ltcBusyPort= &PORTJ;
			break;
		#endif
		#ifdef PORTK
		case 'K':
			ltcBusyPort= &PORTK;
			break;
		#endif
		#ifdef PORTL
		case 'L':
			ltcBusyPort= &PORTL;
			break;
		#endif
		default:
			//exception handling not supported
			ltcBusyPort= NULL;
			break; 
	}
	_busyPin = busyPin;
	_ssPin = pin;
	_mode  = 'single';
	_config = LTC_DEFAULT_MODE;
}

//1 assembler cycle = nop
inline void volatile LTCadc::nop(void) { asm __volatile__ ("nop"); }
void LTCadc::adcSelect()    { SPI.setBitOrder(MSBFIRST); nop(); *ltcPort &=~(_BV(_ssPin)); nop(); }
void LTCadc::adcDeselect()   { nop(); *ltcPort |= _BV(_ssPin); nop(); }

//set the operation mode private variable if defined as a possible mode
//two modes are possible for the LTC2440 - LTC2410 : streaming and single
//'single' outputs 32bits of data on Chip Selection
//'stream' outputs data continuously with a busyPin LOW trigger at each EOC
void LTCadc::setMode(string mode)
{
  bool modeExists=false;
  switch(mode){
  case('single');
	_mode='single';
	break;
  case('stream');
	_mode='stream';
	break;
  default:
	_mode='single';
	break;
  }

}

void LTCadc::setConfig(uint16_t config)
{
  bool configExists=false;
  for(byte i=0; i<NUM_LTC_MODES; i++){
	if(ltcModesArray[i] == config){_config=config; configExists= true;}
  }
  if(!configExists) _config=LTC_DEFAULT_MODE;
}


uint32_t LTCadc::adcRead() {
	uint32_t result = 0;
	// MSB
	result = SPI.transfer(_mode);
	result = result << 8;
    //Central Byte
	result = result + SPI.transfer(0);
	result = result << 8;
    //LSB
	result |= result + SPI.transfer(0);
	result = result << 8;
	//SUB LSBs, not relevant
	SPI.transfer(0);
	return result;
}

uint16_t LTCadc::adcRead16(){
	uint32_t reading = adcRead24();
	return (uint16_t) (0x00FF &(result >> 8));
}

uint16_t LTCadc::adcReadLSBs(){
	uint32_t reading = adcRead24();
	return (uint16_t) (0x00FF & result);
}



