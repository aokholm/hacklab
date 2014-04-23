/*
An Arduino code example for interfacing with the HMC5883

by: Jordan McConnell
 SparkFun Electronics
 created on: 6/30/11
 license: OSHW 1.0, http://freedomdefined.org/OSHW


*/

int inPin = 7;
                       // outside leads to ground and +5V
int val = 0;           // variable to store the value read
int oldVal = 0;
int ledPin = 13;

#define resetTime 2000000


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


unsigned long ticks;
unsigned long timediff;

unsigned long time;
unsigned long timeLast;
unsigned long startTime;

char maxStr[5];
char avgStr[5];
char actualStr[5];

char valStr[6];
char ticksStr[6];
char timeStr[6];

float freq;

float maxi = 0.0;
float avg = 0.0;
float actual = 0.0;

void setup(){
  
  Serial.begin(115200);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("max:--.- av:--.-");
  lcd.setCursor(0,1);
  lcd.print("actual: --.- m/s");
  
  oldVal = digitalRead(inPin);
  pinMode( ledPin, OUTPUT);
  
}

void loop(){
  
  // estimate tick // each tick resembles 1/2 rotation
  
  val = digitalRead(inPin);
  time = micros();
  timediff = time - timeLast;
  
  if (oldVal != val) { // when input changes (2 times per revolusion)
    
    if (ticks == 0) {
     reset(time);
    }
    
    if (ticks == 1) {
      Serial.println("START");
    }
    
    if (ticks > 0) {
      freq = 500000 / (float) timediff; // 1/2 rotations times 1000000 (micro s to s)
      
      // to Android
      Serial.print(time - startTime);
      Serial.print(",");
      Serial.println(freq);
      
      updateInfo(freq);
      timeLast = time; 
      
    }
    oldVal = val;
    digitalWrite(ledPin, val);
    ticks++;
  }
  
  else {
    
    if (timediff > resetTime ) {
      if (ticks == 1) {
        ticks = 0;
      }
      
      if (ticks > 1) {
        clearCurrent();
        ticks = 0;
        Serial.println("END");
      }
    }
  }
}


void reset(unsigned long time) {
  startTime = time;
  timeLast = time;
  
  val = digitalRead(inPin);
  ticks = 0;
  maxi = 0;
}

void clearCurrent() {
  // set up the LCD's number of columns and rows: 
  lcd.setCursor(8,1);
  lcd.print("--.-");
}


void updateInfo(float freq) {
  if (freq > maxi) {
    maxi = freq;
  }
  actual = freq;
  
  // 4 ticks pr revolution
  avg = ticks / (float) (2 * (time - startTime)) * 1000000;
  
  // display
  // max:00.0 av:00.0
  // actual: 00.0 m/s
    
  dtostrf(frequencyToWindspeed(maxi), 4, 1, maxStr);
  dtostrf(frequencyToWindspeed(avg), 4, 1, avgStr);
  dtostrf(frequencyToWindspeed(actual), 4, 1, actualStr);
  
  // print to LCD
  lcd.setCursor(4, 0);
  lcd.print(maxStr);
  
  lcd.setCursor(12, 0);
  lcd.print(avgStr);
  
  lcd.setCursor(8, 1);
  lcd.print(actualStr);
}

float frequencyToWindspeed(float freq) {
  return 0.238 + 0.8910363 * freq; // 90 deg
}
