// Andreas Okholm 9DOF SENSOR
// ITG3205
// HMC5883L
// ADXL345

#include <Wire.h>

#define GYRO_ADR (0x68)
#define MAG_ADR (0x1E)
#define ACC_ADR (0x53)

enum { MAG, ACC, GYRO };

byte buff[8];

char ACC_POWER_CTL = 0x2D;        //Power Control Register
char ACC_DATA_FORMAT = 0x31;
char ACC_DATAX0 = 0x32;        //X-Axis Data 0
char MAG_CFG_A = 0x00;
char MAG_CFG_B = 0x01;
char MAG_MODE = 0x02;
static const byte GYRO_REG_PWR_MGM=0x3E;
static const byte GYRO_REG_SMPLRT_DIV=0x15;
static const byte GYRO_REG_DLPF_FS=0x16;
static const byte GYRO_REG_INT_CFG=0x17;
static const byte GYRO_REG_TEMP_H=0x1B;

unsigned long mag_time;
unsigned long acc_time;
unsigned long gyro_time;
unsigned long time;

int baseDelta = 4000;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output. Make sure you set your Serial Monitor to the same!
  configAcc();
  configMag();
  configGyro();
  time = micros();
  mag_time = time+2000000;
  acc_time = time+2000000+baseDelta;
  gyro_time = time+2000000+baseDelta*3;
}

unsigned long tic;
unsigned long tock;

void loop()
{
  time = micros();
  
  if (mag_time < time) {
    mag_time += baseDelta*2;
//    tic = micros();
    readMag();
//    tock = micros();
//    Serial.print("mag: "); Serial.println(tock-tic, DEC);
  }
  
  if (acc_time < time) {
    acc_time += baseDelta*4;
//    tic = micros();
    readAccel();
//    tock = micros();
//    Serial.print("acc: "); Serial.println(tock-tic, DEC);
  }
  
  

  if (gyro_time < time) {
    gyro_time += baseDelta*4;
//    tic = micros();
    readGyro();
//    tock = micros();
//    Serial.print("gyro: "); Serial.println(tock-tic, DEC);
  }  
}

void configAcc() {
  //Put the ADXL345 into +/- 2G range by writing the value 0x00 to the DATA_FORMAT register.
  writeTo(ACC_ADR, ACC_DATA_FORMAT, 0x00);
  delay(5);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(ACC_ADR, ACC_POWER_CTL, 0x08);
  delay(5);
}


void configMag() {
  // Register 0x00: CONFIG_A
  // normal measurement mode (0x00) and 75 Hz ODR (0x18)
  writeTo(MAG_ADR,  MAG_CFG_A, 0x18);
  delay(5);
  // Register 0x01: CONFIG_B
  // default range of +/- 130 uT (0x20)
  // higher range +/- 190 uT (0x40)
  writeTo(MAG_ADR, MAG_CFG_B, 0x40);
  delay(5);
  // Register 0x02: MODE
  // continuous measurement mode at configured ODR (0x00)
  // possible to achieve 160 Hz by using single measurement mode (0x01) and DRDY
  writeTo(MAG_ADR, MAG_MODE, 0x01);
  delay(5);
}

void configGyro() {
  // Power Management
  writeTo(GYRO_ADR, GYRO_REG_PWR_MGM, 0);
  delay(5);
  // Sample rate divider
  writeTo(GYRO_ADR, GYRO_REG_SMPLRT_DIV, 0x63);
  delay(5);
  //Frequency select and digital low pass filter
  writeTo(GYRO_ADR, GYRO_REG_DLPF_FS, 0x1E);
  delay(5);
  //Interrupt configuration
  writeTo(GYRO_ADR, GYRO_REG_INT_CFG, 0);
  delay(5);
}


void print(unsigned long time, unsigned int device, int x, int y, int z) {
  Serial.print(time, DEC);  Serial.print(",");
  Serial.print(device, DEC);Serial.print(",");
  Serial.print(x,DEC);      Serial.print(",");
  Serial.print(y,DEC);      Serial.print(",");
  Serial.print(z,DEC);      Serial.print("\n");
}

void print(unsigned long time, unsigned int device, int x, int y, int z, int t) {
  Serial.print(time, DEC);  Serial.print(",");
  Serial.print(device, DEC);Serial.print(",");
  Serial.print(x,DEC);      Serial.print(",");
  Serial.print(y,DEC);      Serial.print(",");
  Serial.print(z,DEC);      Serial.print(",");
  Serial.print(t,DEC);      Serial.print("\n");
}

void readAccel() {
  uint8_t bytesToRead = 6;
  
  readFrom(ACC_ADR, ACC_DATAX0, bytesToRead, buff); //read the acceleration data from the ADXL345

  // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  // thus we are converting both bytes in to one int
  int x = (((int)buff[1]) << 8) | buff[0];   
  int y = (((int)buff[3]) << 8) | buff[2];
  int z = (((int)buff[5]) << 8) | buff[4];
  
  print(time, ACC, x, y, z);
}


void readMag() {
  readFrom(MAG_ADR, (byte) 0x03, 6, buff);
  
  // combine the raw data into full integers (HMC588L sends MSB first)
  //           ________ MSB _______   _____ LSB ____
  int x = (buff[0] << 8) | buff[1];
  int y = (buff[2] << 8) | buff[3];
  int z = (buff[4] << 8) | buff[5];
  
  print(time, MAG, x, y, z);

  // put the device back into single measurement mode
  writeTo(MAG_ADR, MAG_MODE, 0x01);
}

void readGyro() {
  readFrom(GYRO_ADR, GYRO_REG_TEMP_H, 8, buff);
  
  int t = (int)(buff[0] << 8 | buff[1]);
  int x = (int)(buff[2] << 8 | buff[3]);   
  int y = (((int)buff[4]) << 8) | buff[5];
  int z = (((int)buff[6]) << 8) | buff[7]; 
  print(time, GYRO, x, y, z, t);
}

void writeTo(byte device, byte address, byte val) {
  Wire.beginTransmission(device); // start transmission to device 
  Wire.write(address);             // send register address
  Wire.write(val);                 // send value to write
  Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); // start transmission to device 
  Wire.write(address);             // sends address to read from
  Wire.endTransmission();         // end transmission

  Wire.beginTransmission(device); // start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())         // device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read();    // receive a byte
    i++;
  }
  Wire.endTransmission();         // end transmission
}
