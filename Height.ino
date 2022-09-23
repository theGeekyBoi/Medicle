#define echoPin 7
#define trigPin 6
long duration;
int distance;
int hc = 1;
long x = 0;
int y = 0;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  Serial.begin(9600); 
}
void loop() {
  int h = checkheight();
  Serial.println(h);
}

int checkheight()
{
  hc = 1;
  x = 0;
  y = 0;
  int sheight = (millis())/1000;
  while(true)
  {
    int eheight = (millis())/1000;
    if((eheight - sheight) == 5)
    {
      return y;
      break;
    }
    else
    {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration * 0.034 / 2;
      x = x + (200-distance);
      y = x/hc;
      hc++;
    }
  }
}
