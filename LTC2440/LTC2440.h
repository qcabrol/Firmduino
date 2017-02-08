/*********************************************************************************
	LTC2440.h - Library for controlling the 24 bits ADC chip LTC2440 and LTC2410
	on ATMEL AVR Mcus
	Created by Quentin Cabrol, February 05, 2017
*********************************************************************************/

#ifndef LTC2440_h
#define LTC2440_h
/*************************************************************************
	      Modes of operation detailed in the datasheet
**************************************************************************/
#define LTC_DEFAULT_MODE   0x80 //External Input, 50Hz and 60Hz Rejection, Autocalibration
#define LTC_NO60_MODE      0x82 //External Input, 50Hz Rejection, Autocalibration
#define LTC_NO50_MODE      0x84 //External Input, 60Hz Rejection, Autocalibration
#define LTC_2X_MODE        0x81 //External Input, 50Hz and 60Hz Rejection, 2x Speed
#define LTC_2X_NO60_MODE   0x83 //External Input, 50Hz Rejection, 2x Speed
#define LTC_2X_NO50_MODE   0x85 //External Input, 60Hz Rejection, 2x Speed
#define LTC_TEMP_MODE      0x88 //Temperature Input, 50Hz and 60Hz Rejection, Autocalibration
#define LTC_TEMP_NO60_MODE 0x8A //Temperature Input, 50Hz Rejection, Autocalibration
#define LTC_TEMP_NO50_MODE 0x8C //Temperature Input, 60Hz Rejection, Autocalibration
const uint8_t *ltcAdcModes[] = {
	LTC_DEFAULT_MODE,
	LTC_NO60_MODE, 
	LTC_NO50_MODE, 
	LTC_2X_MODE, 
	LTC_2X_NO60_MODE, 
	LTC_2X_NO50_MODE, 
	LTC_TEMP_MODE, 
	LTC_TEMP_NO60_MODE, 
	LTC_TEMP_NO50_MODE 
};
#define NUM_LTC_MODES sizeof(ltcAdcModes) / sizeof(ltcAdcModes[0]);

class LTCadc
{
	public:
		LTCadc(char port, byte pin, char busyPort = NULL , byte busyPin = NULL, char clockPort = NULL, byte clockPin = NULL);
		void setMode(string mode);   //'single' (default) or 'stream'
		void setClock(string clock); //'ext' (default) or 'ext', only if clockPort and clockPin are defined
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
