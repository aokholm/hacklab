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
int tick;
long ticks;
long timediff;

#define BUFSIZE 300
#define DIFF 200 
int x_buffer[BUFSIZE];
long x_sum;
int x_avg;
int movAvgCounter = 0;
boolean x_Up = false;


unsigned long freqTime;
unsigned long freqTimeLast;
unsigned long startTime;

char maxStr[5];
char avgStr[5];
char actualStr[6];

char xStr[6];
char yStr[6];
char zStr[6];
char tickStr[6];
char ticksStr[6];
char timeStr[6];

float freq;

float maxi = 0.0;
float avg = 0.0;
float actual = 0.0;

int led = 13;

boolean usb = true;

void setup(){
  
  // check if 5 volt USB or 12 volt external power suply is connected
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  
  if (voltage < 1.2 ) {
    usb = true;
  }
  else {
    usb = false;
  }
  pinMode(led, OUTPUT);
  
  //Initialize Serial and I2C communications
  if (usb) {
    digitalWrite(led, HIGH);
    Serial.begin(115200);
  }
  else {
    digitalWrite(led, LOW);
  }
  
  
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
  
  reset();
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("max:--.- av:--.-");
  lcd.setCursor(0,1);
  lcd.print("actual:--.-- m/s");
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
    
    
  }
    
  if ( x-x_avg  >= DIFF ) {
    
    if ( !x_Up) {
      tick++;
      ticks++;
      lcd.setCursor(0,0);
      lcd.print("-");
    }
    x_Up = true;
  }
  
  if (x-x_avg < DIFF ) {
    
    if ( x_Up) {
      tick++;
      ticks++;
      lcd.setCursor(0,0);
      lcd.print("|");
    }
    x_Up = false;
  }
  
  freqTime = micros();
  timediff = freqTime - freqTimeLast;
  
  if (timediff > 5000000) { // needs to do 2 rotations pr 5 seconds or it will reset
    reset();
  }
  
  if (tick >= 16) { // (8 ticks)
    freq = 8000000 / (float) timediff; // 2 rotations times 1000000 (micro s to s)
    updateInfo(freq);
    
    freqTimeLast = freqTime; 
    tick-=16;
  }
  
  updateAverage(x);
  
  if (usb) {
    //Print out values of each axis
    
    sprintf(xStr, "%5i", x);
    sprintf(yStr, "%5i", x_avg);
    sprintf(zStr, "%5i", z);
    sprintf(tickStr, "%3i", tick);
    sprintf(ticksStr, "%5i", ticks);
    sprintf(timeStr, "%5i", ((micros() - startTime)/1000));
    
    Serial.print("x: ");
    Serial.print(xStr);
    Serial.print("  y: ");
    Serial.print(yStr);
    Serial.print("  z: ");
    Serial.print(zStr);
    Serial.print(" tick: ");
    Serial.print(tickStr);  
    Serial.print(" ticks: ");
    Serial.print(ticksStr);
    Serial.print(" time: ");
    Serial.println(timeStr);    
    
  }
  delay(14);
}


void reset() {
  startTime = micros();
  freqTimeLast = micros();
  
  tick = 0;
  ticks = 0;
  maxi = 0;
  
    // set up the LCD's number of columns and rows: 
  lcd.setCursor(7,1);
  lcd.print("--.--");
}

void updateAverage(int xval){
  
  x_sum -= x_buffer[movAvgCounter]; // first remove previous value from sum
  x_buffer[movAvgCounter] = xval; // update buffer value
  x_sum += x_buffer[movAvgCounter]; // add new buffer value
  x_avg = x_sum / BUFSIZE;
  
  if (movAvgCounter == BUFSIZE -1) {
    movAvgCounter = 0;
  } else {
    movAvgCounter++;
  }
  
}

void updateInfo(float freq) {
  if (freq > maxi) {
    maxi = freq;
  }
  actual = freq;
  
  // 4 ticks pr revolution
  avg = ticks / (float) (4 * (micros() - startTime)) * 1000000;
  
  // display
  // max:00.0 av:00.0
  // actual: 00.0 m/s
    
  dtostrf(frequencyToWindspeed(maxi), 4, 1, maxStr);
  dtostrf(frequencyToWindspeed(avg), 4, 1, avgStr);
  dtostrf(frequencyToWindspeed(actual), 5, 2, actualStr);
  
  // print to LCD
  lcd.setCursor(4, 0);
  lcd.print(maxStr);
  
  lcd.setCursor(12, 0);
  lcd.print(avgStr);
  
  lcd.setCursor(7, 1);
  lcd.print(actualStr);
}

float frequencyToWindspeed(float freq) {
  return freq;
  //return 0.238 + 0.8910363 * freq; // 90 deg
}
