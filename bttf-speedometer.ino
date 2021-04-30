#include <TinyGPS++.h>
#include <SoftwareSerial.h>

boolean PatternNum[10][7]={
  {1,1,1,1,1,1,0}, //0
  {0,1,1,0,0,0,0}, //1
  {1,1,0,1,1,0,1}, //2
  {1,1,1,1,0,0,1}, //3
  {0,1,1,0,0,1,1}, //4
  {1,0,1,1,0,1,1}, //5
  {1,0,1,1,1,1,1}, //6
  {1,1,1,0,0,1,0}, //7
  {1,1,1,1,1,1,1}, //8
  {1,1,1,1,0,1,1} //9
};
boolean PatternClear[7]={0,0,0,0,0,0,0};
boolean PatternMinus[7]={0,0,0,0,0,0,1};

int StartPin[2]={9, 2};
int Segment[7]={0,2,5,6,3,1,4};
int SegmentDot=16;

static const int RXPin = 21, TXPin = 20;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

int wSpeed = -1;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("### Back to the future - Speedometer"));
  Serial.println(F("by Bluewater"));
  Serial.println();

  for (int i=2; i<17; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop()
{
  digitalWrite(SegmentDot, HIGH);
  
  while (ss.available() > 0) {
    byte gpsData = ss.read();
    if (gps.encode(gpsData)){
      displayInfo();
    }

    if (!gps.location.isValid())
    {
      digitalWrite(SegmentDot, LOW);
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    displayDigitPattern(0, PatternMinus);
    displayDigitPattern(1, PatternMinus);
    while(true);
  }

  displaySpeed();
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  int s = (int)(gps.speed.kmph());
  Serial.print("  ");
  Serial.print(s);
  Serial.print("Km/h");  

  Serial.println();
}

void displaySpeed() {
  int s = (int)(gps.speed.kmph());
  while(s != wSpeed) {
    if (wSpeed > s) {
      wSpeed--;
    } else {
      wSpeed++;
    }
    displayNum(wSpeed);
    delay(80);
  }
}

void displayNum(int num) {
  for (int digit=0; digit < 2; digit++) {
    int num1 = (num/(int)pow(10, digit))%10;
    displayDigitNum(digit, num1);
  }
}

void displayDigitNum(int digit, int num) {
  if (digit>0 && num==0) {
    displayDigitPattern(digit, PatternClear);
  } else {
    displayDigitPattern(digit, PatternNum[num]);
  }
}

void displayDigitPattern(int digit, boolean pattern[]) {
  for (int i=0; i<7; i++) {
    if (pattern[i] == 1) {
      digitalWrite(StartPin[digit] + Segment[i], HIGH);
    } else {
      digitalWrite(StartPin[digit] + Segment[i], LOW);
    }
  }
}
