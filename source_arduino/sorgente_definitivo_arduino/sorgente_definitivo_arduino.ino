/***************************************************************************/
/* Owner: Andrea Floridia                                                  */
/* Date: 23/05/2015                                                        */
/* Arduino Server, is able to decode the following HTTP request(API):      */
/* - /obtainmesurement                                                     */
/* - /obtainstatus                                                         */
/* - /update                                                               */
/*                                                                         */
/* the HTTP response message are application/json.                         */
/* The communication with the m3pi is done thought the  Bluetooth module.  */
/*                                                                         */
/* PINS CONFIGURATION:                                                     */
/* - 10,11,12,13 are used by ethernet shield.                              */
/* - 2, 3 are used by bluetooth, pin 2 = TX pin blue, pin 3 = RX pin blue  */
/*                                                                         */
/* The bluetooth messages protocol is:                                     */
/* -  * operation parameter *, for operation                               */
/* -  * latest_status value battery_level * , m3pi response                */
/***************************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <SoftwareSerial.h>


/* ETHERNET GLOBAL VARIABLES  */
byte              mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xFA, 0x0F };    // Arduino MAC adress
char              buffer[50];		                                     // buffer used to store the first line of the request	
uint8_t           NumberOfSpace = 0;			                           // Number of space in the request
uint8_t           i = 0;						                                 // index for the string buffer
EthernetServer    server(80);		                                     // server initialization


/* BLUETOOTH GLOBAL VARIABLES */
const uint8_t       blueTX = 2;
const uint8_t       blueRX= 3;
volatile uint8_t    NumberOfAsterisk = 0;           // count the number of asterisk
volatile char       c;                              // it's used to read the serial data
volatile char       buffer1[50];                    // the buffer used inside the isr
volatile boolean    bluetooth_message = false;      // exit flag, when it's set it means that the strig has been completed
volatile uint8_t    index = 0;                      // string index
char                buffer_debug[50];               // buffer used to decode the string(because i want to print), it makes sense only in debug mode 
SoftwareSerial      blue(blueTX, blueRX);           // RX, TX they are inverted as usual in USART


/* M3PI CLASS */
class m3pi
{
    public:
          //attributes
          uint8_t    latest_status;     //this variable holds the latest m3pi's status
          char       value[6];          //this holds the value read from the temperature sensor. 5 char to the float + '\0'
          char       battery_level[6];  //this holds the battery level

          //methods
          m3pi(void);                   // constructor
          void send_message(uint8_t operation, uint8_t parameter);
          void decode_message(void); 
};


/* M3PI METHODS AND CONSTRUCTOR IMPLEMENTATION */

/*******************************************/
/*  Name: m3pi                             */
/*  Params: Void                           */
/*  Return: Void                           */
/*  Description: This is the constructor   */
/*  it initializes m3pi attributes         */
/*******************************************/
m3pi::m3pi()
{
    latest_status = 0;

    value[0] = '#';
    value[1] = '#';
    value[2] = '#';
    value[3] = '#';
    value[4] = '#';
    value[5] = 0;

    battery_level[0] = '#';
    battery_level[1] = '#';
    battery_level[2] = '#';
    battery_level[3] = '#';
    battery_level[4] = '#';
    battery_level[5] = 0;
}


/*********************************************/
/*  Name: send_message                       */
/*  Params: operation code, operation params */
/*  Return: Void                             */
/*  Description: This is called whenever     */
/*  it's requested to deliver a message to   */
/*  m3pi                                     */
/*********************************************/
void m3pi::send_message(uint8_t operation, uint8_t parameter)
{
  blue.print("*");
  blue.print(" ");
  blue.print(operation);
  blue.print(" ");
  blue.print(parameter);
  blue.print(" ");
  blue.print("*");
}


/**********************************************/
/*  Name: decode_message                      */
/*  Params: Void                              */
/*  Return: Void                              */
/*  Description: This is called whenever      */
/*  it's requested to decode a message from   */
/*  m3pi                                      */
/**********************************************/
void m3pi::decode_message(void)
{
  buffer1[index] = 0;
  /* Decode and set object params */
  latest_status  = buffer1[2] - 48;
  value[0] = buffer1[4];
  value[1] = buffer1[5];
  value[2] = buffer1[6];
  value[3] = buffer1[7];
  value[4] = buffer1[8];
  value[5] = 0;
  battery_level[0] = buffer1[10];
  battery_level[1] = buffer1[11];
  battery_level[2] = buffer1[12];
  battery_level[3] = buffer1[13];
  battery_level[4] = buffer1[14];
  battery_level[5] = 0;
  index = 0;
  bluetooth_message = false; 
}

// object instance
m3pi robot;


/*******************************************/
/*  Name: isr_blue                         */
/*  Params: Void                           */
/*  Return: Void                           */
/*  Description: This is the routine       */
/*  called whenever a bluetooth event      */
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
        bluetooth_message = true;      // the string is completed, set exit flag  
      }
      
                
       buffer1[index] = c;
       index++;
    }   
}


