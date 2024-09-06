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

4. Connect the pins and LEDs as referenced in the schematic bellow, or change the pins on the traffic.h.

5. Connect the Arduino using a serial monitor (Arduino IDE is recommended here because of the ability to send string);
	
## Recommended Circuit Schematic
[!Schematic](./imgs/schematic.pdf)


## Requirements
- **Hardware:** Arduino Uno
- Software: ChibiOS v.21.11.3+/RT
