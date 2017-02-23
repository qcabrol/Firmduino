/*
An example of direct programming GPIO, normally the library automatically
 init the GPIO chip for you but there's cases where you need to set some parameter differently.
 This example uses a MCP23s09 as key scanner with an interrupt so it will set it as input,
 define interrupt mode, etc.
 Connect 8 push buttons to GPIO ports, commoned to ground.
 */
#include <mcp23s09.h>   // import library

#define MCP_CSPIN  4

/*
Following depend of the processor you are using!!!!
 */
#define INTused 2 
#define INTpin 2 
//end

volatile boolean keyPressed;

mcp23s09 mcp(MCP_CSPIN);

void setup()
{
  Serial.begin(38400);
  Serial.println("start");

  mcp.begin();
  mcp.regWrite(mcp.IOCON,0b00101000);//set interrupt on GPIO and other parameters (see datasheet)
  mcp.portMode(INPUT);// Set all pins to be inputs
  mcp.regWrite(mcp.GPPU,0xFF);// pull-up resistor for switch
  mcp.regWrite(mcp.IPOL,0xFF);// invert polarity
  mcp.regWrite(mcp.GPINTEN,0xFF);// enable all interrupt
  mcp.regRead(mcp.INTCAP);// read from interrupt capture ports to clear them
  //now prepare interrupt pin on processor
  pinMode (INTpin, INPUT);
  digitalWrite (INTpin, HIGH);
  keyPressed = false;
  attachInterrupt(INTused, keypress, FALLING);
}

void loop ()
{
  if (keyPressed) handleKeypress();
}  // end of loop

void keypress ()
{
  keyPressed = true;
}

void handleKeypress ()
{
  detachInterrupt(INTused);//protect from further interrupts 
  uint8_t keyValue1 = 0;
  delay (30);  // de-bounce before we re-enable interrupts
  if (mcp.regRead(mcp.INTF)){
    keyValue1 |= mcp.regRead(mcp.INTCAP);
    for (byte button = 0; button < 8; button++)
    {
      // this key down?
      if (keyValue1 & (1 << button))
      {
        Serial.print ("Button ");
        Serial.print (button + 1, DEC);
        Serial.println (" now down");
      }  // end of if this bit changed
    }
  }
  keyPressed = false;
  attachInterrupt(INTused, keypress, FALLING);
} 
