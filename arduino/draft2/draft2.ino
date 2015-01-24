// libraries
#include <Keypad.h>
// include the library code:
#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must

#include <GSM.h>


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

//lcd, keypad init




const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[ROWS] = {30, 31, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6}; //connect to the column pinouts of the keypad

const int DISP_ROWS = 2;
const int DISP_COL = 16;

// initialize the library
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
SerialLCD slcd(11,12);//this is a must, assign soft serial pins

//========================================================================

const int nextCMD = 2;
const int prevCMD = 1;
const int completeCMD = 0;
const int refreshCMD = 3;
const int errorCMD = -1;
const int noCMD = -2;

//________________________________________________________________________



typedef struct {
  String id;
  String menuItem;
  String extras; 
} order;

order orderArray[10]; 
int index;

int remainingOrders;
void retreiveOrders();

void setup() {
  // set up
  Serial.begin(9600);            //for debugging
  slcd.begin();
  slcd.backlight();
  // Print a message to the LCD.
  slcd.print("Hello, Hawker!");
  slcd.display();
  delay(1000);
  index = 0;
  
  //pull orders from cloud.
  //displayOrders(index);
  remainingOrders = 5;
  
    Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  
  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  //testing code
  
//  order orderA;
//  
//  orderA.id = "000";
//  orderA.menuItem = "Chicken Rice";
//  orderA.extras = "no sauce";
//  
//  orderArray[0] = orderA;
//  
//  orderA.id = "101";
//  orderA.menuItem = "Beef Rice";
//  orderA.extras = "no sauce";
//  
//  orderArray[1] = orderA;
//  
//  orderA.id = "202";
//  orderA.menuItem = "Pizza";
//  orderA.extras = "no shrimp";
//  
//  orderArray[2] = orderA;
//  
//  orderA.id = "303";
//  orderA.menuItem = "Prata";
//  orderA.extras = "no curry";
//  
//  orderArray[3] = orderA;
//  
//  orderA.id = "404";
//  orderA.menuItem = "White Rice";
//  orderA.extras = "";
//  
//  orderArray[4] = orderA;
//  
  
  //-----------------------------------------------------
}

//===========================================================================

void emptyDisplay() {
  slcd.setCursor(0,0);
  slcd.print("                  ");
  slcd.setCursor(0,1);
  slcd.print("                  ");  
  return; 
}

void displayOrder(int index) {
  
  if (remainingOrders == 0)
  {
    emptyDisplay();
    slcd.setCursor(0,0);
    slcd.print("No orders!");    
    return;
  }
     
  emptyDisplay();
  
  String line1 = orderArray[index].menuItem;
  
  slcd.setCursor(0,0);
  slcd.print(line1.c_str());
  slcd.setCursor(0,1);
  slcd.print((orderArray[index].extras).c_str());
    
  return;
}
//___________________________________________________________________________



void loop() {  
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
        index = 0;
        break;
      case refreshCMD:
        //update the array from the server
        retreiveOrders();
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

int getCmd() {
  // 0 for order complete, 1 for previous, 2 for next, 3 for refresh, -1 for error
  char key = keypad.getKey();
  
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


void retreiveOrders(){
  
  Serial.println("connecting...");
  char orderPath[] = "/getorders?num=2";
  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(orderPath);
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
  
    String temp = "";
    String buffer[10];
    
    int count = 0;
    int line = 1;
    while (client.available()){
      char c = client.read();
      if (c != '\n')
        temp += c;
      else {
        buffer[line-1] = temp;
        temp = "";
        line++;
      }
    }
    
    remainingOrders = line/3;
    
    for (int i = 0; i < remainingOrders; i++) {
      order orderA; 
      
      orderA.id = buffer[i*3];
      orderA.menuItem = buffer[i*3+1];
      orderA.extras = buffer[i*3+2];
      
      orderArray[i] = orderA;
    }
    
    
//  }
//  String buffer[10] = {""};
//  int num = 0;
//  for( int i = 0; i<count; i++){
//    if(temp[i] != '\n'){
//      buffer[num] += temp[i];
//    }else{
//      num ++;
//    }
//    if (num%3 = 0)
//  }
//  for (int j = 0; j<2; j++){
//  order orderA;
//  
//  orderA.id = buffer[j];
//  orderA.menuItem = bufffer[j+1];
//  orderA.extras = buffer[j+2];
//  
//  orderArray[j] = orderA;
//  }
//   

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
}
