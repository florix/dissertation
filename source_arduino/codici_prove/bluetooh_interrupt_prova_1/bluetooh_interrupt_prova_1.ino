/* prova arduino bluetooth con interrupt, con questo skecth posso leggere 
al massimo 4 caratteri alla volta, cioè il trasmettitore non deve
mandare più di 4 caratteri alla volta!*/
#include <SoftwareSerial.h>

int  blueTX = 2;
int  blueRX= 3;


SoftwareSerial  blue(blueTX, blueRX); // RX, TX their'are inverted as usual in USART

void setup()
{
  Serial.begin(9600);
  blue.begin(115200);
  blue.print("$");
  blue.print("$");
  blue.print("$");
  delay(100);
  blue.println("U, 9600, N");
  blue.begin(9600);
  interrupts();
  attachInterrupt(0, isr_blue, CHANGE);    
}


void loop()
{
 if(Serial.available()) {
    blue.print((char)Serial.read());
  }
}


void isr_blue(void)
{
  noInterrupts();
  if (blue.available()) {
  Serial.print((char)blue.read());
  }
  interrupts();
}


