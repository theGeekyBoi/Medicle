char chinp = 'n';
void setup()
{
  Serial.begin(9600);
}
void loop()
{
while(Serial.available() > 0)
  {
      chinp = char(Serial.read());
      break;
  }
  chinp = 'n';
}
