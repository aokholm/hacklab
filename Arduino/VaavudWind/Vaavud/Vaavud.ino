/*
An Arduino code example for interfacing with the HMC5883

by: Jordan McConnell
 SparkFun Electronics
 created on: 6/30/11
 license: OSHW 1.0, http://freedomdefined.org/OSHW

Analog input 4 I2C SDA
Analog input 5 I2C SCL
*/



#include <Wire.h> //I2C Arduino Library

#define address 0x1E //0011110b, I2C 7bit address of HMC5883
#define HMC5883_WriteAddress 0x1E //  i.e 0x3C >> 1
#define HMC5883_ConfigurationRegisterA 0x00
#define HMC5883_FastDataOutPutCommand 0x18
#define HMC5883_ModeRegisterAddress 0x02
#define HMC5883_ContinuousModeCommand 0x00
#define HMC5883_DataOutputXMSBAddress  0x03


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


int x,y,z; //triple axis data


void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  
  // Register 0x00: CONFIG_A
  // 75 Hz ODR (0x18)
  Wire.beginTransmission(HMC5883_WriteAddress);
  Wire.write(HMC5883_ConfigurationRegisterA);
  Wire.write(HMC5883_FastDataOutPutCommand);
  Wire.endTransmission();
  delay(5);
  
  // Register 0x02: MODE
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(HMC5883_WriteAddress); //open communication with HMC5883
  Wire.write(HMC5883_ModeRegisterAddress); //select mode register
  Wire.write(HMC5883_ContinuousModeCommand); //continuous measurement mode
  Wire.endTransmission();
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

}

void loop(){
    //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883_WriteAddress);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  Wire.requestFrom(HMC5883_WriteAddress, 6);
 
 //Read data from each axis, 2 registers per axis
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
    
      //Print out values of each axis
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
  
  lcd.setCursor(0, 1);
  lcd.print(x);
  
  }
  
  

  
  delay(14);
}
