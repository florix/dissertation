/*********************************************************************************/
/* m3pi definitive                                                 				 */
/* this is the definitive source code for lpc that implements both the line		 */
/* following and the communication with Arduino via bluetooth                    */
/* 																				 */
/*                    														     */
/* PIN CONFIGURATION:                                                            */
/* p8 = RST to 3pi                                                          	 */
/* p9 = RXD to 3pi                                                         	     */
/* p10 = TXD from 3pi                                                    	     */
/* p13 = RX RN-42 pin                  											 */
/* p14 = TX RN-42 pin                 											 */
/* p15 = central pin TMP36                                                       */
/* p12 = right infrared sensor                                                   */
/* p21 = left infrared sensor                                                    */
/* p22 = central infrared sensor                   								 */
/*  Created on: May 24, 2015                                                  	 */
/*      Author: Andrea Floridia                                               	 */
/*********************************************************************************/


#include "mbed.h"
#include "m3pi.h"
#include <cstdio>
#include <cstring>

/* FSM States */
#define IDLE 0
#define DECODE 1
#define EXECUTION 2

/* Global variables */
struct bitflag{
	unsigned	flag_bluetooth_message : 1;		// is set whenever full message is received
	unsigned	flag_operation_code :2;		    // hold the operation code
	unsigned	flag_timeout_expired :1;		// set when the first timeout occurs
	unsigned	flag_timeout_measurement :1;	// set whenever a timeout occurs
} flags;

Serial   		rn42(p13,p14);          // tx, rx
DigitalOut		myled(LED1);			// it's used just to debug, to know when data received
char			buffer[50];				// used to store incoming data from bluetooth
uint8_t			number_of_asterisk;		// it counts the number of asterisk in the buffer
uint8_t			i;						// index used to access buffer
uint8_t			state;					// m3pi state
Timeout 		t;						// this is the timer
m3pi 			m3pi;					// this is the 3pi
DigitalIn 		left_sensor(p21);		// left sensor used for obstacle avoidance
DigitalIn 		central_sensor(p22);	// central sensor used for obstacle avoidance
DigitalIn 		right_sensor(p12);		// right sensor used for obstacle avoidance


/********************************************/
/* Name: isr_bluetooth                      */
/* Params: void                             */
/* Return: void                             */
/* This is the interrupt service routine    */
/* called whenever a data arrives form      */
/* the bluetooth                            */
/********************************************/
void isr_bluetooth(void)
{
    if (rn42.readable()) {
    // get the new byte:
    	char data = (char)rn42.getc();

    	if (data == '*' && number_of_asterisk > 0) {
    		buffer[i] = data;
    		i++;
    		buffer[i] = 0;							//add the terminator at the end
    		flags.flag_bluetooth_message = 0b1;		// set the bit
    		i = 0;									// reset for the next transmission
    		number_of_asterisk = 0;
    	}
    	else if (data == '*' && number_of_asterisk == 0) {
    		buffer[i] = data;
    		number_of_asterisk ++;
    		i++;
    	}
    	else {
    		buffer[i] = data;
    		i++;
    	}

  }
 }


/********************************************/
/* Name: isr_timeout                        */
/* Params: void                             */
/* Return: void                             */
/* This is the interrupt service routine    */
/* called whenever an interrupt occurs      */
/********************************************/
void isr_timeout(void)
{
    flags.flag_timeout_measurement = 0b1;		//set the flag
}


