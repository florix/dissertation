/* prova arduino bluetooth+ethernet shiled, senza codice per lavorare con
quest'ultima. Aggiunto algoritmo per la decodifica del mex bluetooh. In 
polling funziona senza problemi(sia RX che TX) mentre gestendola tramite 
interrupt non funziona la RX(no conflitto perchè l'altro sketch funziona!)+
Il problema è il do while nella isr. Praticamente nella isr non posso 
inserire un loop perchè è la variabile a cui faccio riferimento  viene
congelata al momento di entrare nella isr e non viene aggiornata, in 
quanto sto usando sofware serial e non le porte uart standard!*/

#include <SoftwareSerial.h>

int  blueTX = 2;
int  blueRX= 3;

volatile uint8_t  NumberOfAsterisk = 0;    // count the number of asterisk
volatile char     c;                  // it's used to read the serial data
volatile char buffer1[50];            // the buffer used inside the isr
volatile uint8_t  ex = 0;             // exit flag, when it's set it means that the strig has been completed
volatile uint8_t   i = 0;             // string index
char buffer[50];                      // buffer used to decode the string(because i want to print), it makes sense only in debug mode 


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
    /* if the flag is set, it means that the string is completed */
    if(ex == 1) {
      buffer1[i] = 0;        // add terminator
      /* copy the string, ONLY IN THE DEBUG MODE */
      for(uint8_t j = 0; j <= i; j++)
        buffer[j] = buffer1[j];
      i = 0;
      ex = 0;      //  RESET
      Serial.println(buffer);
    }
}


void isr_blue(void)
{
  
    
    if (blue.available()) {
     
      c = (char)blue.read();
      
      if (c == '*' && NumberOfAsterisk == 0) {
        NumberOfAsterisk ++;
      }
      else if (c == '*' && NumberOfAsterisk > 0) {
        NumberOfAsterisk = 0;
        ex = 1;      // the string is completed, set exit flag  
      }
      			
       buffer1[i] = c;
       i++;
    }   
}


