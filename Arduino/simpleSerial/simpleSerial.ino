
//Settings
int secretNumber = 0;


void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(200);
  Serial.begin(115200);//start the serial
  delay(200);
  digitalWrite(13, HIGH);
  }

void loop(){
    
  
  Serial.print(secretNumber, DEC); Serial.print(","); 
  Serial.print(secretNumber+1, DEC); Serial.print(","); 
  Serial.print(secretNumber+2, DEC); Serial.print(","); 
  Serial.print(secretNumber+3, DEC); Serial.print("\n");   
  
  //Serial.write(secretNumber);
//  if(Serial.available() > 0){//if serial is sending
//      int recieved = Serial.read();//read the first byte
//      Serial.print(recieved*2); // return twice the number
//  }
  
  delay(10);
  secretNumber ++;
}
