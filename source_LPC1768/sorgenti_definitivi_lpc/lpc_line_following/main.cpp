/* lpc line following */

#include "mbed.h"
#include "m3pi.h"
#include "cstdio"

Serial pc(USBTX, USBRX);
uint8_t abort_flag = 0;
uint8_t timeout_measurement = 0;
Timeout t;
m3pi m3pi;
DigitalOut myled(LED1);

void isr_message(void) {
    
    if (pc.readable()) {
        
        char data = (char) pc.getc();
        if(data == 'a')
            abort_flag = 1;
        
        }
 }
 
 
 void isr_timeout(void) {
    
    timeout_measurement = 1;
     
    }
 
int main() {
    
    myled = 0;
    t.attach(&isr_timeout, 3.0);
    pc.attach(&isr_message);
    // Parameters that affect the performance
    float speed = 0.2;
    float correction = 0.1;   
    float threshold = 0.5;
 
    char sample[6];
    char result[50];
    
    
 
    wait(2.0);
    
    sprintf(result, "%f", m3pi.battery());
    sample[0] = result[0];
    sample[1] = result[1];
    sample[2] = result[2];
    sample[3] = result[3];
    sample[4] = 0;

    m3pi.locate(0,1);
    m3pi.printf(sample);
    
    wait(2.0);
 
    m3pi.locate(0,1);
    m3pi.printf("Line Flw");
 
    wait(2.0);
    
    m3pi.sensor_auto_calibrate();
    
    while (1) {
        
        if(abort_flag) {
            abort_flag = 0;
            t.detach();
            m3pi.stop();
            goto end_routine;
            }
            
        if(timeout_measurement) {
            //obtain measurement
            timeout_measurement = 0;
            m3pi.stop();
            wait(2.0);
            myled = !myled;
            t.detach();
            t.attach(&isr_timeout, 3.0);
            }
            
        // -1.0 is far left, 1.0 is far right, 0.0 in the middle
        float position_of_line = m3pi.line_position();
 
        // Line is more than the threshold to the right, slow the left motor
        if (position_of_line > threshold) {
            m3pi.right_motor(speed);
            m3pi.left_motor(speed-correction);
        }
 
        // Line is more than 50% to the left, slow the right motor
        else if (position_of_line < -threshold) {
            m3pi.left_motor(speed);
            m3pi.right_motor(speed-correction);
        }
 
        // Line is in the middle
        else {
            m3pi.forward(speed);
        }
    }
    end_routine:
    myled = 0;
}