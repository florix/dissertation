/***********************************************************/
/* Owner: Andrea Floridia                                  */
/* Date: 27/04/2015                                        */
/* Arduino Ethernet Shield, example 2                      */
/* DHCP server, is able to decode 2 kinds of HTTP requests */
/* the HTTP response message are application/json. Added   */
/* the Bluetooth.                                          */
/* NOTE: stdint.h has been already included because it is  */
/* used by Arduino.h.                                      */
/* Furthermore pins 10,11,12,13 are used by ethernet       */
/* shield.                                                 */
/* The bluetooth message are:                              */
/*        * operation parameter *                          */
/*        * status value *                                 */
/***********************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <SoftwareSerial.h>



/* ETHERNET GLOBAL VARIABLES  */
byte   mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xFA, 0x0F };    // Arduino MAC adress
char    buffer[50];		// buffer used to store the first line of the request	
uint8_t    NumberOfSpace = 0;			// Number of space in the request
uint8_t    i = 0;						// index for the string buffer
EthernetServer    server(80);		// server initialization


/* BLUETOOTH GLOBAL VARIABLES */
uint8_t    blueTX = 2;
uint8_t    blueRX= 3;
volatile uint8_t    NumberOfAsterisk = 0;    // count the number of asterisk
volatile char     c;                  // it's used to read the serial data
volatile char    buffer1[50];            // the buffer used inside the isr
volatile uint8_t    ex = 0;             // exit flag, when it's set it means that the strig has been completed
volatile uint8_t    index = 0;             // string index
char    buffer_debug[50];                      // buffer used to decode the string(because i want to print), it makes sense only in debug mode 
SoftwareSerial     blue(blueTX, blueRX); // RX, TX their'are inverted as usual in USART


/* SMARTPHONE CLASS */
class S2
{
public:
  uint8_t    status;
  uint8_t    value;
  
  S2(void);      // constructor
  uint8_t get_status(void) {return status;};
  uint8_t get_value(void) {return value;};
  void set_status(uint8_t a) {status = a;};
  void set_value(uint8_t a) {value = a;};
  void send_message(uint8_t operation, uint8_t parameter);
  void decode_message(void); 
};


 S2::S2()
{
  status = 0;
  value = 0;
}

void S2::send_message(uint8_t operation, uint8_t parameter)
{
  blue.print("*");
  blue.print(" ");
  blue.print(operation);
  blue.print(" ");
  blue.print(parameter);
  blue.print(" ");
  blue.print("*");
}

void S2::decode_message(void)
{
  buffer1[index] = 0;
  /* Decode and set object params */
  status  = buffer1[2] - 48;
  value = buffer1[4] - 48;
  index = 0;
  ex = 0;
  
}

S2 smartphone;      // S2 object delcaration



/***************************************/
/*	Name: Setup                    */
/*	Params: Void                   */
/*	Return: Void                   */
/*	Description: This is the setup */
/***************************************/
void setup()
{
    Serial.begin(9600);
    
    /* Bluetooth setup */
    blue.begin(115200);
    blue.print("$");
    blue.print("$");
    blue.print("$");
    delay(100);
    blue.println("U, 9600, N");
    blue.begin(9600);
    interrupts();
    attachInterrupt(0, isr_blue, CHANGE); 
    
    /* Ethernet setup */
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
    /* if the flag is set, it means that the string is completed,
       and in this case that it's arrived an unexpected message
       from the smartphone(status update) */
    if(ex == 1) {
      smartphone.decode_message();
      
      /*
      buffer1[index] = 0;        // add terminator
      /* copy the string, ONLY IN THE DEBUG MODE 
      for(uint8_t j = 0; j <= index; j++)
        buffer_debug[j] = buffer1[j];
      index = 0;
      ex = 0;      //  RESET
      Serial.println(buffer_debug);
      */
    }
    
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
  
      
      /* decode the HTTP request from the longest possible path
         in this case the longest could be GET /prova1 */
      if (strncmp(buffer, "GET /prova1", 11) == 0) {
        /* in this case the procedure is: send operation,
           wait for the ack(smartphone is changin its status)
           and then reply to the client */
        smartphone.send_message(1, 2);
        while(ex!=1) {};      // busy waiting
        smartphone.decode_message();
        HTTP_response_GET2(client);
      } else if (strncmp(buffer, "GET /prova", 10) == 0) {
        smartphone.send_message(2, 1);
        while(ex!=1) {};      // busy waiting
        smartphone.decode_message();
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
/*	Name: isr_blue                     */
/*	Params: Void                       */
/*	Return: Void                       */
/*	Description: This is the routine   */
/*	called whenever a bluetooth event  */
/*      occurs                             */
/*******************************************/
void isr_blue(void)
{
    if (blue.available()) {
     
      c = (char)blue.read();
      
      if (c == '*' && NumberOfAsterisk == 0) {
        NumberOfAsterisk ++;
      }      
      else if (c == '*' && NumberOfAsterisk > 0) {
        NumberOfAsterisk = 0;
        ex = 1;      // the string is completed, set exit flag  
      }
      
      			
       buffer1[index] = c;
       index++;
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
    client.print("{ \"temperature\": ");
    client.print(smartphone.get_value());
    client.print(", \"status\": ");
    client.print(smartphone.get_status());
    client.println(" }");    
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
    client.print("{ \"pressure\": ");
    client.print(smartphone.get_value());
    client.print(", \"status\": ");
    client.print(smartphone.get_status());
    client.println(" }");    
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
