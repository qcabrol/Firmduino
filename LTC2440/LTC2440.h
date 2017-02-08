/*********************************************************************************
	LTC2440.h - Library for controlling the 24 bits ADC chip LTC2440 and LTC2410
	on ATMEL AVR Mcus
	Created by Quentin Cabrol, February 05, 2017
*********************************************************************************/

#ifndef LTC2440_h
#define LTC2440_h
/*************************************************************************
	    Modes of operation and speeds detailed in the datasheet p14
**************************************************************************/
#define LTC_SPI_MODE	0 
#define LTC_2WIRE_MODE  1
#define LTC_SINGLE      2
#define LTC_STREAM_MODE 3

#define LTC_HIGH_SPEED     1  //23µVRMS,  17ENOB,   OSR64
#define LTC_SPEED_7        2  //3.5µVRMS, 20ENOB,   OSR128
#define LTC_SDI_LOW        0  //2µVRMS,   21.3ENOB, OSR256, allow tying SDI LOW
#define LTC_SPEED_6        3  //2µVRMS,   21.3ENOB, OSR256
#define LTC_SPEED_5        4  //1.4µVRMS, 21.8ENOB, OSR512,
#define LTC_SPEED_4        5  //1µVRMS,   22.4ENOB, OSR1024
#define LTC_SPEED_3        6  //750nVRMS, 22.9ENOB, OSR2048
#define LTC_SPEED_2        7  //510nVRMS, 23.4ENOB, OSR4096
#define LTC_SPEED_1        8  //375nVRMS, 24ENOB,   OSR8192
#define LTC_HIGH_RES1      7  //250nVRMS, 24.4ENOB, OSR16384
#define LTC_HIGH_RES2      15 //200nVRMS, 24.6ENOB, OSR32768, allow tying SDI HIGH

const uint8_t *ltcAdcModes[] = {
	LTC_HIGH_SPEED,  
	LTC_SPEED_7,      
	LTC_SDI_LOW,      
	LTC_SPEED_6,      
	LTC_SPEED_5,      
	LTC_SPEED_4,  
	LTC_SPEED_3,      
	LTC_SPEED_2,      
	LTC_SPEED_1,        
	LTC_HIGH_RES1,     
	LTC_HIGH_RES2,      
};
#define NUM_LTC_MODES sizeof(ltcAdcModes) / sizeof(ltcAdcModes[0]);

class LTCadc
{
	public:
		LTCadc(char port, byte pin, char busyPort = NULL , byte busyPin = NULL, char clockPort = NULL, byte clockPin = NULL);
		void setMode(string mode);   //'spiMode' (default) with external clock and single conversion), 'twoWire', 'stream', 'singleInternal'
		void setClock(string clock); //'ext' (default) or 'int', only if clockPort and clockPin are defined
		void setConfig(byte config);   
		uint32_t adcRead();     //--> full resolution reading without SUB LSBs
		uint16_t adcRead16();   //--> truncation of 8LSbits
		uint16_t adcReadLSBs(); //--> only 16Least significant
	    void adcEnable();		//--> enable chip select pin and MSB_FIRST mode
		void adcDisable();
	private:
		volatile uint16_t *ltcPort;
		volatile uint16_t *ltcBusyPort;
	    volatile uint16_t *ltcClockPort;
		byte	 _clockPin
		byte	 _busyPin;
		byte     _ssPin;
		byte     _mode;
		byte     _config;
		inline void volatile nop(void);
};
#endif