/***********************************************/
/*  Name: HTTP_response_obtainstatus           */
/*  Params: A client object                    */
/*  Return: Void                               */
/*  Description: This is the routine           */
/*  called whenever the path is /obtainstatus  */
/***********************************************/
void HTTP_response_obtainstatus(EthernetClient  & client) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{ \"battery_level\": ");
    client.print(robot.battery_level);
    client.print(", \"latest_status\": ");
    client.print(robot.latest_status);
    client.println(" }");    
}



/******************************************************/
/*  Name: HTTP_response_obtainmesurement              */
/*  Params: A client object                           */
/*  Return: Void                                      */
/*  Description: This is the routine                  */
/*  called whenever the path is /obtainmesurement     */
/******************************************************/
void HTTP_response_obtainmesurement(EthernetClient  & client) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{ \"latest_status\": ");
    client.print(robot.latest_status);
    client.println(" }");    
}


/*****************************************/
/*  Name: HTTP_response_update           */
/*  Params: A client object              */
/*  Return: Void                         */
/*  Description: This is the routine     */
/*  called whenever the path is /update  */
/*****************************************/
void HTTP_response_update(EthernetClient  & client) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{ \"value\": ");
    client.print(robot.value);
    client.print(", \"latest_status\": ");
    client.print(robot.latest_status);
    client.println(" }");    
}


/*****************************************/
/*  Name: HTTP_response_500              */
/*  Params: A client object              */
/*  Return: Void                         */
/*  Description: This is the routine     */
/*  called whenever an 500 error occurs  */
/*****************************************/
void HTTP_response_500(EthernetClient  & client) 
{
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println("{ \"message\": Internal Server Error } ");
}



/********************************************************/
/*  Name: HTTP_BAD_REQUEST                              */
/*  Params: A client object                             */
/*  Return: Void                                        */
/*  Description: This is the routine                    */
/*  called whenever the path doesn't match anything     */
/********************************************************/
void HTTP_BAD_REQUEST(EthernetClient  & client) 
{
    client.println("HTTP/1.1 400 BAD REQUEST");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println("{ \"error message\": \"Bad Request!\"}");        
}



/***************************************/
/*	Name: Setup                        */
/*	Params: Void                       */
/*	Return: Void                       */
/*	Description: This is the setup     */
/***************************************/
void setup()
{
      Serial.begin(9600);

      /* Bluetooth setup */
      blue.begin(115200);      // The Bluetooth  defaults to 115200bps
      blue.print("$"); 
      blue.print("$");
      blue.print("$");         // Enter command mode
      delay(100);                   // Short delay, wait for the Mate to send back CMD
      blue.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity, 115200 can be too fast at times for NewSoftSerial to relay the data reliably
      blue.begin(9600);          // Start bluetooth serial at 9600


      // set as master and connect to the slave
      delay(5000);                    // wait 5 sec and then connect
      blue.print("$$$");
      delay(100);
      blue.println("SM,1"); 
      delay(100);
      blue.println("C,0006664FE329");
      delay(100);
      blue.println("---"); 
      
      /* empty the buffer */
      while(blue.available()) {

        blue.read();

      }
      interrupts();
      attachInterrupt(0, isr_blue, CHANGE); 
    
      /* Ethernet setup */
      Ethernet.begin(mac);    // start the Ethernet connection 
      server.begin();         // and the server
      Serial.print("server is at ");
      Serial.println(Ethernet.localIP());
}


/******************************************/
/*  Name: Loop                            */
/*  Params: Void                          */
/*  Return: Void                          */
/*  Description: This is the main         */
/*  loop, it will be executed forever     */
/******************************************/
void loop() 
{
    /* if the flag is set, it means that the string is completed,
       and in this case that it's arrived an unexpected message
       from the smartphone(latest_status update) */
    if(bluetooth_message) {
      noInterrupts();
      robot.decode_message();
      interrupts();
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
         in this case the longest could be GET /obtainmesurement */
      if (strncmp(buffer, "GET /obtainmesurement", 21) == 0) {
        
        // to obtain the latest info
        if(bluetooth_message) {
          robot.decode_message();
        }

        robot.send_message(1, 1);

        while(!bluetooth_message);    //busy waiting
        robot.decode_message();
        // if robot status is equal to 1 it means that it has correctly
        // received the message and it has changed its status
        if(robot.latest_status == 2) {
          HTTP_response_obtainmesurement(client);      
        }
        else {
          HTTP_response_500(client);
        }
      }
      else if (strncmp(buffer, "GET /obtainstatus", 17) == 0) {
        
        if(bluetooth_message) {
          robot.decode_message();
        }
        robot.send_message(0, 0);
        while (!bluetooth_message);
        robot.decode_message();
        HTTP_response_obtainstatus(client);
      }
      else if(strncmp(buffer, "GET /update", 11) == 0) {

        if(bluetooth_message) {
          robot.decode_message();
        }

        HTTP_response_update(client);    
      }
      else {
        HTTP_BAD_REQUEST(client);
      }
      
      
      delay(1);         // give the web browser time to receive the data
      client.stop();    // close the connection
      NumberOfSpace = 0;
      i = 0;              // reset for the next client
      Serial.println("client disonnected");
    }
}


