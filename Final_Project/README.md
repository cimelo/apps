# Project Overview

This project is a traffic simulator that runs on an Arduino Uno using the ChibiOS/RT operating system.

## Setup Instructions
1. Download ChibiOS v21.11.3 or later;

2. **Makefile Adjustments**  
   You may need to modify the `Makefile` according to your specific setup. Common adjustments include:
   - Setting the correct `tty` for your Arduino board.
   - Adjusting the relative path to the ChibiOS main directory.

3. Apps Folder
	Create a folder inside the main folder of ChibiOS and paste this project inside it.

4. Connect the pins and LEDs as referenced in the schematic bellow, or change the pins on the traffic.h as you choose.

5. Connect the Arduino using a serial monitor (Arduino IDE is recommended here because of the ability to send string);
	
## Recommended Circuit Schematic
![Schematic](./imgs/schematic.png)

## How to Use
1. The commands send by serial must be written in this way:
	- First char must be the number of the lane, 0 or nothing for the primary lane, 1 for the secondary lane or 2 for the pedestrian lane;
	- On the primary and secondary lanes, you may put up to 7 cars into the lanes, these cars may be 'C' for normal cars or 'A' for ambulances;
	- Onto the pedestrian lane you may only put 'P' for pedestrians;

## Traffic Rules

Only one traffic light will be green at any time. The rules for the green light follow the guidelines below:

- The natural order of the green light is: primary, pedestrian (if there are pedestrians), secondary (if there are cars), and back to primary. However, the pedestrian light must not be green two times in a row if there are cars in the secondary lane;
- Ambulances will move ahead of cars in front of them, even if the traffic light is red;
- If there is an ambulance in the primary lane, the light will turn green within a maximum of 2 seconds, and it will stay green until there are no more ambulances;
- If there is an ambulance in the secondary lane, the light will turn green within 2 seconds or wait until all ambulances in the primary lane have passed;

After the green light, the light will blink at a frequency of 1 Hz, indicating the yellow light.

## Requirements
- **Hardware:** Arduino Uno
- Software: ChibiOS v.21.11.3+/RT
