/******************************************************************************/
/* sensor_bluetooth_andreaf.cpp                                               */
/* In this example the MCU reads values from sensor and it sends these values */
/* to the user via bluetooth                                                  */
/* UPDATED VERSION: lightweight float to string conversion                    */
/* PIN CONFIGURATION:                                                         */
/* p9 = RX RN-42 pin                                                          */
/* p10 = TX RN-42 pin                                                         */
/* p15 = central pin TMP36                                                    */
/*  Created on: May 16, 2015                                                  */
/*      Author: Andrea Floridia                                               */
/******************************************************************************/

#include "mbed.h"
#include <cstdio>
#include <cstring>


// global variables
Serial   rn42(p9,p10);          // tx, rx
uint8_t 	begin;				// is set whenever go is received
DigitalOut myled(LED1);			// it's used just to debug, to know when data received

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

    	if (data == 'g')
    		begin = 1;
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



int main()
{
	AnalogIn       temperature_sensor(p15);
	uint16_t       sensor_value;		// value read from the analog in
	float          temperature;			// to store the value after the conversion from mV
	float          voltage;				// conversion from integer to mV
	char		   data[50];			// to store the result of the conversion
	float          sample = 0;			// it's the sample to send via bluetooth
	uint16_t       i = 0;				// index for

	//setup
	begin = 0;
    rn42.attach(&isr_bluetooth);             // attach interrupt to serial port
    myled = 0;								// turn off the led
    // setup rn42
    bluetooth_setup();


    // main loop
    while(1) {

    	while(begin == 0){};		// waiting for the command

    	myled = 1;					// turn on, something to trasmitt
    	// conversion
    	for (i = 0; i < 1000; i++) {

    		sensor_value = temperature_sensor.read_u16();
    		voltage = (sensor_value / 65536.0 ) * 3.3;
    		temperature = (voltage - .5) * 100;

    		sample += temperature;
    	}

    	sample = sample / 1000.0;

    	// conversion float to string, then send via bluetooth

    	sprintf(data, "%f", sample);

    	rn42.putc('*');

    	/*NOTE that between two putc() there are always wait(0.1)
    	 * because arduino has 8-bit CPU at 16MHz, while LPC has
    	 * ARM cortex 3 at 69MHz!*/
    	wait(0.1);

    	for (i=0; i<strlen(data);i++) {
    		rn42.putc(data[i]);
    		wait(0.1);
    	}

    	wait(0.1);

    	rn42.putc('*');

    	begin = 0;				// reset for the new command
    	myled = 0;
    }

    return 0;

}




