#include <stdio.h>
#include <inttypes.h>
#include <Arduino.h>

#include "mcp23s09.h"
#include "SPI/SPI.h"

mcp23s09::mcp23s09(){
#if defined (SPI_HAS_TRANSACTION)
	_spiTransactionsSpeed = MAXSPISPEED;//set to max supported speed (in relation to chip and CPU)
#else
	_spiTransactionsSpeed = 0;
#endif
}

void mcp23s09::setSPIspeed(uint32_t spispeed){
	#if defined (SPI_HAS_TRANSACTION)
	if (spispeed > 0){
		if (spispeed > MAXSPISPEED) {
			_spiTransactionsSpeed = MAXSPISPEED;
		} else {
			_spiTransactionsSpeed = spispeed;
		}
	} else {
		_spiTransactionsSpeed = 0;//disable SPItransactions
	}
	#else
	_spiTransactionsSpeed = 0;
	#endif
}


int mcp23s09::getInterruptNumber(byte pin) {
	int intNum = digitalPinToInterrupt(pin);
	if (intNum != NOT_AN_INTERRUPT) {
		#if defined (SPI_HAS_TRANSACTION)
			SPI.usingInterrupt(intNum);
		#endif
		return intNum;
	}
	return 255;
}

mcp23s09::mcp23s09(const uint8_t csPin){
	_spiTransactionsSpeed = 0;
	postSetup(csPin);
}

mcp23s09::mcp23s09(const uint8_t csPin,uint32_t spispeed){
	postSetup(csPin,spispeed);
}


void mcp23s09::postSetup(const uint8_t csPin,uint32_t spispeed){
	#if defined (SPI_HAS_TRANSACTION)
		if (spispeed > 0) setSPIspeed(spispeed);
	#endif
	_cs = csPin;
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
	//setup register values for this chip
	IOCON = 	0x05;
	IODIR = 	0x00;
	GPPU = 		0x06;
	GPIO = 		0x09;
	GPINTEN = 	0x02;
	IPOL = 		0x01;
	DEFVAL = 	0x03;
	INTF = 		0x07;
	INTCAP = 	0x08;
	OLAT = 		0x0A;
	INTCON = 	0x04;
}

void mcp23s09::begin(bool protocolInitOverride) {
	if (!protocolInitOverride){
		SPI.begin();
		#if defined (SPI_HAS_TRANSACTION)
		if (_spiTransactionsSpeed == 0){//do not use SPItransactons
			SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE0);
		}
		#else//do not use SPItransactons
		SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
		#endif
	}	
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	delay(100);
	_gpioDirection = 0xFF;//all in
	_gpioState = 0x00;//all low 
}





uint8_t mcp23s09::readAddress(byte addr){
	byte low_byte = 0x00;
	startSend(1);
	SPI.transfer(addr);
	low_byte  = SPI.transfer(0x0);
	endSend();
	return low_byte;
}



void mcp23s09::gpioPinMode(uint8_t mode){
	if (mode == INPUT){
		_gpioDirection = 0xFF;
	} else if (mode == OUTPUT){	
		_gpioDirection = 0x00;
		_gpioState = 0x00;
	} else {
		_gpioDirection = mode;
	}
	writeByte(IODIR,_gpioDirection);
}

void mcp23s09::gpioPinMode(uint8_t pin, bool mode){
	if (pin < 8){//0...7
		mode == INPUT ? _gpioDirection |= (1 << pin) :_gpioDirection &= ~(1 << pin);
		writeByte(IODIR,_gpioDirection);
	}
}

void mcp23s09::gpioPort(uint8_t value){
	if (value == HIGH){
		_gpioState = 0xFF;
	} else if (value == LOW){	
		_gpioState = 0x00;
	} else {
		_gpioState = value;
	}
	writeByte(GPIO,_gpioState);
}


uint8_t mcp23s09::readGpioPort(){
	return readAddress(GPIO);
}

uint8_t mcp23s09::readGpioPortFast(){
	return _gpioState;
}

int mcp23s09::gpioDigitalReadFast(uint8_t pin){
	if (pin < 8){//0...7
		int temp = bitRead(_gpioState,pin);
		return temp;
	} else {
		return 0;
	}
}

void mcp23s09::portPullup(uint8_t data) {
	if (data == HIGH){
		_gpioState = 0xFF;
	} else if (data == LOW){	
		_gpioState = 0x00;
	} else {
		_gpioState = data;
	}
	writeByte(GPPU, _gpioState);
}




void mcp23s09::gpioDigitalWrite(uint8_t pin, bool value){
	if (pin < 8){//0...7
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
		writeByte(GPIO,_gpioState);
	}
}

void mcp23s09::gpioDigitalWriteFast(uint8_t pin, bool value){
	if (pin < 8){//0...8
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
	}
}

void mcp23s09::gpioPortUpdate(){
	writeByte(GPIO,_gpioState);
}

int mcp23s09::gpioDigitalRead(uint8_t pin){
	if (pin < 8) return (int)(readAddress(GPIO) & 1 << pin);
	return 0;
}

uint8_t mcp23s09::gpioRegisterReadByte(byte reg){
  uint8_t data = 0;
    startSend(1);
    SPI.transfer(reg);
    data = SPI.transfer(0);
    endSend();
  return data;
}


void mcp23s09::gpioRegisterWriteByte(byte reg,byte data){
	writeByte(reg,(byte)data);
}

/* ------------------------------ Low Level ----------------*/
void mcp23s09::startSend(bool mode){
#if defined (SPI_HAS_TRANSACTION)
	if (_spiTransactionsSpeed > 0) SPI.beginTransaction(SPISettings(_spiTransactionsSpeed, MSBFIRST, SPI_MODE0));
#endif
#if defined(__FASTWRITE)
	digitalWriteFast(_cs, LOW);
#else
	digitalWrite(_cs, LOW);
#endif
	mode == 1 ? SPI.transfer(_readCmd) : SPI.transfer(_writeCmd);
}

void mcp23s09::endSend(){
#if defined(__FASTWRITE)
	digitalWriteFast(_cs, HIGH);
#else
	digitalWrite(_cs, HIGH);
#endif
#if defined (SPI_HAS_TRANSACTION)
	if (_spiTransactionsSpeed > 0) SPI.endTransaction();
#endif
}


void mcp23s09::writeByte(byte addr, byte data){
	startSend(0);
	SPI.transfer(addr);
	SPI.transfer(data);
	endSend();
}