/****************************************************/
/* Name: obtain_measurement                         */
/* Params: void                                     */
/* Return: void                                     */
/* This function it's called during execution stage */
/* and perform the /obtainmeasurement operation     */
/****************************************************/
void obtain_measurement()
{
	AnalogIn       temperature_sensor(p15);
	uint16_t       sensor_value;			// value read from the analog in
	float          temperature;				// to store the value after the conversion from mV
	float          voltage;					// conversion from integer to mV
	char		   float_converted[50];				// to store the result of the conversion


	myled = 1;					// turn on, something to transmit


	sensor_value = temperature_sensor.read_u16();
	voltage = (sensor_value / 65536.0 ) * 3.3;
	temperature = (voltage - .5) * 100;


	sprintf(float_converted, "%f", temperature);

	rn42.putc('*');
	wait(0.1);
	rn42.putc(' ');
	wait(0.1);
	rn42.putc('2');
	wait(0.1);
	rn42.putc(' ');
	wait(0.1);
	rn42.putc(float_converted[0]);
	wait(0.1);
	rn42.putc(float_converted[1]);
	wait(0.1);
	rn42.putc(float_converted[2]);
	wait(0.1);
	rn42.putc(float_converted[3]);
	wait(0.1);
	rn42.putc(float_converted[4]);
	wait(0.1);
	rn42.putc(' ');
	wait(0.1);
	rn42.putc('#');
	wait(0.1);
	rn42.putc('#');
	wait(0.1);
	rn42.putc('#');
	wait(0.1);
	rn42.putc('#');
	wait(0.1);
	rn42.putc('#');
	wait(0.1);
	rn42.putc(' ');
	wait(0.1);
	rn42.putc('*');

	myled = 0;
}

/**************************************************/
/* Name: bluetooth_setup                          */
/* Params: void                                   */
/* Return: void                                   */
/* This is the bluetooth setup, it's called       */
/* just once in the program. It enters in         */
/* command mode and it changes the baud rate from */
/* 115200 to 9600, because 115200 it's too fast   */
/* to guarantee data integrity                    */
/**************************************************/
void bluetooth_setup(void)
{
	// 115200 it's too fast for data integrity
	rn42.baud(115200);
	rn42.putc('$');
	rn42.putc('$');
	rn42.putc('$');
	wait(0.1);
	rn42.putc('U');
	rn42.putc(',');
	rn42.putc('9');
	rn42.putc('6');
	rn42.putc('0');
	rn42.putc('0');
	rn42.putc(',');
	rn42.putc('N');
	rn42.putc('\n');
	rn42.baud(9600);

	// setup slave
	rn42.putc('$');
	rn42.putc('$');
	rn42.putc('$');
	wait(0.1);
	rn42.putc('S');
	rn42.putc('M');
	rn42.putc(',');
	rn42.putc('0');
	rn42.putc('\n');
	wait(0.1);
	rn42.putc('-');
	rn42.putc('-');
	rn42.putc('-');
	rn42.putc('\n');
}


/**************************************************/
/* Name: decode_message                           */
/* Params: void                                   */
/* Return: void                                   */
/* This function it's called during decode stage  */
/* and decode the string received from bluetooth  */
/* enabling one operation 						  */
/**************************************************/
void decode_message()
{
	if(strncmp(buffer, "* 1 1 *", 7) == 0) {
		flags.flag_operation_code = 0b01;		// op_code = 1 means /obtainmesurement
	}
	else if (strncmp(buffer, "* 0 0 *", 7) == 0) {
		flags.flag_operation_code = 0b10;		// op_code = 2 means /obtainstatus
	}
}


/****************************************************/
/* Name: send_reply                                 */
/* Params: void                                     */
/* Return: void                                     */
/* This function it's called during execution stage */
/* and send the proper message. the message depend  */
/* on the operation enabled   				        */
/****************************************************/
void send_reply()
{
	char	sample[30];		// to store conversion result

	myled = 1;

	if (flags.flag_operation_code == 0b01) {
		// the reply contains just the current state
		rn42.putc('*');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('2');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('*');
	}
	else if (flags.flag_operation_code == 0b10) {
		// the reply contains state and battery level
		sprintf(sample, "%f", m3pi.battery());		//conversion battery level

		wait(0.1);
		rn42.putc('*');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('2');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc('#');
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc(sample[0]);
		wait(0.1);
		rn42.putc(sample[1]);
		wait(0.1);
		rn42.putc(sample[2]);
		wait(0.1);
		rn42.putc(sample[3]);
		wait(0.1);
		rn42.putc(sample[4]);
		wait(0.1);
		rn42.putc(' ');
		wait(0.1);
		rn42.putc('*');
	}

	myled = 0;
}


