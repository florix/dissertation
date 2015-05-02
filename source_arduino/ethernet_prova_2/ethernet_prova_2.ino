/***********************************************************/
/* Owner: Andrea Floridia                                  */
/* Date: 27/04/2015                                        */
/* Arduino Ethernet Shield, example 2                      */
/* DHCP server, is able to decode 2 kinds of HTTP requests */
/* the HTTP response message are application/json          */
/***********************************************************/

// Note: stdint.h has been already included because it is used by Arduino.h
// Furthermore pins 10,11,12,13 are used by ethernet shield.
#include <SPI.h>
#include <Ethernet.h>
#include <string.h>




/* GLOBAL VARIABLES */
byte   mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xFA, 0x0F };    // Arduino MAC adress
char    buffer[50];		// buffer used to store the first line of the request	
uint8_t    NumberOfSpace = 0;			// Number of space in the request
uint8_t    i = 0;						// index for the string buffer
EthernetServer server(80);		// server initialization


/***************************************/
/*	Name: Setup                    */
/*	Params: Void                   */
/*	Return: Void                   */
/*	Description: This is the setup */
/***************************************/
void setup()
{
    Serial.begin(9600);
    Ethernet.begin(mac);    // start the Ethernet connection 
    server.begin();         // and the server
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}


/******************************************/
/*	Name: Loop                        */
/*	Params: Void                      */
/*	Return: Void                      */
/*	Description: This is the main     */
/*	loop, it will be executed forever */
/******************************************/
void loop() 
{
    // listen for incoming clients
    
    EthernetClient   client = server.available();      //the client
    
    if (client) {
      
      Serial.println("new client");
      
      
      /* acquisition HTTP request */
      while (client.connected()) {
        
         if(client.available()) {
          
          char c = client.read();
          if (c == ' ' && NumberOfSpace == 1) {
            buffer[i] = 0;
            goto decode;
          }
          else if (c == ' ') {
            buffer[i] = c;
            i ++;
            NumberOfSpace ++;
          }
          else {
            buffer[i] = c;
            i ++;
          }
        }
        
      }
      
      decode:
      Serial.println(buffer);
  
      
      // decode the HTTP request from the longest possible path
      // in this case the longest could be GET /prova1
      if (strncmp(buffer, "GET /prova1", 11) == 0) {
        HTTP_response_GET2(client);
      } else if (strncmp(buffer, "GET /prova", 10) == 0) {
        HTTP_response_GET1(client);
      } else {
        HTTP_BAD_REQUEST(client);
      }
      
      
      delay(1);         // give the web browser time to receive the data
      client.stop();    // close the connection
      NumberOfSpace = 0;
      i = 0;              // reset for the next client
      Serial.println("client disonnected");
    }
}



/*******************************************/
/*	Name: HTTP_response_GET1           */
/*	Params: A client object            */
/*	Return: Void                       */
/*	Description: This is the routine   */
/*	called whenever the path is /prova */
/*******************************************/
void HTTP_response_GET1(EthernetClient  & client) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    client.println("{ \"temperature\": 14.44, \"status\": 1 }");
    
}



/********************************************/
/*	Name: HTTP_response_GET2            */
/*	Params: A client object             */
/*	Return: Void                        */
/*	Description: This is the routine    */
/*	called whenever the path is /prova1 */
/********************************************/
void HTTP_response_GET2(EthernetClient  & client) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    client.println("{ \"pressure\": 14.44, \"status\": 1 }");    
}



/********************************************************/
/*	Name: HTTP_BAD_REQUEST                          */
/*	Params: A client object                         */
/*	Return: Void                                    */
/*	Description: This is the routine                */
/*	called whenever the path doesn't match anything */
/********************************************************/
void HTTP_BAD_REQUEST(EthernetClient  & client) 
{
    client.println("HTTP/1.1 400 BAD REQUEST");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    client.println("{ \"error message\": \"Bad Request!\"}");        
}
