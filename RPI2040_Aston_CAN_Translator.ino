// CANBED DUAL TEST EXAMPLE
// CAN 0 Send, CAN 1 Recv

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
    CAN1.init(1000000);         // CAN1 baudrate: 1000kb/s
}

void ClusterKeepAlive() {
	static const unsigned long REFRESH_INTERVAL = 1000; // ms
	static unsigned long lastRefreshTime = 0;
  unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, 141};  //Coolant Temp - For Temp data to be received, Byte 4 = 95, Byte 6 = 192
  //Byte 7 controls temperature.  94 - Full Cold, 185 - Full Hot, 141 - Dead Center


//  Serial.print("Sending Dash Keepalive Frame");
//  Serial.print("\r\n"); 
	if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
	{
		lastRefreshTime += REFRESH_INTERVAL;
    CAN0.send(0x15B00260, 1, 0, 0, 8, CANMSG);
	}
}

void TracABSLights() {
	static const unsigned long REFRESH_INTERVAL = 750; // ms
	static unsigned long lastRefreshTime = 0;
  unsigned char CANMSG[8] = {0, 8, 223, 126, 0, 0, 0, 0};  //Clears ABS and Trac Lights - Byte 1 value of 8 clears Trac, Byte 3 value of 126 clears ABS.


//  Serial.print("Sending Trac/ABS Light Frame");
//  Serial.print("\r\n"); 
	if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
	{
		lastRefreshTime += REFRESH_INTERVAL;
    CAN0.send(0x91C0170, 1, 0, 0, 8, CANMSG);
	}
}

void RPM() {
	static const unsigned long REFRESH_INTERVAL = 50; // ms
	static unsigned long lastRefreshTime = 0;
  unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 254, 242, 255};


//  Serial.print("Sending RPM Frame");
//  Serial.print("\r\n"); 
	if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
	{
		lastRefreshTime += REFRESH_INTERVAL;
    CAN0.send(0x40360, 1, 0, 0, 8, CANMSG);
  }
}

void loop()
{
    ClusterKeepAlive(); 
    RPM();
    TracABSLights();
}

// ENDIF