/*********************************************/
/* Name: object_presence                     */
/* Params: void                              */
/* Return: uint8_t                           */
/* This routine detects the presence of any  */
/* object along the path 					 */
/*********************************************/
uint8_t object_presence(){

    uint8_t  object = 0;

    if(central_sensor.read() == 0)
        object ++;

    if(left_sensor.read() == 0)
        object ++;

    if(right_sensor.read() == 0)
        object ++;

    return object;
}


/*********************************************/
/* Name: line_following                      */
/* Params: void                              */
/* Return: void                              */
/* This is the line following routine        */
/* called in when the operation requested is */
/* /obtainmeasuremnt                         */
/*********************************************/
void line_following(void)
{
    // local variables
    const float 	speed = 0.2;
    const float 	correction = 0.1;
    const float 	threshold = 0.5;

    m3pi.locate(0,1);
    m3pi.printf("Line Flw");		// print on LCD display

    wait(2.0);

    m3pi.sensor_auto_calibrate();
    t.attach(&isr_timeout, 5.0);		// attach isr for timeout

    while (1) {

    	// check for obstacles along the path
    	if (object_presence() > 0) {

    		//an obstacle has been detected
    		m3pi.right_motor(0.2);		// turn right
    		m3pi.left_motor(0.1);
    		wait(1);
    		m3pi.left_motor(0.2);		// then turn left
    		m3pi.right_motor(0.1);
    		wait(1);
    		m3pi.forward(0.1);			// forward
    		wait(1);
    	}
    	else {
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

    	// check if timeout has expired
        if(flags.flag_timeout_measurement) {

        	if(flags.flag_timeout_expired) {
        		t.detach();
        		m3pi.stop();
        		goto end_routine;				//jump out of the loop
        	}
        	else {
				//obtain measurement
        		flags.flag_timeout_expired = 0b1;			//set the flag
				flags.flag_timeout_measurement = 0b0;		//reset flag
				m3pi.stop();								//stop the 3pi
				obtain_measurement();						// perform measurement
				t.detach();									// reset the timer, must be reloaded
				t.attach(&isr_timeout, 5.0);
        	}
        }
    }

    end_routine:
    	flags.flag_timeout_expired = 0b0;
    	flags.flag_timeout_measurement = 0b0;
}


/* MAIN FUNCTION */
int main()
{
	// global setup
	flags.flag_operation_code = 0b00;		// it means no operation required
	flags.flag_bluetooth_message = 0b0;		// initialization
	flags.flag_timeout_expired = 0b0;
	flags.flag_timeout_measurement = 0b0;
	state = IDLE;
	rn42.attach(&isr_bluetooth);            // attach interrupt to serial port
	myled = 0;								// turn off the led
	bluetooth_setup();
	number_of_asterisk = 0;
	i = 0;

	//infinite loop
	while(1) {

		switch(state) {

			case IDLE:
				while(!flags.flag_bluetooth_message){};
				state = DECODE;
				break;
			case DECODE:
				decode_message();
				flags.flag_bluetooth_message = 0b0;		//clear the bit
				state = EXECUTION;
				break;
			case EXECUTION:
				switch(flags.flag_operation_code) {

					case 0b01:		//obtainmesurement
						send_reply();
						wait(0.1);
						line_following();
						state = IDLE;
						flags.flag_operation_code = 0b00;		//nop
						break;
					case 0b10:		//obtainstatus
						send_reply();
						state = IDLE;
						flags.flag_operation_code = 0b00;		//nop
						break;
				}
				break;
		}
	}

}
