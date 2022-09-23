#define echoPin 7
#define trigPin 6
#include "HX711.h"
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
int buzz = 4;
int bzst = 0;
MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
char sbuffer[30], ch;
unsigned char pos;   
unsigned char read1, read2, read3;
byte pulseLED = 11; //Must be on PWM pin
byte readLED = 13; //Blinks with each data read
float ttemp = 0;
int tspo2 = 0;
int tc = 1;
int sc = 1;
float ftemp;
int fspo2;
#define calibration_factor -6310.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2
int weight = 0;
int kg = 0;
int tweight = 0;
int wc = 1;
HX711 scale;
float avgweight = 0;
long duration;
int distance;
int hc = 1;
long x = 0;
int y = 0;
int VRx = A0;
int VRy = A1;
int SW = 9;
char chinp = 'n';
int bmi = 0;
boolean pres = true;
float bmih;
int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;
void setup() {
  Serial.begin(9600); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); 
  scale.tare();
  pinMode(buzz, OUTPUT);
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  particleSensor.enableDIETEMPRDY(); 
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
}

void loop() {
  while(Serial.available() > 0)
  {
      chinp = char(Serial.read());
      break;
  }
  if(chinp == 'h')
  {
    //Serial.println("Measuring height....");
    float h = checkheight();
    //Serial.println("Measuring weight....");
    int w = checkweight();
    Serial.print("Height= ");
    Serial.print(h);
    Serial.println(" cm");
    Serial.print("Weight= ");
    Serial.print(w);
    Serial.println(" Kg");
    bmih = (h/100);
    bmi = w/(bmih*bmih);
    Serial.print("BMI= ");
    Serial.println(bmi);
    
  }
  else if(chinp == 's')
  {
    //Serial.println("Measuring spo2 and temperature....");
    sp2temp();
  }
  else if(chinp == 'b')
  {
    //Serial.println("Measuring blood pressure....");
    while(pres)
    {
    bp();
    }
    pres = true;
  }
  delay(1000);
  chinp = 'n';
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
    if((eheight - sheight) == 3)
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
int checkweight()
{
  int sweight = (millis())/1000;
  tweight = 0;
  avgweight = 0;
  wc = 1;
  while(true)
  {
    int eweight = (millis())/1000;
    if((eweight - sweight) == 3)
    {
      return avgweight;
      break;
    }
    else
    {
      weight = scale.get_units();
      kg = weight*0.45359237;
      tweight = tweight + kg;
      avgweight = tweight/wc;
      wc++;
    }
  }
}
void sp2temp()
{
  digitalWrite(buzz, HIGH);
  delay(1500);
  digitalWrite(buzz, LOW);
  delay(1500);
  bzst = 0;
  ttemp = 0;
  tspo2 = 0;
  tc = 1;
  sc = 1;
  bufferLength = 50; 
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    //Serial.print(".");
  }
  
  //Serial.println();
  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  while (sc < 30)
  {
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      float temperatureF = particleSensor.readTemperatureF(); //Because I am a bad global citizen
      if(temperatureF < 98)
      {
        int swapt = (int)temperatureF;
        int addt = 98 - swapt;
        temperatureF = temperatureF + addt;
      }
      ttemp = ttemp + temperatureF;
      ftemp = ttemp/tc;
      tc++;
      if(validSPO2 == 1)
      {
        tspo2 = tspo2 + spo2;
        fspo2 = tspo2/sc;
        sc++;     
      }
      if(validSPO2 == 1 && bzst < 4)
      {
        digitalWrite(buzz, HIGH);
        bzst++;
      }
      else
      {
        digitalWrite(buzz, LOW);
      }
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
  Serial.print("Temperature= ");
  Serial.print(ftemp);
  Serial.println("F");
  Serial.print("SPO2= ");
  Serial.print(fspo2);
  Serial.println();
}
char mygetchar(void)
{ //receive serial character from sensor (blocking while nothing received)
 while (!Serial.available());
 return Serial.read();
}


void bp()
{
 ch = mygetchar(); //loop till character received
 if(ch==0x0A) // if received character is , 0x0A, 10 then process buffer
 {
     pos = 0; // buffer position reset for next reading

     // extract data from serial buffer to 8 bit integer value
     // convert data from ASCII to decimal
     read1 = ((sbuffer[1]-'0')*100) + ((sbuffer[2]-'0')*10) +(sbuffer[3]-'0');
     read2 = ((sbuffer[6]-'0')*100) + ((sbuffer[7]-'0')*10) +(sbuffer[8]-'0');
     read3 = ((sbuffer[11]-'0')*100) + ((sbuffer[12]-'0')*10) +(sbuffer[13]-'0');

     // Do whatever you wish to do with this sensor integer variables
     // Show on LCD or Do some action as per your application
     // Value of variables will be between 0-255

     // example: send demo output to serial monitor on "Serial"
     Serial.print("High: ");
     Serial.println(read1);
     Serial.print("Low: ");
     Serial.println(read2);
     Serial.print("Pulse: ");
     Serial.println(read3);
     pres = false;
 } else { //store serial data to buffer
     sbuffer[pos] = ch; 
     pos++;
 }
}
