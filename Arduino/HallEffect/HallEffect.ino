//HallEffect TLE4905L,

int hallPin = 3;
int ledPin = 13;

volatile boolean hall_low = false;
long start_time = 0;
long previous_time = 0;
long loop_duration = 0;

void setup(){
    Serial.begin(9600);  //Begin serial communcation
    pinMode(ledPin, OUTPUT);
    
    pinMode(hallPin, INPUT);  
    digitalWrite(hallPin, HIGH);
    attachInterrupt(1, hallFalling, FALLING); // pin 3
}

void hallFalling() {
  hall_low = true;
}

void loop() {
  digitalWrite(ledPin, LOW);
  
  if(hall_low) {
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }
  
  if (hall_low) {
    hall_low = false;
    digitalWrite(ledPin, HIGH);    
  }

  delay(100);  
}

//
//void loop(){
//   Serial.println(digitalRead(hallPin)); 
//     
//    if(digitalRead(hallPin) == 1){
//        digitalWrite(ledPin, HIGH);
//        //Serial.println("high"); 
//    }else{
//        digitalWrite(ledPin, LOW);
//        //Serial.println("low"); 
//    }
//
//    delay(200);
//}
