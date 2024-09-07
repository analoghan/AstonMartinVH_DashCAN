#include <Wire.h>
#include <stdio.h>
#include "canbed_dual.h"

CANBedDual CAN0(0); 
CANBedDual CAN1(1); 

void setup() 
{
    Serial.begin(115200);
    pinMode(18, OUTPUT);

    Wire1.setSDA(6);
    Wire1.setSCL(7);
    Wire1.begin();

    CAN0.init(500000);          // CAN0 baudrate: 500kb/s
    CAN1.init(500000);         // CAN1 baudrate: 500kb/s
}

void ClusterKeepAlive() {
	static const unsigned long REFRESH_INTERVAL = 2000; // ms
	static unsigned long lastRefreshTime = 0;
  unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, 141};  //Coolant Temp - For Temp data to be received, Byte 4 = 95, Byte 6 = 192
  //Byte 7 controls temperature.  94 - Full Cold, 185 - Full Hot, 141 - Dead Center

    static int ect = 94;
    static int increment = 3;
    static int direction = 1;

    if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
    	lastRefreshTime += REFRESH_INTERVAL;
      if ((ect <= 94))
      {
        direction = 0;
        ect = (ect + increment);
//        Serial.print("ECT: ");
//        Serial.println(ect);
//        Serial.println("Engine Cold");                
      }
      if ((ect < 193) && (ect >= 95) && (direction == 0))
      {
        ect = (ect + increment);
        byte temp = ect;
//        Serial.println("Going Up!");
//        Serial.print("ECT: ");
//        Serial.println(ect);
        unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, temp};
        CAN0.send(0x15B00260, 1, 0, 0, 8, CANMSG);
      }
      else if ((ect >= 193))
      {
        direction = 1;
        ect = (ect - increment);
//        Serial.println("Max Temp Hit");
      }
      else if ((ect < 193) && (ect > 94) && (direction == 1))
      {
        ect = (ect - increment);
        byte temp = ect;
//        Serial.println("Going Down!");
//        Serial.print("ECT: ");
//        Serial.println(ect);
        unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, temp};
        CAN0.send(0x15B00260, 1, 0, 0, 8, CANMSG);
      }
    }


}

void TracABSLights() {
	static const unsigned long REFRESH_INTERVAL_TRACABS = 750; // ms
	static unsigned long lastRefreshTime_tracAbs = 0;
  unsigned char CANMSG[8] = {0, 8, 223, 126, 0, 0, 0, 0};  
  //Clears ABS and Trac Lights - Byte 1 value of 8 clears Trac, Byte 3 value of 126 clears ABS.

	if(millis() - lastRefreshTime_tracAbs >= REFRESH_INTERVAL_TRACABS)
	{
//    Serial.print("Sending Trac/ABS Light Frame");
//    Serial.print("\r\n"); 
		lastRefreshTime_tracAbs += REFRESH_INTERVAL_TRACABS;
    CAN0.send(0x91C0170, 1, 0, 0, 8, CANMSG);
	}
}

void RPM() {
	static const unsigned long REFRESH_INTERVAL = 50; // ms
	static unsigned long lastRefreshTime = 0;
//  unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   
  //Final 2 bytes specify RPM, direct decimal to binary

    static int rpm = 0;
    static int increment = 50;
    static int direction = 1;

    if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
    	lastRefreshTime += REFRESH_INTERVAL;
      if ((rpm == 0))
      {
        direction = 0;
        rpm = (rpm + increment);
//        Serial.print("RPM: ");
//        Serial.println(rpm);
//        Serial.println("Engine Stall");                
      }
      if ((rpm < 8000) && (rpm >= 50) && (direction == 0))
      {
        rpm = (rpm + increment);
        byte upper = ((rpm + 60) >> 8) + 224;
        byte lower = (rpm + 60) & 0xFF;
//        Serial.println("Going Up!");
//        Serial.print("RPM: ");
//        Serial.println(rpm);
//        Serial.print("Upper: ");
//        Serial.println(upper);
//        Serial.print("Lower: ");
//        Serial.println(lower); 
        unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   //Final 2 bytes specify RPM, direct decimal to binary
        CAN0.send(0x40360, 1, 0, 0, 8, CANMSG);       
      }
      else if ((rpm == 8000))
      {
        direction = 1;
        rpm = (rpm - increment);
//        Serial.println("Redline Hit");
      }
      else if ((rpm < 8000) && (rpm > 0) && (direction == 1))
      {
        rpm = (rpm - increment);
        byte upper = ((rpm + 60) >> 8) + 224;
        byte lower = (rpm + 60) & 0xFF;
//        Serial.println("Going Down!");
//        Serial.print("RPM: ");
//        Serial.println(rpm);
//        Serial.print("Upper: ");
//        Serial.println(upper);
//        Serial.print("Lower: ");
//        Serial.println(lower);
        unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   //Final 2 bytes specify RPM, direct decimal to binary
        CAN0.send(0x40360, 1, 0, 0, 8, CANMSG);
      }
    }
}

void loop()
{
    ClusterKeepAlive(); 
    RPM();
    TracABSLights();
}