//********************************************************//
//*  University of Nottingham                            *//
//*  Department of Electrical and Electronic Engineering *//
//*  UoN EEEBot                                          *//
//*                                                      *//
//*  Skeleton Master Code for Use with the               *//
//*  EEEBot_MainboardESP32_Firmware Code                 *//
//********************************************************//

// the following code acts as a 'bare bones' template for your own custom master code that works with the firmware code provided
// therefore, the variable names are non-descriptive - you should rename these variables appropriately
// you can either modify this code to be suitable for the project week task, or use the functions as inspiration for your own code

#include <Wire.h>
#include <NewPing.h>     // include Wire library

#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal

#define TRIGGER_PIN  19  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     18  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define PING_COUNT 5   // number of samples for filtering
//#define LED_PIN 14 // defining led pin

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

int distance;

void setup()
{
  Serial.begin(9600);
  Wire.begin();   // join i2c bus (address optional for the master) - on the ESP32 the default I2C pins are 21 (SDA) and 22 (SCL)
}

// three integer values are sent to the slave device
int16_t x = 0;
int16_t y = 0;
int z = 90;
int go = 1;
int flag = 0;
bool isTurning = false;
int turnDelay = 0;

float getDistance(){
  unsigned int Med = sonar.ping_median(PING_COUNT);  // filtered output
  float distance = Med / 58.0;
  float Unfiltered= sonar.ping_cm();
  Serial.print(distance);
  return distance;
}

int Move(){

  if (getDistance() > 10){
    x = 255;
    y = 255;
    go = 1;
  }else{
    go = 0;
  }
  return go;
}
int Rotate(int flag){
  if (flag == 0){
    x = 0;
    y = 0;
    z = 0;
    flag = 1;
    x = -255;
    y = 255;


  }else if (flag == 1){
    x = 0;
    y = 0;
    z = 180;
    x = 255;
    y = -255;
    flag = 0;
  }
  return flag;
}


void loop()
{
  // two 16-bit integer values are requested from the slave
  int16_t a = 0;
  int16_t b = 0;
  uint8_t bytesReceived = Wire.requestFrom(I2C_SLAVE_ADDR, 4);  // 4 indicates the number of bytes that are expected
  uint8_t a16_9 = Wire.read();  // receive bits 16 to 9 of a (one byte)
  uint8_t a8_1 = Wire.read();   // receive bits 8 to 1 of a (one byte)
  uint8_t b16_9 = Wire.read();   // receive bits 16 to 9 of b (one byte)
  uint8_t b8_1 = Wire.read();   // receive bits 8 to 1 of b (one byte)

  a = (a16_9 << 8) | a8_1; // combine the two bytes into a 16 bit number
  b = (b16_9 << 8) | b8_1; // combine the two bytes into a 16 bit number

  //Serial.print(a);
  //Serial.print("\t");
  //Serial.println(b);

  //SENSOR DISTANCE
  //delay(1000);

  //Serial.println(dista);
  
 if (!isTurning){
    int canMove = Move();

    if (canMove == 0){
      flag =  Rotate(flag);
      isTurning = true;
      turnDelay = 400;

    }else{
      flag = 0;
      z = 90;
    }
  }else{
    delay(turnDelay);
    isTurning = false;
  }
  

  
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
  /* depending on the microcontroller, the int variable is stored as 32-bits or 16-bits
     if you want to increase the value range, first use a suitable variable type and then modify the code below
     for example; if the variable used to store x and y is 32-bits and you want to use signed values between -2^31 and (2^31)-1
     uncomment the four lines below relating to bits 32-25 and 24-17 for x and y
     for my microcontroller, int is 32-bits hence x and y are AND operated with a 32 bit hexadecimal number - change this if needed

     >> X refers to a shift right operator by X bits
  */
  //Wire.write((byte)((x & 0xFF000000) >> 24)); // bits 32 to 25 of x
  //Wire.write((byte)((x & 0x00FF0000) >> 16)); // bits 24 to 17 of x
  Wire.write((byte)((x & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(x & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  //Wire.write((byte)((y & 0xFF000000) >> 24)); // bits 32 to 25 of y
  //Wire.write((byte)((y & 0x00FF0000) >> 16)); // bits 24 to 17 of y
  Wire.write((byte)((y & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(y & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((z & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(z & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1
  Wire.endTransmission();   // stop transmitting
  
}
