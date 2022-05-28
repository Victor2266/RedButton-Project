#include <Arduino.h>

#include "TPrinter.h"
#include <HardwareSerial.h> // or SoftwareSerial


const int printerBaudrate = 19200;  // or 19200 usually
const byte rxPin = 1;   // check datasheet of your board
const byte txPin = 0;   // check datasheet of your board
const int resetPin = 4;
//const byte dtrPin = 4;   // if used

HardwareSerial mySerial(1);
Tprinter myPrinter(&mySerial, printerBaudrate);

void setup() {
  digitalWrite(resetPin, HIGH);
  pinMode(resetPin, OUTPUT);
  micros();
  Serial.begin(19200);
  Serial.println("\n\n [STARTED PROGRAM] \n (Ignore the weird symbols above)");
  Serial.println("   Whatever you type into the terminal will be sent to the printer, \n   type /h for a list of commands you can do \n   don't use / in your messages because it is used for commands \n\n   Whenever you see > that means that message was sent to the printer\n   and = means it was their response \n");
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
  myPrinter.setMode(DOUBLE_WIDTH, DOUBLE_HEIGHT);
  //myPrinter.feed(2);
  double_print("HEY!");

  myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println(" My name's Bill!");

  myPrinter.unsetMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println("and I've got a qustion for you,");
  myPrinter.feed(1);
  double_println("Are you old enough to vote?");

  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no or three times if you don't know the voting age");
  //myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);

  myPrinter.feed(2);

  myPrinter.println("DEFAULT FONT_A");

  bool valid_response = false;
  while (valid_response == false) {
    int response1 = wait_for_response();
    
    if (response1 == 1) {
      double_println("That's awesome! I wish I was old enough");
      myPrinter.feed(2);
      valid_response = true;
    }
    else if (response1 == 2) {
      double_println("Do you think the voting age should be reduced so that you can vote?");
      double_println("Press once for yes and twice for no");
      myPrinter.feed(2);

      bool valid2 = false;
      while(valid2 == false){
        int response2 = wait_for_response();
        if (response2 == 1) {
          double_println("Really!? but you cant even drink yet!,");
          myPrinter.feed(2);
          valid2 = true;
        } 
        else if (response2 == 2){
          double_println("Wow you must be mature for your age");
          myPrinter.feed(2);
          valid2 = true;
        }
        else{
          double_println("Woah there you pressed the button " + response2 + " times, try again");
          myPrinter.feed(2);
        }
      }
      valid_response = true;
      
    }
    else if (response1 == 3){
      double_println("The legal age to vote in Canada is 18, try answering again");
      myPrinter.feed(2);
    }
    else if (response1 == -1) {
      //RESET THE PROGRAM
      digitalWrite(4, LOW);//resetPin default = 4
    }
    else {
      double_println("Woah there you pressed the button " + response1 + " times, try again");
      myPrinter.feed(2);
    }
  }
  
  double_println("In that case I've got another qustion for you,");

  myPrinter.feed(1);
  double_println("Are you comfortable talking into the microphone?");

  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  //myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);

  myPrinter.feed(2);

  wait_for_response();
}
void wait_for_response() {
  //return -1 for timed out
  //otherwise return the number of consecutive button presses
}
