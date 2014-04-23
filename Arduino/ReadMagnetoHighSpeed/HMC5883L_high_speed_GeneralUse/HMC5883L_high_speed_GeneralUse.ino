#include <Wire.h>

#define MAG_ADDRESS ((char) 0x1E)
uint8_t mag_buffer[6];
int x,y,z; //triple axis data
int x_prev;

char xStr[6];
char xavgStr[6];

unsigned long time = 0;
unsigned long readTime = 0;
unsigned long lastUp = 0;

int SFmicro = 6250; // 6250 = 160Hz

#define BUFSIZE 500
#define DIFF 200 
int x_buffer[BUFSIZE];
long x_sum;
int x_avg;
int movAvgCounter = 0;
boolean x_Up = false;

int led = 13;
int out = 7;

void setup() {
  configOutput();
  configMag();
}

void configOutput() {
  delay(200);
  Serial.begin(115200);  
  Wire.begin();
  pinMode(led, OUTPUT);
  pinMode(out, OUTPUT);
}


void loop() {
  time = micros();

  if(readTime < time) {
    readTime += SFmicro;
    
    
    readMag();
    updateAverage(x);
    
    int dx = x-x_avg;
    
    if ( dx  > DIFF ) {

      if ( !x_Up) {
        
        long diff = time-lastUp;
        float samples = diff/ (float) SFmicro;
        float freq = 1000000/ (float) diff;
        Serial.print(samples); Serial.print(" "); Serial.println(freq);
        lastUp = time;
        
        digitalWrite(led, HIGH);
        digitalWrite(out, HIGH);
        x_Up = true;
      }
    }

    if (dx < -DIFF ) {

      if ( x_Up) {
        digitalWrite(led, LOW);
        digitalWrite(out, LOW);
        x_Up = false;
      }
    }
    
    x_prev = x;
    
    // update display
//    sprintf(xStr, "%5i", x);
//    sprintf(xavgStr, "%5i", x_avg);
//    
//    Serial.print("x: ");
//    Serial.println(xStr);
//    Serial.print(" xavg: ");
//    Serial.println(xavgStr);
    
  }
}


int interpolDelay(int dx_prev, int dx, int SFmicro) {
  
   int delayMicro = 0;
   
   if (dx_prev * dx < 0) { // should have opposing signs
     delayMicro = (long) SFmicro * dx_prev / (dx_prev-dx);
   } 
   
//   Serial.print(dx_prev);
//   Serial.print(" ");
//   Serial.print(dx);
//   Serial.print(" " );
//   Serial.print(SFmicro);
//   Serial.print(" ");
//   Serial.println(delayMicro);
   
   
   return delayMicro;

   
}

void updateAverage(int xval){

  x_sum -= x_buffer[movAvgCounter]; // first remove previous value from sum
  x_buffer[movAvgCounter] = xval; // update buffer value
  x_sum += x_buffer[movAvgCounter]; // add new buffer value
  x_avg = x_sum / BUFSIZE;

  if (movAvgCounter == BUFSIZE -1) {
    movAvgCounter = 0;
  } 
  else {
    movAvgCounter++;
  }

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
  x = (mag_buffer[0] << 8) | mag_buffer[1];
  y = (mag_buffer[2] << 8) | mag_buffer[3];
  z = (mag_buffer[4] << 8) | mag_buffer[5];

  // put the device back into single measurement mode
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write((byte) 0x02);
  Wire.write((byte) 0x01);
  Wire.endTransmission();
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
    Serial.print(mag_name, HEX); 
    Serial.println(" found, should be 0x48");
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
