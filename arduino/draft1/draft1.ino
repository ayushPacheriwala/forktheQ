//lcd, keypad init

// include the library code:
#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must
#include <Keypad.h>


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[ROWS] = {10, 13, 8, 9}; //connect to the row pinouts of the keypad
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
void retreiveInfo(order array[]);

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
  remainingOrders = 4;
  
  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  //testing code
  
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
  
//---------------------------------------------------------------------------
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
        emptyDisplay();
        slcd.setCursor(0,0);
        slcd.print("Order Completed :");
        slcd.setCursor(0,1);
        slcd.print((orderArray[index].menuItem).c_str());
        remainingOrders--;
        for (int i = index; i < remainingOrders; i++) {
          orderArray[i] = orderArray[i+1];
        }
        delay(1000);        
        index = 0;
        break;
      case refreshCMD:
        //update the array from the server
        emptyDisplay();
        slcd.setCursor(0,0);
        slcd.print("NEW: ");
        index = remainingOrders;
        orderArray[index].id = "555";
        orderArray[index].menuItem = "2x Laksa";
        orderArray[index].extras = "1 spicy|1 normal";
        remainingOrders++;
        slcd.print((orderArray[index].menuItem).c_str());
        slcd.setCursor(0,1);
        slcd.print((orderArray[index].extras).c_str());
        delay (1000);
//        emptyDisplay();
//        slcd.setCursor(0,0);
//        slcd.print("NEW: ");
//        index = remainingOrders;
//        orderArray[index].id = "006";
//        orderArray[index].menuItem = "1x Chicken Rice";
//        orderArray[index].extras = "less rice";
//        remainingOrders++;
//        slcd.print((orderArray[index].menuItem).c_str());
//        slcd.setCursor(0,1);
//        slcd.print((orderArray[index].extras).c_str());
//        delay (1000);
        
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
