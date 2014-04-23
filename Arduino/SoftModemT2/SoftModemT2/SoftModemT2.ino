#include "SoftModem.h"
#include "ctype.h"
SoftModem modem;

int ledPin = 13; // choose the pin for the LED
int inPin = 7;   // choose the input pin (for a pushbutton)
int val = 0;     // variable for reading the pin status

void setup() {
  pinMode(ledPin, OUTPUT);  // declare LED as output
//  pinMode(inPin, INPUT);    // declare pushbutton as input
  modem.begin();
}

void loop(){
  
  
//  val = digitalRead(inPin);  // read input value
  
//  if (val == HIGH) {         // check if the input is HIGH (button released)
//    digitalWrite(ledPin, LOW);  // turn LED OFF
//  } else {
  if (true){
     // We have a heart beat:
     char h = '1';
     //modem.write(h);
     digitalWrite(ledPin, HIGH);
     delay(100);
     digitalWrite(ledPin, LOW);
     delay(400);
  }
}
