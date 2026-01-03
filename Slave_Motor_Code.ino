//********************************************************//
//*  University of Nottingham                            *//
//*  Department of Electrical and Electronic Engineering *//
//*  UoN EEEBot                                          *//
//*                                                      *//
//*  Motor & Servo Basic Test Code                       *//
//********************************************************//

// ASSUMPTION: Channel A is LEFT, Channel B is RIGHT

// use this code to correctly assign the four pins to move the car forwards and backwards
// you first need to change the pin numbers for the four motor input 'IN' pins and two enable 'en' pins below and then 
// decide which go HIGH and LOW in each of the movements, stopMotors has been done for you
// ** marks where you need to insert the pin number or state

// feel free to modify this code to test existing or new functions

#include <ESP32Servo.h>
#include <Wire.h>        // include Wire library

Servo steeringServo;
int distance;

#define enA 33  // enableA command line
#define enB 25  // enableB command line

#define INa 26  // channel A direction
#define INb 27  // channel A direction
#define INc 14  // channel B direction
#define INd 12  // channel B direction

// setting PWM properties
const int freq = 2000;
const int ledChannela = 1;  // the ESP32 servo library uses the PWM channel 0 by default, hence the motor channels start from 1
const int ledChannelb = 2;
const int resolution = 8;

int steeringAngle = 90;    // variable to store the servo position
int servoPin = 13;  // the servo is attached to IO_13 on the ESP32


void setup() {
  Wire.begin(0x08);             // join i2c bus with address 8
  Wire.onReceive(receiveEvent); // create a receive event

  // configure the LED PWM functionalitites and attach the GPIO to be controlled - ensure that this is done before the servo channel is attached
  ledcAttachChannel(enA, freq, resolution, ledChannela);
  ledcAttachChannel(enB, freq, resolution, ledChannelb);

  Servo steeringServo;

  int steeringAngle = 90;    // variable to store the servo position
  int servoPin = 13;  // the servo is attached to IO_13 on the ESP32


  // allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	steeringServo.setPeriodHertz(50);    // standard 50Hz servo
	steeringServo.attach(servoPin, 500, 2400);   // attaches the servo to the pin using the default min/max pulse widths of 500us and 2400us

  pinMode(INa, OUTPUT);
  pinMode(INb, OUTPUT);
  pinMode(INc, OUTPUT);
  pinMode(INd, OUTPUT);

  // initialise serial communication
  Serial.begin(9600);
  Serial.println("ESP32 Running");  // sanity check
}


void receiveEvent(){
  while (Wire.available()){  // loop whilst bus is busy
    distance = Wire.read();     // receive data byte by byte 
  }
  Serial.println(distance);    // write distance to serial monitor
  delay(500);
}


void loop() {
  int leftSpeed = 255;
  int rightSpeed = 255;

  if (distance < 10){
    moveSteeringLeft();
    delay(500)
  }

  if (distance > 10){
    goForwards();
  }else {
    stopMotors();
    moveSteeringRight();
    delay(500)
  }

}


void motors(int leftSpeed, int rightSpeed) {
  // set individual motor speed
  // the direction is set separately

  // constrain the values to within the allowable range
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
  ledcWrite(enA, leftSpeed);
  ledcWrite(enB, rightSpeed);
  delay(25);
}

void moveSteeringLeft() {
  steeringServo.write(0);//i think this will turn it left as 90 makes it straight
  steeringServo.write(90);//i think this makes it straight
}

void moveSteeringRight() {
  steeringServo.write(0);
  steeringServo.write(180);//unsure what these 2 lines do remove steering.write 90 to see what the 0 does
}

void goForwards() {
  digitalWrite(INa, LOW);
  digitalWrite(INb, HIGH);
  digitalWrite(INc, HIGH);
  digitalWrite(INd, LOW);
}

void stopMotors() {
  digitalWrite(INa, LOW);
  digitalWrite(INb, LOW);
  digitalWrite(INc, LOW);
  digitalWrite(INd, LOW);
}