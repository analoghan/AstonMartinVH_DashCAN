# AstonMartinVH_DashCAN
CAN Control for Aston Martin VH Series CAN Dashboard Control

This code is designed to control the Aston Martin VH series dashboard.  This is a CAN based system and utilizes the chassis CEM in the data pipeline.  This code assumes the CEM is in place and functional and is designed to replicate necessary data from the OEM ECU for the V8/V12 engine to allow for engine conversions.   This project is designed around translating GM OBDII data for RPM and engine temperature to run the appropriate gauges, as well as managing dash wake up/keep alive and traction control and ABS lights.

This code is designed around a RPI2040 CAN (https://www.longan-labs.cc/1030019.html) and implemented via Arduino IDE with appropriate modules for RPI2040 (https://github.com/Longan-Labs/CANBedDual_Arduino_Lib).   The logic in place should be transferrable to other solutions and an ESP32 solution may be added/branched in the future.
