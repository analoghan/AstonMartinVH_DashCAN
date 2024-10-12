#include <Wire.h>
#include <stdio.h>
#include "canbed_dual.h"

CANBedDual CAN0(0); 
CANBedDual CAN1(1); 
# define starterEnable 10

static int ect;
static int rpm;

void setup() 
{
    Serial.begin(115200);
    pinMode(18, OUTPUT);
    pinMode(starterEnable, OUTPUT);

    Wire1.setSDA(6);
    Wire1.setSCL(7);
    Wire1.begin();

    CAN0.init(500000);          // CAN0 baudrate: 500kb/s
    CAN1.init(500000);          // CAN1 baudrate: 500kb/s
}

void starterEnableControl() {
  //Signals external mosfet controller via "starterEnable" pin; connect to ground to enable starter button/light when RPM is <500.  
  //Disables once engine is running (RPM >=500).  Circuit should run through clutch switch for safety.
  if(rpm < 500) {
    digitalWrite(starterEnable, 1);
  }
  else if(rpm >= 500 ) {
    digitalWrite(starterEnable, 0);    
  }
}


void ClusterKeepAlive() {
  //Manages keep alive for cluster and send cluster temperature gauge data.
	static const unsigned long REFRESH_INTERVAL = 2000; // ms
	static unsigned long lastRefreshTime = 0;
  static byte temp;
  //unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, 141};  //Coolant Temp - For Temp data to be received, Byte 4 = 95, Byte 6 = 192
  //Byte 7 controls temperature.  94 - Full Cold, 185 - Full Hot, 141 - Dead Center

    if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
    	lastRefreshTime += REFRESH_INTERVAL;
      temp = (ect + 55);
    //  Serial.print("ECT:");
    //  Serial.println(temp);
      unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, temp};
      CAN0.send(0x15B00260, 1, 0, 0, 8, CANMSG);
    }
}

void TracABSLights() {
  //Disables Traction Control and ABS warning lights on dash.
	static const unsigned long REFRESH_INTERVAL_TRACABS = 750; // ms
	static unsigned long lastRefreshTime_tracAbs = 0;
  unsigned char CANMSG[8] = {0, 8, 223, 126, 0, 0, 0, 0};  //Clears ABS and Trac Lights - Byte 1 value of 8 clears Trac, Byte 3 value of 126 clears ABS.

	if(millis() - lastRefreshTime_tracAbs >= REFRESH_INTERVAL_TRACABS)
	{
//    Serial.print("Sending Trac/ABS Light Frame");
//    Serial.print("\r\n"); 
		lastRefreshTime_tracAbs += REFRESH_INTERVAL_TRACABS;
    CAN0.send(0x91C0170, 1, 0, 0, 8, CANMSG);
	}
}

void RPM() {
  //Send RPM to gauge cluster tachometer
	static const unsigned long REFRESH_INTERVAL = 50; // ms
	static unsigned long lastRefreshTime = 0;
  static byte rpmOut;
//  unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   
//Final 2 bytes specify RPM, direct decimal to binary

    if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
    	lastRefreshTime += REFRESH_INTERVAL;
      //Serial.print("RPM:");
      //Serial.println(rpm);
      byte upper = ((rpm + 60) >> 8) + 224;
      byte lower = (rpm + 60) & 0xFF;
      unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   //Final 2 bytes specify RPM, direct decimal to binary
      CAN0.send(0x40360, 1, 0, 0, 8, CANMSG);
    }
}

void canbus_read() {
  //Read incoming CANbus data into "dtaGet" and isolate/process ECT and RPM frames from GM ECU.
  unsigned char dtaGet[100];
  unsigned long id = 0; 
  int ext = 0;
  int rtr = 0;
  int fd = 0;
  int len = 0;
  int rpmA;
  int rpmB;
  int ectA;

  if(CAN1.read(&id, &ext, &rtr, &fd, &len, dtaGet)) {
    if(id == 201) {
      rpmA = dtaGet[1];
      rpmB = dtaGet[2];
      rpm = (((rpmA * 256) + rpmB) / 4);
    //  Serial.println("RPM Input Frame");
    //  Serial.println(rpm);
    }
    if(id == 1217) {
      ectA = dtaGet[2];
      ect = (ectA - 40);
    //  Serial.println("ECT Input Frame");
    //  Serial.println(ect);
    }
  }
}

void loop()
//Do the things
{
    canbus_read();
    ClusterKeepAlive(); 
    RPM();
    TracABSLights();
    starterEnableControl();
}