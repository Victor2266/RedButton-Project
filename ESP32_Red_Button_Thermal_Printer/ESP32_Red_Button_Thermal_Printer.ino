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
  myPrinter.setHeat(9,186,2);
  myPrinter.autoCalculate(true);
  
  //myPrinter.setMode(UPSIDE_DOWN);
  //myPrinter.println("DEFAULT FONT_A");
  myPrinter.setMode(DOUBLE_WIDTH, DOUBLE_HEIGHT);
  myPrinter.feed(2);
  double_print("HEY!");
  
  myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println(" My name's Bill!");
  
  myPrinter.unsetMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  double_println("and I've got a qustion for you,");
  myPrinter.feed(1);
  double_println("Are you old enough to vote?");

  myPrinter.setMode(FONT_B);
  double_println("Press once for yes and twice for no");
  //myPrinter.setMode(FONT_B, DOUBLE_WIDTH, DOUBLE_HEIGHT);
  
  myPrinter.feed(2);
  //myPrinter.identifyChars("ą ę");  // UTF-8
}

void loop() {
  myPrinter.setMode(FONT_B);
  myPrinter.printFromSerial();  // open monitor and print something
}

void double_print(String str){
  myPrinter.print(str);
  Serial.println("> "+ str);
}
void double_println(String str){
  myPrinter.println(str);
  Serial.println("> " + str);
}
