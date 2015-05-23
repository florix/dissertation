/*lettura sensore di temperatura*/

#include "mbed.h"
#include "stdint.h"

AnalogIn    sensor(p15);
Serial      pc(USBTX, USBRX);

int main()
{
    uint16_t       sensor_value;
    float          temperature;
    float          voltage;
    float          sample = 0;
    int     i = 0;
    
    while (1) {
        
        for (i = 0; i < 1000; i++) {
            
            sensor_value = sensor.read_u16();
            voltage = (sensor_value / 65536.0 ) * 3.3;
            temperature = (voltage - .5) * 100;
            
            sample += temperature;
            
            
           } 
        
        sample = sample / 1000.0;
        pc.printf(" %f \n", sample);   
        
        wait(2);
   } 
}
