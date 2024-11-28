#include <FlexCAN_T4.h>

# define starterEnable 9

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;

static CAN_message_t msg;
static int rpm = 850;
static int ect = 89;
static int speed = 40;

// -------------------------------------------------------------
void setup(void)
{

  pinMode(2, OUTPUT); // for the transceiver enable pin
  digitalWrite(2, LOW);

  pinMode(3, OUTPUT); // for the transceiver enable pin
  digitalWrite(3, LOW);

  pinMode(starterEnable, OUTPUT);
  digitalWrite(starterEnable, 1);

  can1.begin();
  can1.setBaudRate(500000);
  can2.begin();
  can2.setBaudRate(500000);

  can2.setMaxMB(16);
  can2.enableFIFO();
  can2.enableFIFOInterrupt();
  can2.onReceive(GmCanProcessor);
  can2.mailboxStatus();

}

void GmCanProcessor(const CAN_message_t &msg) {
  //Read incoming CANbus data into "dtaGet" and isolate/process ECT and RPM frames from GM ECU.
  int rpmA;
  int rpmB;
  int speedA;
  int speedB;  
  int ectA;

      //Serial.print("Input Frame Detected, Message ID: ");
      //Serial.println(msg.id);
      //Serial.print(" Extended: ");
      //Serial.println(msg.flags.extended);
      if(msg.id == 201) {
        rpmA = msg.buf[1];
        rpmB = msg.buf[2];
        rpm = (((rpmA * 256) + rpmB) / 4);
        Serial.println("RPM Input Frame");
        Serial.println(rpm);
      }
      if(msg.id == 1001) {
        speedA = msg.buf[0];
        speedB = msg.buf[1];
        speed = (((speedA * 256) + speedB) / 100);
        Serial.println("Speed Input Frame");
        Serial.println(speed);
      }          
      if(msg.id == 1217) {
        ectA = msg.buf[2];
        ect = (ectA - 40);
        Serial.println("ECT Input Frame");
        Serial.println(ect);
      }
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
	const unsigned long REFRESH_INTERVAL_KA = 1000; // ms
	static unsigned long lastRefreshTimeKa = 0;
  static byte temp;
  //ect = 89;
  //unsigned char CANMSG[8] = {0, 0, 181, 127, 95, 0, 192, 141};  //Coolant Temp - For Temp data to be received, Byte 4 = 95, Byte 6 = 192
  //Byte 7 controls temperature.  94 - Full Cold, 185 - Full Hot, 141 - Dead Center

    if(millis() - lastRefreshTimeKa >= REFRESH_INTERVAL_KA)
    {
    	lastRefreshTimeKa += REFRESH_INTERVAL_KA;
      temp = (ect + 77);
      //Serial.print("ECT:");
      //Serial.println(temp);
      msg.flags.extended = 1;
      msg.id = 363856480;
      msg.len = 8;
      msg.buf[0] = 0;
      msg.buf[1] = 0;
      msg.buf[2] = 181;
      msg.buf[3] = 127;
      msg.buf[4] = 95;
      msg.buf[5] = 0;
      msg.buf[6] = 192;
      msg.buf[7] = temp;      
      msg.buf[0]++;
      can1.write(msg);
    }
}

void RPM() {
  //Send RPM to gauge cluster tachometer
	static const unsigned long REFRESH_INTERVAL_RPM = 20; // ms
	static unsigned long lastRefreshTimeRPM = 0;
  //rpm = 650;
  //  unsigned char CANMSG[8] = {0, 0, 0, 0, 0, 0, upper, lower};   
  //Final 2 bytes specify RPM, direct decimal to binary

    if(millis() - lastRefreshTimeRPM >= REFRESH_INTERVAL_RPM)
    {
    	lastRefreshTimeRPM += REFRESH_INTERVAL_RPM;
      //Serial.print("RPM:");
      //Serial.println(rpm);
      byte upper = ((rpm + 30) >> 8) + 224;
      byte lower = (rpm + 30) & 0xFF;
      msg.flags.extended = 1;
      msg.id = 263008;
      msg.len = 8;
      msg.buf[0] = 0;
      msg.buf[1] = 200;
      msg.buf[2] = 0;
      msg.buf[3] = 0;
      msg.buf[4] = 0;
      msg.buf[5] = 0;
      msg.buf[6] = upper;
      msg.buf[7] = lower;      
      msg.buf[0]++;
      can1.write(msg); 
    }
}

void Speed() {
  // Send vehicle speed to gauge cluster speedometer
	static const unsigned long REFRESH_INTERVAL_SPEED = 50; // ms
	static unsigned long lastRefreshTimeSpeed = 0;
  static int speedMPH;
  // speed = 50;
  // unsigned char CANMSG[8] = {0, 8, 0, 126, 0, 0, upper, lower};   
  // Final 2 bytes specify Speed, last byte is 0 - 255 for 1 MPH, ignoring for simplicity.

    if(millis() - lastRefreshTimeSpeed >= REFRESH_INTERVAL_SPEED)
    {
    	lastRefreshTimeSpeed += REFRESH_INTERVAL_SPEED;
      speedMPH = (speed / 1.59);
      Serial.print("Speed:");
      Serial.println(speed);
      msg.flags.extended = 1;
      msg.id = 152830320;
      msg.len = 8;
      msg.buf[0] = 0;
      msg.buf[1] = 8;
      msg.buf[2] = 223;
      msg.buf[3] = 126;
      msg.buf[4] = 192;
      msg.buf[5] = 0;
      msg.buf[6] = speedMPH;
      msg.buf[7] = 0;
      msg.buf[0]++;
      can1.write(msg); 
    }
}

// -------------------------------------------------------------
void loop(void)
{

  can2.events();
  ClusterKeepAlive(); 
  Speed();
  RPM();
  starterEnableControl();

}