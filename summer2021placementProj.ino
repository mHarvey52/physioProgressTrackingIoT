#include <TheThingsNetwork.h>
//#include <LoRaWan_APP.h>
//#include "ttnparams.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

#define appEui "0000000000000000"
#define appKey "97C1278C6C662845B6E612A3DB4E8838"

#define loraSerial Serial1
#define debugSerial Serial

#define freqPlan TTN_FP_EU868
    
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

int ledPin = 5;
int buttonPinA = 9;
int buttonPinB = 8;

float data[3];
float initData[3];
byte dataTransfer[3];
byte initDataTransfer[3];

bool setFlag = false;

//set by user on setup
int numMoves = 3;
//moves counter
int moveNumber = 0;
unsigned long time_now = 0;

void setup(void) {
  loraSerial.begin(57600);
  debugSerial.begin(9600);
  //Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPinA, INPUT_PULLUP); 
  pinMode(buttonPinB, INPUT_PULLUP);

  Wire.begin(); 

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop(void) {
  if (digitalRead(buttonPinB) == LOW and setFlag == false){
    setupInfo();
    setFlag = true;
  }

  if (digitalRead(buttonPinB) == LOW and setFlag == true){
    //transfer data code
    transferData(true);
    debugSerial.println(initData[0]);
    debugSerial.println(initData[1]);
    debugSerial.println(initData[2]);
    delay(500);
  }

  if (moveNumber >= numMoves)
  {
    moveNumber = 0;
    //transfer data code
    transferData(false);
    debugSerial.println("I'm here");
    debugSerial.println(data[0]);
    debugSerial.println(data[1]);
    debugSerial.println(data[2]);
    delay(500);
  }

  if (digitalRead(buttonPinA) == LOW)
  {
    float largestAngle = 0;
    sensors_event_t event; 
    mag.getEvent(&event);
    float startPosX, startPosY, startPosZ;
    unsigned long period = 5000;
    unsigned long myTime = millis();
    while (millis() < myTime + period)
    {
      startPosX = event.magnetic.x;
      startPosY = event.magnetic.y;
      startPosZ = event.magnetic.z;
      float currentAngle = getData(startPosX, startPosY, startPosZ);
      debugSerial.print("Init Data Angle: "); debugSerial.println(initData[moveNumber]);
      debugSerial.print("Current Angle: "); debugSerial.println(currentAngle);
      if (currentAngle >= initData[moveNumber])
      {
        digitalWrite(ledPin, HIGH);
      }
      if (currentAngle > largestAngle)
      {
        largestAngle = currentAngle;
      }
    }
    digitalWrite(ledPin, LOW);
    data[moveNumber] = largestAngle;
    moveNumber += 1;
    debugSerial.println("+1");
    time_now = 0;
  }
}

void setupInfo(void){
  debugSerial.println("Setting up data");
  float startPosX, startPosY, startPosZ, val;
  bool frstRun = true; 
  
  for(int i = 0; i <= 2; i++) {
    sensors_event_t event; 
    mag.getEvent(&event);
    if (frstRun == true){
      delay(5000);
      frstRun = false;
    }
    startPosX = event.magnetic.x;
    startPosY = event.magnetic.y;
    startPosZ = event.magnetic.z;
    debugSerial.print("X: "); debugSerial.print(event.magnetic.x); debugSerial.print("  ");
    debugSerial.print("Y: "); debugSerial.print(event.magnetic.y); debugSerial.print("  ");
    debugSerial.print("Z: "); debugSerial.print(event.magnetic.z); debugSerial.print("  ");debugSerial.println("uT");
  
    digitalWrite(ledPin, HIGH);
    delay(2000);
    val = getData(event.magnetic.x, event.magnetic.y, event.magnetic.z);
    //show ready to move to next movement
    digitalWrite(ledPin, LOW);
    initData[i] = val;
    delay(3000);
  }
}

float getData(float startPosX, float startPosY, float startPosZ) {
  float currentX, currentY, currentZ;
  delay(1000);

  sensors_event_t event1; 
  mag.getEvent(&event1);

  currentX = event1.magnetic.x;
  currentY = event1.magnetic.y;
  currentZ = event1.magnetic.z;
  debugSerial.print("X: "); debugSerial.print(currentX); debugSerial.print("  ");
  debugSerial.print("Y: "); debugSerial.print(currentY); debugSerial.print("  ");
  debugSerial.print("Z: "); debugSerial.print(currentZ); debugSerial.print("  ");debugSerial.println("uT");


  float aDotb = startPosX*currentX + startPosY*currentY + startPosZ*currentZ;
  float aMultb = (sqrt(pow(startPosX,2) + pow(startPosY,2) + pow(startPosZ,2))) * (sqrt(pow(currentX,2) + pow(currentY,2) + pow(currentZ,2)));
  float dotMult = aDotb/aMultb;
  float theta = acos(dotMult);
  debugSerial.println(theta);

  return theta;
}
  
void transferData(bool setUp) {
  if (setUp == true){
    debugSerial.println("setup == true");
    for(int i=0; i <= sizeof(initData); i++){
      initDataTransfer[i] = round(initData[i] * 100);
      debugSerial.println(initDataTransfer[i]);
    }
    ttn.sendBytes(initDataTransfer, sizeof(initDataTransfer));
      //delay(1000);
  }
  else {
    debugSerial.println("setup == false");
    for(int i=0; i <= sizeof(data); i++){
      dataTransfer[i] = round(data[i] * 100);
      debugSerial.println(dataTransfer[i]);
    }
    ttn.sendBytes(dataTransfer, sizeof(dataTransfer));
    //delay(1000);
  }
}
