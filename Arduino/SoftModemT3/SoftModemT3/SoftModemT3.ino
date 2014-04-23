#include "SoftModem.h"
#include "ctype.h"
SoftModem modem;

void setup() {
  Serial.begin(115200);
  modem.begin();
}

void loop(){
  

 while (modem.available ()) {
   int c = modem.read ();
   if (isprint (c)) {
     Serial.println ((char) c);
   }
   else {
     Serial.print ("(");
     Serial.print (c, HEX);
     Serial.println (")");      
   }
 }
 if (Serial.available ()) {
   modem.write (0xff);
   while (Serial.available ()) {
     char c = Serial.read ();
     modem.write (c);
   }
 } 
  
//  while(modem.available()){
//    int c = modem.read();
//    if (isprint(c)) {
//      Serial.print((char) c);
//    }
//    else {
//      Serial.print('(');
//      Serial.print(c, HEX);
//      Serial.print(')'); 
//    }
//  }
//  
//  while(Serial.available()) {
//    char c = Serial.read();
//    modem.write(c);
//  }
//  modem.write('2');
//  delay(2000);
}
