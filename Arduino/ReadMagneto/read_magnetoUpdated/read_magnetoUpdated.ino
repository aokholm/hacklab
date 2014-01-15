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

#define MAG_ADDRESS ((char) 0x1E)
uint8_t mag_buffer[6];
int16_t mag_raw[3];

void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(115200);
  Serial.println("Initializing...");
  Wire.begin();
  
  configMag();
}

void loop(){
  
  readMag();
  
  //Print out values of each axis
  Serial.print("x: ");
  Serial.print(mag_raw[0]);
  Serial.print("  y: ");
  Serial.print(mag_raw[1]);
  Serial.print("  z: ");
  Serial.println(mag_raw[2]);
  
  delay(10);
}


void configMag() {
  uint8_t mag_name;
  
  // make sure that the device is connected
  Wire.beginTransmission(MAG_ADDRESS); 
  Wire.write((byte) 0x0A); // Identification Register A
  Wire.endTransmission();
  
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.requestFrom(MAG_ADDRESS, 1);
  mag_name = Wire.read();
  Wire.endTransmission();
  
  if(mag_name != 0x48) {
    Serial.println("HMC5883L not found!");
    Serial.print(mag_name, HEX); Serial.println(" found, should be 0x48");
    delay(1000);
  }
  
  // Register 0x00: CONFIG_A
  // normal measurement mode (0x00) and 75 Hz ODR (0x18)
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x00);
  Wire.write((byte) 0x18);
  Wire.endTransmission();
  delay(5);
  
  // Register 0x01: CONFIG_B
  // default range of +/- 130 uT (0x20)
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x01);
  Wire.write((byte) 0x20);
  Wire.endTransmission();
  delay(5);
  
  // Register 0x02: MODE
  // continuous measurement mode at configured ODR (0x00)
  // possible to achieve 160 Hz by using single measurement mode (0x01) and DRDY
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x02);
  Wire.write((byte) 0x00);
  Wire.endTransmission();
  
  delay(200);
  
}

// read 6 bytes (x,y,z magnetic field measurements) from the magnetometer
void readMag() {
  
  // multibyte burst read of data registers (from 0x03 to 0x08)
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x03); // the address of the first data byte
  Wire.endTransmission();
  
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.requestFrom(MAG_ADDRESS, 6);  // Request 6 bytes
  int i = 0;
  while(Wire.available())
  { 
    mag_buffer[i] = Wire.read();  // Read one byte
    i++;
  }
  Wire.read();
  Wire.endTransmission();
  
  // combine the raw data into full integers (HMC588L sends MSB first)
  //           ________ MSB _______   _____ LSB ____
  mag_raw[0] = (mag_buffer[0] << 8) | mag_buffer[1];
  mag_raw[1] = (mag_buffer[2] << 8) | mag_buffer[3];
  mag_raw[2] = (mag_buffer[4] << 8) | mag_buffer[5];
  
}

