/* Obstacle avoidance
 * Author: Andrea Floridia
 * Date: 30/05/2015
 *
 * */

#include "mbed.h"
#include "m3pi.h"

/* global variables */
DigitalIn left_sensor(p21);
DigitalIn central_sensor(p22);
DigitalIn right_sensor(p12);

uint8_t timeout_measurement = 0;
Timeout t;
DigitalOut myled(LED1);
uint8_t    timeout_expired = 0;

m3pi m3pi;


void isr_timeout(void) {
    
    timeout_measurement = 1;
     
    }


/* this function detect objects presence */
uint8_t object_presence(){
    
    uint8_t  object = 0;
    
    if(central_sensor.read()==0)
        object ++;
    
    if(left_sensor.read()==0)
        object ++;
    
    if(right_sensor.read()==0)
        object ++;
       
    return object;
}
 
int main() {
    
    myled = 0;

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
    
    t.attach(&isr_timeout, 5.0);

    while (1) {
        
        
        
        if (object_presence() > 0) {

            m3pi.right_motor(0.2);		// turn right, an obstacle has been detected!
            m3pi.left_motor(0.1);
            wait(1);        
            m3pi.left_motor(0.2);		// then turn left
            m3pi.right_motor(0.1);
            wait(1);
            m3pi.forward(0.1);			// forward
            wait(1);
        } 
        else {
            // no obstacle, line following
            
            
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
        
        if(timeout_measurement) {
            if (timeout_expired) {
                 t.detach();
                 m3pi.stop();
                 goto end_routine;   
            }
            else {
                timeout_expired ++;
                //obtain measurement
                timeout_measurement = 0;
                m3pi.stop();
                wait(2.0);
                myled = !myled;
                t.detach();
                t.attach(&isr_timeout,5.0);
            }
        }
    }
    
    end_routine:
		myled = 0;
		timeout_expired = 0;
		timeout_measurement = 0;
}
