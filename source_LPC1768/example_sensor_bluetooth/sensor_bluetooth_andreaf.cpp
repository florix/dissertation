/******************************************************************************/
/* sensor_bluetooth_andreaf.cpp                                               */
/* In this example the MCU reads values from sensor and it sends these values */
/* to the user via bluetooth                                                  */
/*                                                                            */
/*  Created on: May 9, 2015                                                   */
/*      Author: Andrea Floridia                                               */
/******************************************************************************/

#include "mbed.h"
#include "string"
#include "iostream"
#include "sstream"

using namespace std;

// global variables
AnalogIn    sensor(p15);
Serial   rn42(p9,p10);          // tx, rx
Serial   pc(USBTX, USBRX);      // tx, rx
uint8_t 	begin;		// is set whenever go is received


/********************************************/
/* Name: callback                           */
/* Params: void                             */
/* Return: void                             */
/* This is the interrupt service routine    */
/* called whenever a data arrives form      */
/* the bluetooth                            */
/********************************************/
void callback(void)
{
    if (rn42.readable()) {
    // get the new byte:
    char data = (char)rn42.getc();

    if (data == 'g')
    	begin = 1;
  }
 }


int main()
{

	uint16_t       sensor_value;		// value read from the analog in
	float          temperature;			// to store the value after the conversion from mV
	float          voltage;				// conversion from integer to mV
	stringstream ss (stringstream::in | stringstream::out);		// needed for the conversion
	string		   data;				// to store the result of the conversion
	float          sample = 0;			// it's the sample to send via bluetooth
	uint16_t       i = 0;				// counter

	//setup
	begin = 0;
    rn42.attach(&callback);             // attach interrupt to serial port

    // setup rn42, 115200 it's too fast for data integrity
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


    // main loop
    while(1) {

    	while(begin == 0){};		// waiting for the command

    	// conversion
    	for (i = 0; i < 1000; i++) {

    		sensor_value = sensor.read_u16();
    		voltage = (sensor_value / 65536.0 ) * 3.3;
    		temperature = (voltage - .5) * 100;

    		sample += temperature;


    	}

    	sample = sample / 1000.0;

    	// conversion float to string, then send via bluetooth
    	ss << sample;
    	data = ss.str();

    	for (i=0; i<data.length();i++) {
    		rn42.putc(data[i]);
    	}

    	begin = 0;				// reset for the new command

    }

}




