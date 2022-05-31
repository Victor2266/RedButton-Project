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

  beginning_sequence();

  //myPrinter.identifyChars("ą ę");  // UTF-8
}

void loop() {
  myPrinter.setMode(FONT_B);
  myPrinter.printFromSerial();  // open monitor and print something
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

  double_println("In that case I've got another qustion for you,");

  myPrinter.feed(1);

  ask_if_they_can_talk(); //QUESTION #2

}
void ask_if_they_can_vote() {
  double_println("Are you old enough to vote?");

  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  double_println("(The legal age to vote in Canada is 18)");
  //myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);

  myPrinter.feed(2);

  myPrinter.unsetMode(FONT_B);

  bool valid_response = false;
  while (valid_response == false) {
    int response1 = wait_for_response();

    if (response1 == 1) {
      double_println("That's awesome! I wish I was old enough");
      myPrinter.feed(2);
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
      double_println("Really!? but you cant even drink yet!,");
      myPrinter.feed(2);
      valid2 = true;
    }
    else if (response2 == 2) {
      double_println("Wow you must be mature for your age");
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
  double_println("I'm a sentient AI and I'd like to have a conversation");
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
      double_println("That's okay too! It was nice talking to you. If you want more infomation try scanning this QR code:");
      myPrinter.print_QR();
      double_println("[Pull Down to rip out this reciept]");
      myPrinter.feed(2);
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
      
      myPrinter.checkForCommand(sign);
      
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
    if (digitalRead(buttonPin) == HIGH && myPrinter.operator_available) {
      started = true;
      digitalWrite(ledPin, LOW);
    }
    vTaskDelay(1);
    while (Serial.available()) {
      char sign{};
      sign = (char)Serial.read();
      
      myPrinter.checkForCommand(sign);
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
