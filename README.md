# AstonMartinVH_DashCAN
CAN Control for Aston Martin VH Series CAN Dashboard Control

This code is designed to control the Aston Martin VH series dashboard.  This is a CAN based system and utilizes the chassis CEM in the data pipeline.  This code assumes the CEM is in place and functional and is designed to replicate necessary data from the OEM ECU for the V8/V12 engine to allow for engine conversions.   This project is designed around translating GM OBDII data for RPM and engine temperature to run the appropriate gauges, as well as managing dash wake up/keep alive and traction control and ABS lights.

The current code base is written for a Teensy 4.0 using a Dual CAN-Bus adapter from Fusion Tech.  Previous builds for the RPI2040 had performance issues with incoming data due to hardware design limits within the CANbed Dual Platform.  I will keep those versions of the code here for reference, but all future releases will be based on Teensy.

Multiple variations of this code are presented here to assist others with projects.  They are as follows:

* Teensy_4_Aston_CAN_Translator_v1.ino - (Current) Full support for tachometer, speedometer, coolant gauge, cluster keep-alive and disabling of trac and ABS lights.  Interrupt based management of incoming OBDII data on second CAN channel allows for read from new engine ECU and translation to Aston values and updates to Aston dashboard.  Additional updates will be added, but this is currently a fully functional example.
  
* RPI2040_Aston_CAN_Translator.ino - (Legacy) Basic build with manual control of RPM, coolant temp and trac/ABS lights.
* RPI2040_Aston_CAN_Translator_v3.ino - (Legacy) Display only build with sweep implemented for RPM and coolant temp.  Use to help understand how to interact with these gauges.
* RPI2040_Aston_CAN_Translator_v6.ino - (Legacy) Translator stack implemented.  Reads RPM and ECT from GM ECU and translates to Aston gauge cluster.  Also adds output to enable a ground circuit for Aston starter button when RPM is below a specified value, allowing both the button to be functionally enabled as well as the red "ready to start" light feature to function as OEM.
