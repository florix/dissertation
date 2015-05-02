/* prova arduino bluetooth+ethernet shiled, senza codice per lavorare con
quest'ultima. Aggiunto algoritmo per la decodifica del mex bluetooh. In 
polling funziona senza problemi(sia RX che TX) mentre gestendola tramite 
interrupt non funziona la RX(possibile conflitto?)*/

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
  //interrupts();
  //attachInterrupt(0, isr_blue, CHANGE);    
}


void loop()
{
  uint8_t  NumberOfAsterisk = 0;
  char     c;
   if(blue.available()) {
     do {
      if (blue.available()) {
        c = (char)blue.read();
        if (c == '*' && NumberOfAsterisk == 0) {
          NumberOfAsterisk ++;
        }
        else if (c == '*' && NumberOfAsterisk > 0) {
          NumberOfAsterisk = 0;
        }			
        Serial.print(c);
      }
    } while (NumberOfAsterisk == 0);
   } 
   if(Serial.available()) {
      blue.print((char)Serial.read());
    }
}


void isr_blue(void)
{
  uint8_t  NumberOfAsterisk = 0;
  char     c;
  noInterrupts();
  do {
    if (blue.available()) {
      c = (char)blue.read();
      if (c == '*' && NumberOfAsterisk == 0) {
        NumberOfAsterisk ++;
      }
      else if (c == '*' && NumberOfAsterisk > 0) {
        NumberOfAsterisk = 0;
      }			
      Serial.print(c);
    }
  } while (NumberOfAsterisk == 0);
  interrupts();
}


