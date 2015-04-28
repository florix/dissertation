/***********************************************************/
/* Owner: Andrea Floridia                                  */
/* Date: 27/04/2015                                        */
/* Arduino Ethernet Shield, example 2                      */
/* DHCP server, is able to decode 2 kinds of HTTP requests */
/***********************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <stdint.h>



/* GLOBAL VARIABLES */
byte   mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xFA, 0x0F };    // Arduino MAC adress
char    buffer[50];		// buffer used to store the first line of the request
EthernetClient   client;		// The client
uint8_t    NumberOfSpace = 0;			// Number of space in the request
uint8_t    i = 0;						// index for the string buffer


EthernetServer server(80);		// server initialization


/***************************************/
/*	Name: Setup                    */
/*	Params: Void                   */
/*	Return: Void                   */
/*	Description: This is the setup */
/***************************************/
void setup() {
  Serial.begin(9600);
   


  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
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
void loop() {
  // listen for incoming clients
  
  client = server.available();
  
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
      HTTP_response_GET2();
    } else if (strncmp(buffer, "GET /prova", 10) == 0) {
      HTTP_response_GET1();
    } else {
      HTTP_BAD_REQUEST();
    }
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    NumberOfSpace = 0;			// reset for the next connection
    i = 0;		// reset for the next connection
    Serial.println("client disonnected");
  }
}



/*******************************************/
/*	Name: HTTP_response_GET1           */
/*	Params: Void                       */
/*	Return: Void                       */
/*	Description: This is the routine   */
/*	called whenever the path is /prova */
/*******************************************/
void HTTP_response_GET1() 
{
    client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("Ciao");
          client.println("</body>"); 
          client.println("</html>");
}



/********************************************/
/*	Name: HTTP_response_GET2            */
/*	Params: Void                        */
/*	Return: Void                        */
/*	Description: This is the routine    */
/*	called whenever the path is /prova1 */
/********************************************/
void HTTP_response_GET2() 
{
    client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("Ciao Ciao");
          client.println("</body>");
          client.println("</html>");
}



/********************************************************/
/*	Name: HTTP_BAD_REQUEST                          */
/*	Params: Void                                    */
/*	Return: Void                                    */
/*	Description: This is the routine                */
/*	called whenever the path doesn't match anything */
/********************************************************/
void HTTP_BAD_REQUEST() 
{
    client.println("HTTP/1.1 400 BAD REQUEST");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("Bad Request!");
          client.println("</body>");
          client.println("</html>");
}
