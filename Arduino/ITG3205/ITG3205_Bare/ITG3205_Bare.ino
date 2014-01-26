//Ricardo Arturo Cabral <ing dot cabral dot mejia at gmail
#include <Wire.h>
#include "itg3200.h"
ITG3200 gyro;
void setup(){
  Serial.begin(9600);
  gyro.begin(0x68);
  delay(1000);
}
int cnt=0;
void loop(){
  Serial.println(gyro.readGyro());
  delay(100);
}
