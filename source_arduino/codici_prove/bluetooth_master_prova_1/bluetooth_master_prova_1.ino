/*
 in this example arduino is the master, and sends to the PC only the string 
 *....*, note that it was necessary to introduce delays in lpc code!
 because arduino has just a 8-bit CPU at 16 MHz, while LPC has 32-bit at
 96MHz, so 96/16 = 6 times faster!
 */

#include <SoftwareSerial.h>  

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
uint8_t   index = 0;
char buffer1[50];
boolean ex = false;
char c;
uint8_t NumberOfAsterisk = 0;
boolean command = true;


SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600


 // set as master and connect to the slave
  delay(5000);                    // wait 5 sec and then connect
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("SM,1"); 
  delay(100);
  bluetooth.println("C,0006664FE329");
  delay(100);
  bluetooth.println("---"); 
  
  while(bluetooth.available()) {

    bluetooth.read();

  }

  interrupts();
  attachInterrupt(0, isr_blue, CHANGE); 

}


void isr_blue(void)
{
    if (bluetooth.available() && !ex) {
     
      c = (char)bluetooth.read();
      
      
      if (c == '*' && NumberOfAsterisk == 0) {
        NumberOfAsterisk ++;
      }      
      else if (c == '*' && NumberOfAsterisk > 0) {
        NumberOfAsterisk = 0;
        ex = true;      // the string is completed, set exit flag  
      }
      
      
            
       buffer1[index] = c;
       index++;
    }   
}


void loop()
{

  delay(100);


  if(ex) {
    noInterrupts();
    buffer1[index] = 0;
    Serial.println(buffer1);
    index = 0;
    ex = false;
    command = true;
    interrupts();
  }

  if(!ex && command)  // If stuff was typed in the serial monitor
  {
    delay(5000);
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print('g');
    command = false;
  }
  // and loop forever and ever!
}

