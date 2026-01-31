# autonomous-robot-car(Ongoing)
Ongoing robot car project using C and esp32 

## What it does
- navigates a maze using the default left rule
- has a remote that communicates wirelessly with the robot to make it move and displays movements on LCD
- Uses encoder to control motors
- Reads sensor data to detect obstacles
- Uses i2c communication between 2 esp32 microcontrollers
- Displays a value on a seven segment depending on battery charge using logic gates

## Tech used
- C
- 2 ESP32 Microcontrollers
- HS204 Ultrasonic Sensor
- Motors
- Seven-segment display
- Liquid Crystal Display

## What I learned
- Debugging hardware and software together
- Using complex logic
- Testing reliability and handling sensor noise
- I2C communication
- Wireless ESP32 communication
- Liquid Crystal Display Interfacing
- Keypad Interfacing

## Improvements planned
- Slow robot down in maze (completed)
- More organised code (completed)
- Resolder header :| (completed)
- Tape back of keypad connectivity on Remote PCB to avoid mis communication from keypad to ESP32 
