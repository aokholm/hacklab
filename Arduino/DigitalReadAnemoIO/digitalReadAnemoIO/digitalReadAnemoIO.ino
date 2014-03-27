int inPin = 7;
                       // outside leads to ground and +5V
int val = 0;           // variable to store the value read
int oldVal = 0;
int ledPin = 13;

unsigned long time = 0;
unsigned long readTime = 0;
unsigned long lastUp = 0;


int baseDelta = 250; // 250 mus ~ 4000Hz

void setup()
{
  pinMode(inPin, INPUT);      // sets the digital pin 7 as input
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);          //  setup serial
}

void loop()
{
  time = micros();
  
  if (readTime < time) {
    readTime += baseDelta;
    
    
    val = digitalRead(inPin);
    
    if (oldVal != val) { // when input changes (2 times per revolusion)
    
    //if (!oldVal && val) { // if input goes from 0 to 1
      
      long diff = time-lastUp;
      float freq = 500000/ (float) diff;
      
      Serial.println(freq);
      lastUp = time;
    }
    
    oldVal = val;
    digitalWrite(ledPin, val);
  }  
}
