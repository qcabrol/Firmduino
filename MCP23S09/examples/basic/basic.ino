#include <mcp23s09.h>   // import library

//define CS pin and using HAEN or not
//to use HAEN, address should be 0x20 to 0x23
mcp23s09 mcp(10);//instance with CS on pin 10

void setup(){
  mcp.begin();//x.begin(true) will override automatic SPI initialization
  mcp.portMode(OUTPUT);
}

void loop(){
  for (int i = 0; i < 8; i++) {
    mcp.pinWrite(i, HIGH);
    delay(10);
  }

  for (int i = 0; i < 8; i++) {
    mcp.pinWrite(i, LOW);
    delay(10);
  } 
}
