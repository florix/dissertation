/* what i read from the serial is sent on the PC serial */


#include "mbed.h"
 
Serial rn42(p9,p10);
Serial pc(USBTX, USBRX); // tx, rx

 
int main() {
    
   
    
    while (1) {
        
        while (rn42.readable()) {
            
            pc.putc(rn42.getc());
            
        
        }
}

}