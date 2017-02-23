/*
This example use an MCP23s09 and a standard keypad 4x4, this time
 I will use the MCU interrupt to speedup the key scan. 
 This example uses the ability of the GPIO's chip to change I/O directions on the fly to identify first the row
 and by inverting I/O the col, without use any cicle For..Next. It's also identify 
 when key has been released.
 
 */


#include <SPI.h>
#include <mcp23s09.h>

#define CS_COMMON_PIN    10


#define INTused 0 
#define INTpin 2 

volatile boolean keyPressed;

mcp23s09 mkeypad(CS_COMMON_PIN);

/* setup
 7  6  5  4  3  2  1  0
 c  c  c  c  r  r  r  r
 */

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(38400);
  mkeypad.begin();
  mkeypad.regWrite(mkeypad.IOCON,0b00101000);//chip configure,serial, HAEN, etc.
  mkeypad.regWrite(mkeypad.GPPU,0b11110000);//pull up resistors on for col and off for row
  mkeypad.regWrite(mkeypad.IODIR,0b00001111);//pin0..3(in) / 4...7(out)
  mkeypad.regWrite(mkeypad.INTCON,0b00001111);//int on change control on pin 0..3
  mkeypad.regWrite(mkeypad.DEFVAL,0b00001111);//default comparison value for pin 0..3
  mkeypad.regWrite(mkeypad.GPINTEN,0b00001111);//int on change enable on pin 0..3

  mkeypad.regRead(mkeypad.INTCAP);//clear int register
  
  pinMode(INTpin, INPUT);
  digitalWrite(INTpin, HIGH);
  attachInterrupt(INTused, keypress, FALLING);
  keyPressed = false;
}

volatile byte pressed = 0;


void loop() {

  pressed = handleKeypress();
  if (pressed != 0){
    Serial.println(pressed,HEX);
  }

}

void keypress ()
{
  keyPressed = true;
}

// following function identify pressed button
//following the scheme below or 0 if nothing
//pressed.....
// Keyboard Map
//  1   2   3   A
//  4   5   6   B
//  7   8   9   C
//  *   0   #   D
// Key Code returned
// 77  7B  7D  7E
// B7  BB  DB  BE
// D7  DB  DD  DE
// E7  EB  ED  EE
byte handleKeypress(){
  byte res = 0;
  if (keyPressed){
    byte col;
    byte row;
    detachInterrupt(INTused);//protect from further interrupts 
    delay(30);//a kinda debounce
    //start keyscan
    //read INTCAP register and store the value, this contain the row
    row = mkeypad.regRead(mkeypad.INTCAP);
    // ...now identify the column
    //invert the direction of the pins IODIR
    mkeypad.regWrite(mkeypad.IODIR,0b11110000);//pin0..3(out) / 4...7(in)
    //invert the other registers
    mkeypad.regWrite(mkeypad.INTCON,0b11110000);//int on change control on pin 4..7
    mkeypad.regWrite(mkeypad.DEFVAL,0b11110000);//default comparison value for pin 4..7
    mkeypad.regWrite(mkeypad.GPINTEN,0b11110000);//int on change enable on pin 4..7
    //now read GPIO to get the column
    col = mkeypad.regRead(mkeypad.GPIO);
    // combine in OR the 2 register
    res = row|col;
    if (res == 0xF0) res = 0;
    // not ended, key it's still down and we have to put back everithing as it was
    mkeypad.regWrite(mkeypad.IODIR,0b00001111);//pin0..3(in) / 4...7(out)
    mkeypad.regWrite(mkeypad.INTCON,0b00001111);//int on change control on pin 0..3
    mkeypad.regWrite(mkeypad.DEFVAL,0b00001111);//default comparison value for pin 0..3
    // Cannot put back the int register since key it's still down!
    // so wait until key has been released
    while(mkeypad.regRead(mkeypad.GPIO) != 0x0F){
      delayMicroseconds(15);
    }
    //put back the interrupt register since now key has been released
    mkeypad.regWrite(mkeypad.GPINTEN,0b00001111);//int on change enable on pin 0..3
    keyPressed = false;
    attachInterrupt(INTused, keypress, FALLING);
  }
  return res;
}


