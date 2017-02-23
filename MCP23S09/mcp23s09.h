/*********************************************************************************
	MCP23S09.h - Library for controlling the GPIO MCP23S09
	on ATMEL AVR Mcus
	Adapted by Quentin Cabrol, February 23, 2017
	From Sumoboy Repo https://github.com/sumotoy/gpio_expander
*********************************************************************************/
// Credentials:
/*
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/ 
										  
	gpio_expander - An attempt to create a fast and universal library for drive many GPIO chips
	
model:			company:		pins:		protocol:		Special Features:
---------------------------------------------------------------------------------------------------------------------
mcp23s09		Microchip		  8			SPI					INT/OPEN DRAIN
---------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------
		Copyright (c) 2013-2014, s.u.m.o.t.o.y [sumotoy(at)gmail.com]
---------------------------------------------------------------------------------------------------------------------

    gpio_expander Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gpio_expander Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

/* ------------------------------ MCP23S09 WIRING ------------------------------------
	MCP23S09 Does not support HAEN

				__ __
		+++   [|  U  |] GND
		NC    [|     |] NC
	 ->  cs   [|     |] IO-7
	 ->  sck  [|     |] IO-6
	 ->  mosi [|     |] IO-5
	 <-  miso [|     |] IO-4
		rst   [|     |] IO-3
		int   [|     |] IO-2
		IO-0  [|_____|] IO-1
*/
#ifndef _MCP23S09_H_
#define _MCP23S09_H_

#include <inttypes.h>
#include <SPI.h>//this chip needs SPI
#include "gpio_expander.h"
#include "_utility/SPI.parameters.h"

class mcp23s09 : public gpio_expander
{

public:
	mcp23s09(const uint8_t csPin);
	mcp23s09(const uint8_t csPin,uint32_t spispeed);//for SPI transactions
	mcp23s09();
	void			postSetup(const uint8_t csPin,uint32_t spispeed=0);//used with other libraries only
	virtual void 	begin(bool protocolInitOverride=false); //protocolInitOverride=true	will not init the SPI	

	void 			portMode(uint8_t mode);					//set all pins to INPUT or OUTPUT
	void 			portWrite(uint8_t value);				//write data to all pins
	uint8_t 		portRead();								//read the state of the pins (all)
	uint8_t 		portReadFast();							
	
	void 			pinMode(uint8_t pin, bool mode);		//set a unique pin as IN(1) or OUT (0)	
	void 			pinWrite(uint8_t pin, bool value);  //write data to one pin
	int 			pinRead(uint8_t pin);				//read data from one pin
	int 			pinReadFast(uint8_t pin);
	
	uint8_t		 	regRead(byte reg);				    //read a byte from chip register
	void 			regWrite(byte reg,byte data);		//write a chip register
	
	void			portPullup(uint8_t data);			// true=pullup, false=pulldown all pins
	void			portUpdate();
	
	// direct access commands
	uint8_t 		readAddr(byte addr);
	int 			getInterruptNumber(byte pin);
	void			setSPIspeed(uint32_t spispeed);//for SPI transactions
	
	//------------------------- REGISTERS
	byte			IOCON;
	byte			IODIR;
	byte			GPPU;
	byte			GPIO;
	byte			GPINTEN;
	byte			IPOL;
	byte			DEFVAL;
	byte			INTF;
	byte			INTCAP;
	byte			OLAT;
	byte			INTCON;
	
private:
    uint8_t 		_cs;	
	uint32_t		_spiTransactionsSpeed;//for SPI transactions
	uint8_t 		_readCmd;
	uint8_t 		_writeCmd;
	void 			startSend(bool mode);
	void 			endSend();
	uint8_t			_gpioDirection;
	uint8_t			_gpioState;
    void			writeByte(byte addr, byte data);	
};
#endif