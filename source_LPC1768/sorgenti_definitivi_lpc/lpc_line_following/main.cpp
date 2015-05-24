/*********************************************************************************/
/* lpc_line_following definitive                                                 */
/* this is the definitive source code for lpc that implements the line following */
/* to stop the mp3i in a given point on the path, is used a timeout that expires */
/* two times: the first one after 5 seconds, and the second one after another 5  */
/* seconds, in that way the m3pi is able to return to the start                  */
/*                    														     */
/* PIN CONFIGURATION:                                                            */
/* p8 = RST to 3pi                                                          	 */
/* p9 = RXD to 3pi                                                         	     */
/* p10 = TXD from 3pi                                                    	     */
/*  Created on: May 24, 2015                                                  	 */
/*      Author: Andrea Floridia                                               	 */
/*********************************************************************************/


#include "mbed.h"
#include "m3pi.h"
#include "cstdio"


uint8_t 	timeout_expired = 0;		//it counts the number of timeout
uint8_t 	timeout_measurement = 0;	//it's a flag, set whenever timeout occurs
Timeout 	t;
m3pi 		m3pi;

 
 /* this is the isr called whenever interrupt from timer occurs */
 void isr_timeout(void)
{
    timeout_measurement = 1;		//set the flag
}
 


int main()
{
    // local variables
    const float 	speed = 0.2;
    const float 	correction = 0.1;
    const float 	threshold = 0.5;
 
    m3pi.locate(0,1);
    m3pi.printf("Line Flw");
 
    wait(2.0);
    
    m3pi.sensor_auto_calibrate();
    t.attach(&isr_timeout, 5.0);
    
    while (1) {
        
            
        if(timeout_measurement) {

        	if(timeout_expired) {
        		t.detach();
        		m3pi.stop();
        		goto end_routine;				//jump to line 91 of this function
        	}
        	else {
				//obtain measurement
        		timeout_expired ++;
				timeout_measurement = 0;		//reset flag
				m3pi.stop();					//stop the 3pi
				wait(2.0);						// to simulate obtain_measurement()
				t.detach();						// reset the timer, must be reloaded
				t.attach(&isr_timeout, 5.0);
        	}
        }

        /* Line following algorithm */
            
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
    	timeout_expired = 0;
    	timeout_measurement = 0;
}
