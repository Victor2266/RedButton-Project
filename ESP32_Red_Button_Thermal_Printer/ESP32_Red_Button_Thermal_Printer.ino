#include <Arduino.h>

#include "TPrinter.h"
#include <HardwareSerial.h> // or SoftwareSerial


const int printerBaudrate = 19200;  // or 19200 usually
const byte rxPin = 1;   // check datasheet of your board
const byte txPin = 0;   // check datasheet of your board
const int resetPin = 4;
const int buttonPin = 5;
byte butLst;
enum { None, SingleClick, DoubleClick };
const int ledPin = 6;
//const byte dtrPin = 4;   // if used

bool beginning{true};
bool operator_available{true}; //added to keep track of operator availablility 
String open_hours = "Later";
  
HardwareSerial mySerial(1);
Tprinter myPrinter(&mySerial, printerBaudrate);

void setup() {
  digitalWrite(resetPin, HIGH);
  pinMode(resetPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  //pinMode (buttonPin, INPUT_PULLUP);
  butLst = digitalRead (buttonPin);
  pinMode(buttonPin, OUTPUT);
  
  micros();
  Serial.begin(19200);
  Serial.println("\n\n [STARTED PROGRAM][ACTIVE] \n (Ignore the weird symbols above)");
  Serial.println("   Whatever you type into the terminal will be sent to the printer, \n   type /h for a list of commands you can do \n   don't use / in your messages because it is used for commands \n   You can type y or n to force an answer for testing purposes \n\n   Whenever you see > that means that message was sent to the printer\n   and = means it was their response \n");
  mySerial.begin(printerBaudrate, SERIAL_8N1, rxPin, txPin);  // must be 8N1 mode
  myPrinter.begin();

  //myPrinter.disableDtr();
  myPrinter.setHeat(9, 186, 2);
  myPrinter.autoCalculate(true);

  //myPrinter.identifyChars("ą ę");  // UTF-8
}

void loop() {
  if (beginning){
    beginning_sequence();
    beginning = false;
  }
  myPrinter.setMode(FONT_B);
  printFromSerial();  // open monitor and print something
}

void double_print(String str) {
  myPrinter.print(str);
  Serial.println("> " + str);
}
void double_println(String str) {
  myPrinter.println(str);
  Serial.println("> " + str);
}
void beginning_sequence() {
  //myPrinter.setMode(UPSIDE_DOWN);
  //myPrinter.println("DEFAULT FONT_A");
  wait_for_inital_press();

  myPrinter.setMode(DOUBLE_WIDTH, DOUBLE_HEIGHT);
  //myPrinter.feed(2);
  double_print("HEY!");

  myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println(" My name's Bill!");

  myPrinter.unsetMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println("and I've got a qustion for you,");
  myPrinter.feed(1);

  ask_if_they_can_vote(); //QUESTION #1

  double_println("In that case I've got another");
  double_println(" question for you,");
  
  myPrinter.feed(1);

  ask_if_they_can_talk(); //QUESTION #2

}
void ask_if_they_can_vote() {
  double_println("Are you old enough to vote?");

  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  double_println("(The legal age to vote in Canada is 18)");
  //myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);

  myPrinter.unsetMode(FONT_B);
  myPrinter.feed(2);
  
  bool valid_response = false;
  while (valid_response == false) {
    int response1 = wait_for_response();

    if (response1 == 1) {
      double_println("That's awesome! I wish I was old enough");
      myPrinter.feed(2);
      vTaskDelay(2000);
      valid_response = true;
    }
    else if (response1 == 2) {
      ask_if_voting_age_should_be_reduced();
      valid_response = true;

    }
    else if (response1 == 3) {
      double_println("The legal age to vote in Canada is 18, try answering again");
      myPrinter.feed(2);
    }
    else {
      double_print("Woah there you pressed the button ");
      double_print(String(response1));
      double_println(" times, try again");
      myPrinter.feed(2);
    }
  }
}
void ask_if_voting_age_should_be_reduced() {
  double_println("Do you think the voting age should be reduced so that you can vote?");
  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  myPrinter.unsetMode(FONT_B);
  myPrinter.feed(2);

  bool valid2 = false;
  while (valid2 == false) {
    int response2 = wait_for_response();
    if (response2 == 1) {
      double_println("Really!? but ya cant even drink!");
      myPrinter.feed(2);
      valid2 = true;
    }
    else if (response2 == 2) {
      double_println("Wow you're mature for your age");
      myPrinter.feed(2);
      valid2 = true;
    }
    else {
      double_print("Woah there you pressed the button ");
      double_print(String(response2));
      double_println(" times, try again");
      myPrinter.feed(2);
    }
  }
}

void ask_if_they_can_talk() {

  double_println("Are you comfortable talking into the microphone?");
  myPrinter.feed(1);
  double_println("Im a sentient AI and I'd like to have a conversation");
  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  myPrinter.unsetMode(FONT_B);
  myPrinter.feed(2);

  bool valid = false;
  while (valid == false) {
    int response = wait_for_response();
    if (response == 1) {
      double_println("Awesome! Try and speak loud and clear,");
      myPrinter.feed(2);
      valid = true;
    }
    else if (response == 2) {
      myPrinter.setMode(FONT_B);
      double_println("That's okay too! It was nice talking to ya");
      double_println("Please scan this QR code:");
      myPrinter.unsetMode(FONT_B);
      printQRcode();
        Serial.println("  [Rip Out This Reciept]");
        myPrinter.println("Rip Out This Reciept");
        myPrinter.println("--------------------------------");

      myPrinter.feed(3);
      reset_program();
    }
    else {
      double_print("Woah there you pressed the button ");
      double_print(String(response));
      double_println(" times, try again");
      myPrinter.feed(2);
    }
  }
}

int wait_for_response() {
  //say bye msg and then reset if timed out
  //otherwise return the number of consecutive button presses
  //include a serial override by typing y or n
  while (true) {
    vTaskDelay(1);

    while (Serial.available()) {
      char sign{};
      sign = (char)Serial.read();
      
      checkForCommand(sign);
      
      if (sign == 'y') {
        double_println("You responded with yes");
        Serial.flush();
        return 1;
      }
      else if (sign == 'n') {
        double_println("You responded with no");
        Serial.flush();
        return 2;
      }
    }
    int presses = check_for_button_presses();
    if (presses != 0){
      return presses;
    }

  }

}
int check_for_button_presses() {
  switch (chkButton ())  {
    case SingleClick:
      Serial.println ("[Button was pressed once]");
      return 1;

    case DoubleClick:
      Serial.println ("[Button was pressed twice]");
      return 2;
  }
  return 0;
}
bool wait_for_inital_press() {
  bool started = false;
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  int counter = 0;

  while (started == false) {
    if (digitalRead(buttonPin) == HIGH && operator_available) {
      started = true;
      digitalWrite(ledPin, LOW);
    }
    else if (digitalRead(buttonPin) == HIGH && operator_available == false){
      digitalWrite(ledPin, LOW);
      myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
      double_println("Sorry!");
      myPrinter.unsetMode(DOUBLE_WIDTH, DOUBLE_HEIGHT);
      double_println("Bill is on an errand try again " + open_hours);
      myPrinter.feed(2);
      vTaskDelay(2000);
    }
    vTaskDelay(1);
    while (Serial.available()) {
      char sign{};
      sign = (char)Serial.read();
      
      checkForCommand(sign);
      if (sign == 'y' || sign == 'n') {
        double_println("You forced the script to run");
        started = true;
        digitalWrite(ledPin, LOW);
        return true;
      }
    }
    counter++;
    if (counter == 150) {
      digitalWrite(8, !digitalRead(ledPin));
      counter = 0;
    }
  }
  return true;
}


void reset_program() {
  //RESET THE PROGRAM
  digitalWrite(4, LOW);//resetPin default = 4
}
int chkButton (void)
{
  const  unsigned long ButTimeout  = 800;
  static unsigned long msecLst;
  unsigned long msec = millis ();

  if (msecLst && (msec - msecLst) > ButTimeout)  {
    msecLst = 0;
    return SingleClick;
  }

  byte but = digitalRead (buttonPin);
  if (butLst != but)  {
    butLst = but;
    delay (10);           // **** debounce
    
    if (HIGH == but)  {   // press
      if (msecLst)  { // 2nd press
        msecLst = 0;
        return DoubleClick;
      }
      else
        msecLst = 0 == msec ? 1 : msec;
      
    }
  }

  return 0;
}
void printFromSerial() {
  // you can use it e.g for test;
  // require Serial.begin(baudrate) in void setup()
  myPrinter.wait();
  if (Serial.available()) {
    Serial.print("> ");
  }
  while (Serial.available()) {
    char sign{};
    sign = (char)Serial.read();

    checkForCommand(sign);
    if (sign != '/') {
      Serial.print(sign);
      myPrinter.print(sign);
    }

  }
}

void checkForCommand(char sign) {
  if (sign == '/') {
    Serial.println("");
    if (Serial.available()) {
      char nextChar{};
      nextChar = (char)Serial.read();
      Serial.println(nextChar);
      if (nextChar == 'h') {
        Serial.println("[This is a list of commands]");
        Serial.println("/h (Lists all commands)");
        Serial.println("/c XX:XXAM-XX:XXPM (sets the availability times)");
        Serial.println("   (write when you are coming back)");
        Serial.println("   (hard reset before using this)");
        Serial.println("/s (send thank you, closing message & QR code)");
        Serial.println("/r (hard reset program)");
        Serial.println("");
      }
      else if (nextChar == 's') {
        sendClosingMSG();
      } 
      else if (nextChar == 'c') {
        open_hours = "";
       
        while (Serial.available()) {
          char sign{};
          sign = (char)Serial.read();
          
          open_hours += sign;
        }
        if (open_hours.equals("\n")){
          open_hours = "later";
        }
        
        if (operator_available) {
          operator_available = false;
          beginning = true;
          Serial.println("[Program is set to INACTIVE]");
          Serial.print("you will be back: ");
          Serial.println(open_hours);
          Serial.println("Users will be sent to website and told availability hours)");
          
          return;
        }
        else {
          operator_available = true;
          Serial.println("[Program is set to ACTIVE]");
          Serial.println("Get ready to respond when users press the button");
        }
      }
      else if (nextChar == 'r') {
        digitalWrite(4, LOW);//resetPin default = 4
      }
      else if (nextChar = '\n') {
        Serial.println("[No command detected] (try /h for help)");
      }
    }
  }
}
void sendClosingMSG(){
  String end_msg = "It ws nice talking to ya,\nVisit www.futureofengagement.ca\n or Please scan this QR code:";
        Serial.println("> " + end_msg);
        myPrinter.print(end_msg);
        printQRcode();
        Serial.println("  [Rip Out This Reciept]");
        myPrinter.println("Rip Out This Reciept");
        myPrinter.println("--------------------------------");
        myPrinter.feed(2);
}

const uint8_t bitmapWidth = 56;
const uint8_t bitmapHeight = 57;

// link to repo
uint8_t qrcode[] = {
   // 'qr', 56x57px
0xff, 0xfc, 0x19, 0xfe, 0x1f, 0xbf, 0xff, 0xff, 0xfc, 0x19, 0xfe, 0x1f, 0xbf, 0xff, 0xc0, 0x0d, 
0x98, 0x7f, 0xe0, 0x30, 0x03, 0xc0, 0x0d, 0x98, 0x7f, 0xe0, 0x30, 0x03, 0xcf, 0xcc, 0x01, 0xe6, 
0x07, 0xb3, 0xf3, 0xcf, 0xcc, 0x01, 0xe6, 0x07, 0xb3, 0xf3, 0xcf, 0xcd, 0xf9, 0x9e, 0x66, 0x33, 
0xf3, 0xcf, 0xcd, 0xf9, 0x9e, 0x66, 0x33, 0xf3, 0xcf, 0xcc, 0x60, 0x07, 0x9f, 0xb3, 0xf3, 0xcf, 
0xcc, 0x60, 0x07, 0x9f, 0xb3, 0xf3, 0xc0, 0x0d, 0xe7, 0x80, 0x19, 0xb0, 0x03, 0xc0, 0x0d, 0xe7, 
0x80, 0x19, 0xb0, 0x03, 0xff, 0xfd, 0x99, 0x99, 0x99, 0xbf, 0xff, 0xff, 0xfd, 0x99, 0x99, 0x99, 
0xbf, 0xff, 0x00, 0x00, 0x7e, 0x19, 0x81, 0x80, 0x00, 0x00, 0x00, 0x7e, 0x19, 0x81, 0x80, 0x00, 
0xff, 0xcf, 0xe6, 0x00, 0x60, 0x4c, 0xcc, 0xff, 0xcf, 0xe6, 0x00, 0x60, 0x4c, 0xcc, 0xc0, 0x00, 
0x19, 0xfe, 0x1f, 0xff, 0x03, 0xc0, 0x00, 0x19, 0xfe, 0x1f, 0xff, 0x03, 0x00, 0xcc, 0x18, 0x7f, 
0xe0, 0x00, 0x00, 0x00, 0xcc, 0x18, 0x7f, 0xe0, 0x00, 0x00, 0xcf, 0x00, 0x01, 0xe1, 0x87, 0xfc, 
0xcc, 0xcf, 0x00, 0x01, 0xe1, 0x87, 0xfc, 0xcc, 0xcc, 0xcd, 0xf9, 0x98, 0x60, 0x00, 0xf0, 0xcc, 
0xcd, 0xf9, 0x98, 0x60, 0x00, 0xf0, 0xff, 0x01, 0x80, 0x00, 0x1f, 0xb3, 0x03, 0xff, 0x01, 0x80, 
0x00, 0x1f, 0xb3, 0x03, 0xf3, 0xce, 0x67, 0x87, 0x81, 0x8c, 0xf0, 0x33, 0xc2, 0x06, 0x1e, 0x07, 
0x8f, 0x0c, 0x33, 0xc2, 0x06, 0x1e, 0x07, 0x8f, 0x0c, 0x00, 0x3d, 0xfe, 0x06, 0x66, 0x0c, 0xf0, 
0x00, 0x3d, 0xfe, 0x06, 0x66, 0x0c, 0xf0, 0xc0, 0xf0, 0x79, 0xf9, 0xff, 0xbf, 0x33, 0xc0, 0xf0, 
0x79, 0xf9, 0xff, 0xbf, 0x33, 0xcf, 0x0f, 0xe0, 0x7f, 0x81, 0x8c, 0x30, 0xcf, 0x0f, 0xe0, 0x7f, 
0x81, 0x8c, 0x30, 0xcc, 0x30, 0x79, 0xe0, 0x01, 0xcf, 0x0c, 0xcc, 0x30, 0x79, 0xe0, 0x01, 0xcf, 
0x0c, 0xcc, 0xcf, 0x99, 0x9e, 0x61, 0xff, 0x3f, 0xcc, 0xcf, 0x99, 0x9e, 0x61, 0xff, 0x3f, 0x00, 
0x01, 0xf8, 0x01, 0x99, 0x83, 0xff, 0x00, 0x01, 0xf8, 0x01, 0x99, 0x83, 0xff, 0xff, 0xfd, 0x87, 
0x86, 0x07, 0xb3, 0xf0, 0xff, 0xfd, 0x87, 0x86, 0x07, 0xb3, 0xf0, 0xc0, 0x0c, 0x66, 0x19, 0x87, 
0x83, 0x0f, 0xc0, 0x0c, 0x66, 0x19, 0x87, 0x83, 0x0f, 0xcf, 0xcd, 0xfe, 0x00, 0x61, 0xff, 0x3c, 
0xcf, 0xcd, 0xfe, 0x00, 0x61, 0xff, 0x3c, 0xcf, 0xcd, 0xe1, 0xf8, 0x79, 0x8c, 0xff, 0xcf, 0xcd, 
0xe1, 0xf8, 0x79, 0x8c, 0xff, 0xcf, 0xcd, 0x98, 0x7e, 0x60, 0x3f, 0xfc, 0xcf, 0xcd, 0x98, 0x7e, 
0x60, 0x3f, 0xfc, 0xc0, 0x0d, 0xe1, 0xe7, 0x9f, 0x8c, 0xcc, 0xc0, 0x0d, 0xe1, 0xe7, 0x9f, 0x8c, 
0xcc, 0xff, 0xfd, 0xf9, 0x9f, 0xe6, 0x0f, 0xf0, 0xff, 0xfd, 0xf9, 0x9f, 0xe6, 0x0f, 0xf0};

void printQRcode (){
  myPrinter.printBitmap(qrcode, bitmapWidth, bitmapHeight, 4);
}
