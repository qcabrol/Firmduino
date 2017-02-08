#include "Arduino.h"
#include "LTC2440.h"
#include "SPI.h"
//takes as parameters the port letter as a string (eg. 'A') 
//and pin number of the chip select pin
//the busy pin MUST BE on an interruptible pin of the AVR MCU
LTCadc::LTCadc(char port, byte pin, char busyPort = NULL , byte busyPin = NULL, char clockPort = NULL, byte clockPin = NULL)
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
	if(busyPort!=NULL){
		switch (busyPort) {
	
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
	}
	if(clockPort!=NULL){
		switch (clockPort) {
				#ifdef PORTA
				case 'A':
					ltcClockPort= &PORTA;
					break;
				#endif
				#ifdef PORTB
				case 'B':
					ltcClockPort= &PORTB;
					break;
				#endif
				#ifdef PORTC
				case 'C':
					ltcClockPort= &PORTC;
					break;
				#endif
				#ifdef PORTD
				case 'D':
					ltcClockPort= &PORTD;
					break;
				#endif
				#ifdef PORTE
				case 'E':
					ltcClockPort= &PORTE;
					break;
				#endif
				#ifdef PORTF
				case 'F':
					ltcClockPort= &PORTF;
					break;
				#endif
				#ifdef PORTG
				case 'G':
					ltcClockPort= &PORTG;
					break;
				#endif
				#ifdef PORTH
				case 'H':
					ltcClockPort= &PORTH;
					break;
				#endif
				#ifdef PORTJ
				case 'J':
					ltcClockPort= &PORTJ;
					break;
				#endif
				#ifdef PORTK
				case 'K':
					ltcClockPort= &PORTK;
					break;
				#endif
				#ifdef PORTL
				case 'L':
					ltcClockPort= &PORTL;
					break;
				#endif
				default:
					//exception handling not supported
					ltcClockPort= NULL;
					break; 
			}	
			_clockPin=clockPin;
		}
	_ssPin = pin;
	_mode  = LTC_SPI_MODE;
	_speed = LTC_SPEED_5 ;
}

//1 assembler cycle = nop
inline void volatile LTCadc::nop(void) { asm __volatile__ ("nop"); }
void LTCadc::adcSelect()    { SPI.setBitOrder(MSBFIRST); nop(); *ltcPort &=~(_BV(_ssPin)); nop(); }
void LTCadc::adcDeselect()  { nop(); *ltcPort |= _BV(_ssPin); nop(); }

/***************************************************************************
set the operation mode private variable if defined as a possible mode
two modes are possible for the LTC2440 - LTC2410 : streaming and single
'single' outputs 32bits of data on Chip Selection
'stream' outputs data continuously with a busyPin LOW trigger at each EOC
****************************************************************************/
void LTCadc::setMode(byte mode)
{	 
  switch(mode){
  case(LTC_SPI_MODE);
	 setClock(LTC_EXT_CLK);
	_mode=LTC_SPI_MODE;
	break;
  case(LTC_2WIRE_MODE);
    setClock(LTC_EXT_CLK);
	_mode=LTC_2WIRE_MODE;
	break;
  case(LTC_SINGLE_MODE);
    if(ltcClockPort!=NULL && _clockPin !=NULL){
		setClock(LTC_INT_CLK);
		_mode=LTC_SINGLE_MODE;
	}
	else{
		 setClock(LTC_EXT_CLK);
		_mode=LTC_SPI_MODE;
	}
	break;
  case(LTC_STREAM_MODE);
    if(ltcClockPort!=NULL && _clockPin !=NULL){
		setClock(LTC_INT_CLK);
		_mode=LTC_STREAM_MODE;
	}
	else{
		 setClock(LTC_EXT_CLK);
		_mode=LTC_SPI_MODE;
	}
	break;
  default:
	_mode=LTC_SPI_MODE;
	break;
  }
}

/*****************************************************************************
set the clock mode if defined as a possible mode
two modes are possible for the LTC2440 - LTC2410 : internal or external clock
****************************************************************************/
void LTCadc::setClock(bool clock)
{
	if(ltcClockPort!=NULL && _clockPin!=NULL){
	  switch(clock){
		  case(LTC_EXT_CLK);
			*ltcClockPort &=~(_BV(_clockPin));
			break;
		  case(LTC_INT_CLK);
			*ltcClockPort |= _BV(_clockPin);
			break;
		  default:
			*ltcClockPort &=~(_BV(_clockPin));
			break;
		}
  }
}

/****************************************************************************
set the operation speed private variable if defined as a possible speed
several speeds are possible for the LTC2440 - LTC2410 
*****************************************************************************/
void LTCadc::setSpeed(byte speed)
{
  bool speedExists=false;
  for(byte i=0; i<NUM_LTC_MODES; i++){
	if(ltcAdcModes[i] == speed){_speed=speed; speedExists= true;}
  }
  if(!speedExists) _speed= LTC_SPEED_5 ;
}

/****************************************************************************

*****************************************************************************/
uint32_t LTCadc::adcRead() {
	switch(_mode){
		case(LTC_SPI_MODE);
		break;
	  case(LTC_2WIRE_MODE);
		break;
	  case(LTC_SINGLE_MODE);
		break;
	  case(LTC_STREAM_MODE);
		break;
	  default:
		break;
	}

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

/****************************************************************************

*****************************************************************************/
uint16_t LTCadc::adcRead16(){
	uint32_t reading = adcRead24();
	return (uint16_t) (0x00FF &(result >> 8));
}

/****************************************************************************

*****************************************************************************/
uint16_t LTCadc::adcReadLSBs(){
	uint32_t reading = adcRead24();
	return (uint16_t) (0x00FF & result);
}



