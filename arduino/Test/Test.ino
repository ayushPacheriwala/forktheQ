

/*
  Web client

 This sketch connects to a website through a GSM shield. Specifically,
 this example downloads the URL "http://arduino.cc/asciilogo.txt" and
 prints it to the Serial monitor.

 Circuit:
 * GSM shield attached to an Arduino
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMExamplesWebClient

 */

// libraries
#include <Keypad.h>
#include <GSM.h>
#include <hardSerialLCD.h>

//-------------Keypad-------------------------------------------------------------------------------------
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 37,43,41,39};
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 30,28,32}; 

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//----------------keypad end-----------------------------------------------------------------------------------

//========================================================================

const int nextCMD = 2;
const int prevCMD = 1;
const int completeCMD = 0;
const int refreshCMD = 3;
const int errorCMD = -1;
const int noCMD = -2;

//________________________________________________________________________

//-----------------------------GSM Shield-----------------------------------------------------------------------
  // PIN Number
  #define PINNUMBER ""

  // APN data
  #define GPRS_APN       "internet" // replace your GPRS APN
  #define GPRS_LOGIN     ""    // replace with your GPRS login
  #define GPRS_PASSWORD  "" // replace with your GPRS password

  // initialize the library instance
  GSMClient client;
  GPRS gprs;
  GSM gsmAccess;

  // URL, path & port (for example: arduino.cc)
  char server[] = "forktheq.karankamath.com";
  char path[] = "/getorders?num=2";
  int port = 80; // port 80 is the default for HTTP

//------------------------------------------------GSM Shield end----------------------------------------------------

//---------------------------------LCD------------------------------------------------------------------------------
  HardSerialLCD LCD;
//-----------------------------------------LCD END------------------------------------------------------------------


//----------------------------------------extra stuff----------------------------------------------------
  typedef struct {
    String id;
    String menuItem;
    String extras; 
  } order;

  order orderArray[10]; 
  int index;

  int remainingOrders;
  void retreiveInfo(order array[]);
//---------------------------------------------------------------------------------------------------------

//-------------------------------------------setup functions --------------------------------------------------------

  void lcdSetup() {
    LCD.begin();
    delay(10);
    LCD.backlight();
    delay(10);
    LCD.clear();
    delay(10);
    char temp2[100] = "hello";
    LCD.print(temp2);
  }
//-------------------------------------------setup func end -----------------------------------------------------------
void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  lcdSetup();

  Serial.println("Starting Fork the Q web client.");
  // connection state
  boolean notConnected = true;
  LCD.print("Hello Hawker");
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  
  index = 0;
  
  //pull orders from cloud.
  //displayOrders(index);
  

  //-----------------------testing------------------------\
  remainingOrders = 4;
  order orderA;
  
  orderA.id = "000";
  orderA.menuItem = "1x Chicken Rice";
  orderA.extras = "No chilli";
  
  orderArray[0] = orderA;
  
  orderA.id = "101";
  orderA.menuItem = "2x Nasi Lemak";
  orderA.extras = "extra spicy";
  
  orderArray[1] = orderA;
  
  orderA.id = "202";
  orderA.menuItem = "1x Satay Chicken";
  orderA.extras = "w pepper sauce";
  
  orderArray[2] = orderA;
  
  orderA.id = "303";
  orderA.menuItem = "1x Laksa";
  orderA.extras = "more curry";
  
  orderArray[3] = orderA;


  //-----------------------------------------------------
}


//------- Loop functions ---------------------------------------------

void displayOrder(int index) {
  
  if (remainingOrders == 0)
  {
    LCD.clear();
    
    LCD.print("No orders!");    
    return;
  }
     
  LCD.clear();
  
  String line1 = orderArray[index].menuItem;
  
  LCD.print(line1.c_str());
  LCD.print((orderArray[index].extras).c_str());
  return;
}


int getCmd() {
  // 0 for order complete, 1 for previous, 2 for next, 3 for refresh, -1 for error
  char key = keypad.getKey();
  Serial.println(key);
  
  if (key != NO_KEY){
    if (key == '1' || key == '4' || key == '7')
      return prevCMD;
    else if (key == '2' || key == '5' || key == '8')
      return completeCMD;
    else if (key == '3' || key == '6' || key == '9')
      return nextCMD;
    else if (key == '*' || key == '0' || key == '#')
      return refreshCMD;
    else 
      return errorCMD;
  } else 
      return noCMD; 
}

//---------------------------------------------------------------------------------------



void loop()
{
  int cmd = getCmd();
  
  if (cmd != noCMD)  {
    switch (cmd)
    {
      case prevCMD:
        index--;
        break;
      case nextCMD:
        index++;
        break;
      case completeCMD:
        //send complete status to server 
        //get the updated array from the server;
        LCD.clear();
        LCD.print("Order Completed :");
        LCD.print((orderArray[index].menuItem).c_str());
        remainingOrders--;
        for (int i = index; i < remainingOrders; i++) {
          orderArray[i] = orderArray[i+1];
        }
        delay(1000);        
        index = 0;
        break;
      case refreshCMD:
        //update the array from the server
        LCD.clear();
        LCD.print("NEW: ");
        index = remainingOrders;
        orderArray[index].id = "555";
        orderArray[index].menuItem = "2x Laksa";
        orderArray[index].extras = "1 spicy|1 normal";
        remainingOrders++;
        LCD.print((orderArray[index].menuItem).c_str());
        LCD.print((orderArray[index].extras).c_str());
        delay (1000);
        
        index = 0;  
        break;
      case errorCMD:
        Serial.println("error reading input.");
        //display error on lcd
        index = -1;
        break;
    }
    if (index < 0)
      index = remainingOrders-1;
    else if (index >= remainingOrders)
      index = 0;
      
    displayOrder(index);
    
  }
  
}

//   // if there are incoming bytes available
//   // from the server, read them and print them:
//   if (client.available())
//   {
//     char c = client.read();
//     Serial.print(c);
//     LCD.print(c);
//   }
  

//   // if the server's disconnected, stop the client:
//   if (!client.available() && !client.connected())
//   {
//     Serial.println();
//     Serial.println("disconnecting.");
//     client.stop();
    
    

//     // do nothing forevermore:
//     for (;;)
//       ;
//   }
// }



