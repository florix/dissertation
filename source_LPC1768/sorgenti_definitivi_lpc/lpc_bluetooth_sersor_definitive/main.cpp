/*********************************************************************************/
/* lpc_sensor_bluetooth definitive                                               */
/* this is the definitive source code for lpc, that implements the communication */
/*  with Arduino throught the bluetooth module                                   */
/*                    														     */
/* PIN CONFIGURATION:                                                            */
/* p9 = RX RN-42 pin                                                          	 */
/* p10 = TX RN-42 pin                                                         	 */
/* p15 = central pin TMP36                                                    	 */
/*  Created on: May 23, 2015                                                  	 */
/*      Author: Andrea Floridia                                               	 */
/*********************************************************************************/

#include "mbed.h"
#include <cstdio>
#include <cstring>


/* States */
#define IDLE 0
#define DECODE 1
#define EXECUTION 2

/* Global variables */
struct bitflag{
	unsigned	flag_bluetooth_message : 1;		// is set whenever full message is received
	unsigned	flag_operation_code :2;
} flags;

Serial   		rn42(p9,p10);           // tx, rx
DigitalOut		myled(LED1);			// it's used just to debug, to know when data received
char			buffer[50];				// used to store incoming data from bluetooth
uint8_t			number_of_asterisk;		// it counts the number of asterisk in the buffer
uint8_t			i;						// index used to access buffer
uint8_t			state;					// m3pi state


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
	char	sample[10];

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
		sprintf(sample, "%f", 34.45);		//conversion battery level

		wait(0.1)
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


/***************************************************/
/* Name: obtain_mesurement                         */
/* Params: void                                    */
/* Return: void                                    */
/* This function it's called during excution stage */
/* and perform the /obtainmesurement operation     */
/***************************************************/
void obtain_mesurement()
{
	AnalogIn       temperature_sensor(p15);
	uint16_t       sensor_value;			// value read from the analog in
	float          temperature;				// to store the value after the conversion from mV
	float          voltage;					// conversion from integer to mV
	char		   float_converted[50];				// to store the result of the conversion
	float          sample = 0;				// it's the sample to send via bluetooth
	uint16_t       index = 0;					// index for

	myled = 1;					// turn on, something to transmit

	// conversion
	for (index = 0; index < 1000; i++) {

		sensor_value = temperature_sensor.read_u16();
		voltage = (sensor_value / 65536.0 ) * 3.3;
		temperature = (voltage - .5) * 100;

		sample += temperature;
	}

	sample = sample / 1000.0;

	// conversion float to string, then send via bluetooth

	sprintf(float_converted, "%f", sample);

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





/* MAIN FUNCTION */
int main()
{
	// global setup
	flags.flag_operation_code = 0b00;		// it means no operation required
	flags.flag_bluetooth_message = 0b0;		// initialization
	state = IDLE;
	rn42.attach(&isr_bluetooth);            // attach interrupt to serial port
	myled = 0;								// turn off the led
	bluetooth_setup();
	number_of_asterisk = 0;

	//infinite loop
	while(1) {

		switch(state) {

			case IDLE:
				if(flags.flag_bluetooth_message) {
					state = DECODE;
				}
				break;
			case DECODE:
				decode_message();
				flags.flag_bluetooth_message = 0b0;		//clear the bit
				i = 0;
				number_of_asterisk = 0;					// reset for the next message
				state = EXECUTION;
				break;
			case EXECUTION:
				switch(flags.flag_operation_code) {

					case 0b01:		//obtainmesurement
						send_reply();
						obtain_mesurement();
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

