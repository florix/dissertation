/* legge i dati che gli arrivano dal bluetooth e li manda sulla porta serial con il PC,
 una stringa alla volta */
#include "mbed.h"


#define     TRUE    1
#define     FALSE   0
 

Serial   rn42(p9,p10);          // tx, rx
Serial   pc(USBTX, USBRX);      // tx, rx


char data;          // il dato letto di volta in volta dalla serial con il bluetooth
char data_string[50];           // stringa da stampare
int     i;                      // contatore
int stringComplete = FALSE;         // booleano per vedere se la stringa è stata completata


/*   Interrupt Servire Routine  */ 
/*     E' chiamata ogni volta   */
/* che arrivano dati alla porta */
/*     riservata al bluetooth   */
void callback(void)
{
    while (rn42.readable()) {
    // get the new byte:
    char data = (char)rn42.getc();
    // add it to the inputString:
    data_string[i] = data;
    i ++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (data == '\n') {
      stringComplete = TRUE;
      data_string[i] = 0;       //terminator
    }
  }
 }
 
 
 
int main() {
    
    /* setup */
    i = 0;
    rn42.attach(&callback);             // attacco l'interrupt alla porta seriale
    
    
    /* main loop */
    while (1) {
        
       /* se la conversione è stata completata allora può stampare */ 
       if (stringComplete) {
            pc.printf(data_string);
           
            stringComplete = FALSE;
            i = 0;      // reset della strigna
          }
        
        }
}
