/*
Owner: Andrea Floridia
Date: 27/04/2015
Arduino Ethernet Shield, example 1
A simple DHCP Server that print via Serial the HTTP request that it recieves,
and it sends as HTTP response a html page.
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xFA, 0x0F };
  
String buffer;

unsigned short int    NumberOfSpace = 0;

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. 
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  
  EthernetClient client = server.available();
  
  if (client) {
    
    Serial.println("new client");
    
    /* acquisition HTTP request */
    while (client.connected()) {
      
      while (client.available()) {
        
        String c = String(client.read());
        if (c.compareTo(" ") == 0 && NumberOfSpace == 1) {
          break;
        }
        else
          buffer = buffer + c;
       
      }
      if (NumberOfSpace == 1) break;
    }
    Serial.println(buffer);
    /*decode the HTTP request*/
    
    if ( buffer.substring(0) == "GET") {
      if (buffer.substring(5) == "/prova")
        int a = 1;
        /*HTTP response for /prova*/
      else if ( buffer.substring(5) == "/prova1")
        int a = 2;
        /* HTTP response for /prova1 */
    } else 
      int a = 1;
    /* HTTP response Bad Request */ 
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void HTTP_response_GET(void) 
{
    client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          
          client.println("<body>");
          client.println("Ciao");
          client.println("</body>");
          
          client.println("</html>");
   

}

