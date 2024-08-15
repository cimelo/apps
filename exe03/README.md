# Exe03

## TARGET

The demo runs on an Arduino Uno board.

## The Demo
Program that reads luminosity from a LDR and turns a LED if it's dark and 
turns it off else.

it uses 3 threads:
- Thread1: Blinks 2 LEDs
- Thread2: Reads from a LDR and turns on a LED based upon the brightness
