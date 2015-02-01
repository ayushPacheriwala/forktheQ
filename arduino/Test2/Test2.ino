           
#include <Keypad.h>
#include <GSM.h>
#include <hardSerialLCD.h>

                                                                                                          
#include "Arduino.h"
void lcdSetup();
void displayMsg(String dispMsg);
void setup();
void displayOrder(int index);
int getCmd();
void loop();
void retreiveOrders();
#line 27
const byte ROWS = 4;             
const byte COLS = 3;                 
                    
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
                                                                  
byte rowPins[ROWS] = { 37,43,41,39};
                                                            
byte colPins[COLS] = { 30,28,32}; 

                    
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

                                                                                                               

                                                                          

const int nextCMD = 2;
const int prevCMD = 1;
const int completeCMD = 0;
const int refreshCMD = 3;
const int errorCMD = -1;
const int noCMD = -2;

                                                                          

                                                                                                                
               
  #define PINNUMBER ""

             
  #define GPRS_APN       "internet"                         
  #define GPRS_LOGIN     ""                                   
  #define GPRS_PASSWORD  ""                                   

                                    
  GSMClient client;
  GPRS gprs;
  GSM gsmAccess;

                                               
  char server[] = "forktheq.karankamath.com";
  char path[] = "/getorders?num=2";
  int port = 80;                                   

                                                                                                                    

                                                                                                                    
  HardSerialLCD LCD;
                                                                                                                    


                                                                                                         
  typedef struct {
    String id;
    String menuItem;
    String extras; 
  } order;

  order orderArray[10]; 
  int index;

  int remainingOrders;
  void retreiveInfo(order array[]);
                                                                                                           

                                                                                                                     

  void lcdSetup() {
    LCD.begin();
    delay(10);
    LCD.backlight();
    delay(10);
    LCD.clear();
    delay(10);
    char temp2[100] = "hello";
    delay(10);
    LCD.print(temp2);
  }
  
  void displayMsg(String dispMsg) {
    Serial.println(dispMsg);
    LCD.clear();
    delay(10);
    LCD.print(dispMsg.c_str());    
  }
  
                                                                                                                       
void setup()
{
                                                                
  Serial.begin(9600);
  lcdSetup();

  displayMsg("Starting Fork the Q web client.");
                     
  boolean notConnected = true;
  
                                              
                                                                           
  while (notConnected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
    {
      displayMsg("Not connected");
      delay(1000);
    }
  }

  displayMsg("connecting...");

                                                     
  if (client.connect(server, port))
  {
    displayMsg("connected");
                           
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
                                                    
    displayMsg("connection failed");
  }

  index = 0;
  remainingOrders = 0;  
                                                           
}


                                                                      

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
                                         
                                                
       LCD.clear();
       LCD.print("Order Completed :");
                            
       index = 0;
       break;
       case refreshCMD:
                                          
        LCD.clear();
        LCD.print("NEW: ");
        retreiveOrders();
        delay (1000);
        
        index = 0;  
        break;
      case errorCMD:
        Serial.println("error reading input.");
                              
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

  

void retreiveOrders(){
    displayMsg("refreshing...");
    String temp = "";
    String buffer[100];
    int line = 1;
    order orderA;
    
   while (client.available())
  {
    char c = client.read();
                      
    if (c != '\n')
        temp += c;
    else {                        
      buffer[line-1] = temp;
      temp = "";
      line++;
    }                        
  }
    Serial.println(line);
    for (int i = 10; i < line; i++) {
      Serial.println(buffer[i]);
  }
//    orderA.id = buffer[10];
//    orderA.menuItem = buffer[11];
//    orderA.extras = buffer[12];
    
  int oldRemaining = remainingOrders; 
  if((line-12)/3)>0)
    remainingOrders = oldRemaining + (line-12)/3;
 
 
  for (int j = 0; j < remainingOrders; j++)
  {
    orderA.id = buffer[10+3*j];
    orderA.menuItem = buffer[11+3*j];
    orderA.extras = buffer[12+3*j];  
    orderArray[oldRemaining+j] = orderA;
  }  
  Serial.println("new orders: ");
  Serial.println(orderArray[0].id);
  Serial.println(orderArray[0].menuItem);
  
  delay(10);
  
  Serial.println("DONE");
  delay(10);

}





