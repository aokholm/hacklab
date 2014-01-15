#include <Wire.h>

#define MAG_ADDRESS ((char) 0x1E)
uint8_t mag_buffer[6];
int16_t mag_raw[3];

long start_time = 0;
long previous_time = 0;
long loop_duration = 0;

volatile boolean isDataReady = false;

void setup() {
//  Serial.begin(115200);
  Serial.begin(9600);

  Serial.println("Initializing...");
  
  Wire.begin();

  configMag();
  
  attachInterrupt(0, magDataReady, RISING);
  
  previous_time = micros();
}


//interrupt function when mag DRDY pin is brought LOW
void magDataReady() {
  isDataReady = true;
}


void loop() {

  start_time = micros();
  loop_duration = start_time - previous_time;
  
  if(loop_duration >= 6250) { // 6250 = 160Hz
    if(isDataReady) {
      isDataReady = false;
      readMag();
  
      Serial.println(mag_raw[0], DEC); //Serial.print(",");
//      Serial.print(mag_raw[1], DEC); Serial.print(",");
//      Serial.print(mag_raw[2], DEC); Serial.println();
      previous_time = start_time;
    
    }
    else {
      Serial.println("Missed one");
    }
  }
}

int mag_rawToMicroTesla(int raw_value) {
  return raw_value * 100; // devide by 820 for 0x40, 1090 for 0x20
  // timed by 100, since 1000 Gauss pr Tesla - showed in microTesla
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
  // higher range +/- 190 uT (0x40)
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x01);
  Wire.write((byte) 0x40);
  Wire.endTransmission();
  delay(5);
  
  // Register 0x02: MODE
  // continuous measurement mode at configured ODR (0x00)
  // possible to achieve 160 Hz by using single measurement mode (0x01) and DRDY
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x02);
  Wire.write((byte) 0x01);
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
  
  // put the device back into single measurement mode
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x02);
  Wire.write((byte) 0x01);
  Wire.endTransmission();
  
}
