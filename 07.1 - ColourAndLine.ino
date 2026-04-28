// TCS230 or TCS3200 pins wiring to Arduino
#define S0 14
#define S1 12
#define S2 25
#define S3 26
#define sensorOut 27

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "color.h"


#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal

#define PIN_NEO_PIXEL 16  // The ESP32 pin GPIO16 connected to NeoPixel
#define NUM_PIXELS 30     // The number of LEDs (pixels) on NeoPixel LED strip

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

// === PIN ASSIGNMENTS ===
int sensorPins[6] = {36, 33, 32, 35, 34, 39}; 

// === VARIABLE ASSIGNMENTS ===
int16_t x = 0;
int16_t y = 0;
int z = 90;

int yellowHits = 0;


int leftSpeed = 150, rightSpeed = 150, servoAngle = 90;
int baseSpeed = 130, setpoint = 0;

float previousError = 0, integral = 0;
float Kp = 1.25, Ki = 0.05, Kd =0.07, K = 1.2;

float P, I, D, u, error, derivative;

float X[6] = { 25, 15, 5, -5, -15, -25 };
//float X[6] = { 40, 20, 10, -10, -20, -40 };
int black[6];
int white[6];


// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Stores the red. green and blue colors
int redColor = 0;
int greenColor = 0;
int blueColor = 0;


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

// change this to make the song slower or faster
int tempo = 144;

// change this to whichever pin you want to use
int buzzer = 15;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;


void setup() {
  // Begins serial communication
  Serial.begin(9600);

  Wire.begin();
  //Red Detected
  NeoPixel.begin();  // initialize NeoPixel strip object

  calibration();

  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  pinMode(buzzer, OUTPUT);
  
}

void FollowLine(){

  int S[6];

  // Read all 6 sensors
  for (int i = 0; i < 6; i++) {
    int raw = analogRead(sensorPins[i]);// ESP32 returns 0–4095
    int reading = map(raw, white[i], black[i], 1000, 0);
    Serial.print(raw);
    reading = constrain(reading, 0, 1000);
    S[i] = reading;
  }

  //weighted avg calculation
  float Xpk = WeightedAverage(S, X, 6);

  //error calculation
  error = Xpk - setpoint;

  //PID Terms
  P = Kp * error;
  integral += error;
  I = Ki * integral;
  derivative= error - previousError;
  D = Kd * derivative;
  //Serial.print("Xpk = ");
  //Serial.print(Xpk);
  //Serial.print("  error = ");
  //Serial.print(error);
  //Serial.print("  P = ");
  //Serial.print(P);
  //Serial.print("  I = ");
  //Serial.print(I);
  //Serial.print("  D = ");
  //Serial.println(D);
  u = P + I + D;
  previousError = error;

  //Set and send Motor and steering values
  servoAngle = 90 + 8*u;
  servoAngle = constrain(servoAngle, 0, 180);
  z = servoAngle;

  //Serial.print("  u = ");
  //Serial.print(u);
  //Serial.print("  Xpk = ");
  //Serial.print(Xpk);
  //Serial.print("  servoAngle = ");
  //Serial.println(servoAngle);

  leftSpeed = baseSpeed + (K*u) ;
  rightSpeed = baseSpeed - (K*u) ;
  leftSpeed = constrain(leftSpeed, 100, 170);
  rightSpeed = constrain(rightSpeed, 100, 170);
  x = leftSpeed;
  y = rightSpeed;

  sendMovement();
}

void RedDetected() {
  // stop motors
  x = 0;
  y = 0;
  // send stop command
  sendMovement();
  // Phase 1: chaotic purple sparkles
  for (int i = 0; i < 20; i++) {
    int p = random(NUM_PIXELS);
    NeoPixel.setPixelColor(p, NeoPixel.Color(180, 0, 180)); // bright purple
    NeoPixel.show();
    delay(25);

    // occasional dim fade to keep it chaotic
    if (random(3) == 0) {
      NeoPixel.setPixelColor(random(NUM_PIXELS), NeoPixel.Color(40, 0, 40));
    }
  }

  // Phase 2: red shockwave burst
  for (int i = 0; i < NUM_PIXELS; i++) {
    NeoPixel.setPixelColor(i, NeoPixel.Color(255, 0, 0));
  }
  NeoPixel.show();
  delay(120);

  // Phase 3: chaotic flicker (purple + red)
  for (int i = 0; i < 10; i++) {
    int p = random(NUM_PIXELS);
    if (random(2) == 0) {
      NeoPixel.setPixelColor(p, NeoPixel.Color(255, 0, 0)); // red spark
    } else {
      NeoPixel.setPixelColor(p, NeoPixel.Color(150, 0, 150)); // purple spark
    }
    NeoPixel.show();
    delay(100);
  }

  // Phase 4: fast fade-out
  for (int b = 150; b >= 0; b -= 30) {
    for (int i = 0; i < NUM_PIXELS; i++) {
      NeoPixel.setPixelColor(i, NeoPixel.Color(b, 0, b)); // fading purple
    }
    NeoPixel.show();
    delay(100);
  }

  NeoPixel.clear();
  NeoPixel.show();
  delay(3000);  // stop for 3 seconds
  // resume forward motion
  x = 150;
  y = 150;
  sendMovement();
  delay(300);
  x = 0;
  y = 0;
  sendMovement();
  delay(500);
}


void BlueDetected(){
// stop motors
  x = 0;
  y = 0;
  // send stop command
  sendMovement();
  Serial.println("BlueDetected start");
  unsigned long startTime = millis();

  while (millis() - startTime < 100) {   // 9 seconds
    for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
      if (millis() - startTime >= 7000) break;
      divider = melody[thisNote + 1];
      if (divider > 0) {
        noteDuration = wholenote / divider;
      } else {
        noteDuration = (wholenote / abs(divider)) * 1.5;
      }

      tone(buzzer, melody[thisNote], noteDuration * 0.9);
      delay(noteDuration);
      noTone(buzzer);
    }
  }
  // resume forward motion
  x = 150;
  y = 150;
  sendMovement();
  delay(300);
  x = 0;
  y = 0;
  sendMovement();
  delay(500);
}

void sendMovement() {
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write((byte)((x >> 8) & 0xFF));
    Wire.write((byte)(x & 0xFF));
    Wire.write((byte)((y >> 8) & 0xFF));
    Wire.write((byte)(y & 0xFF));
    Wire.write((byte)((z >> 8) & 0xFF));
    Wire.write((byte)(z & 0xFF));
    Wire.endTransmission();
}

void GreenDetected(){
  Serial.println("GREEN → STOP 3 seconds");
  // stop motors
  x = 0;
  y = 0;
  // send stop command
  sendMovement();
  delay(3000);  // stop for 3 seconds
  // resume forward motion
  x = 150;
  y = 150;
  sendMovement();
  delay(300);
  x = 0;
  y = 0;
  sendMovement();
  delay(500);

}

void YellowDetected() {
  Serial.println("YELLOW → 180° TURN");

  // balanced turn (tune these values)
  x = -150;   // left motor backwards
  y = 150;    // right motor forwards (slightly weaker)
  z = 180;
  sendMovement();

  delay(5500);   // tune this for exactly 180°

  // resume forward
  x = 150;
  y = 150;
  z = 90;
  sendMovement();
  delay(300);

  // stop
  x = 0;
  y = 0;
  sendMovement();
  delay(500);
}

void loop() {
    // RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFrequency = pulseIn(sensorOut, LOW);
  Serial.print("R = ");
  Serial.print(redFrequency);

  // GREEN
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFrequency = pulseIn(sensorOut, LOW);
  Serial.print(" G = ");
  Serial.print(greenFrequency);

  // BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFrequency = pulseIn(sensorOut, LOW);
  Serial.print(" B = ");
  Serial.println(blueFrequency);


    // Convert to HSV
  float h, s, v;
  convertRGBtoHSV(redFrequency, greenFrequency, blueFrequency, &h, &s, &v);
  Serial.print(" H = ");
  Serial.println(h);


  // NO COLOUR
  if (s<0.3  && s>0.1){
    Serial.println(" - NO COLOUR detected!");
    FollowLine();
  }
  else if(redFrequency < greenFrequency && redFrequency < blueFrequency){
    Serial.println(" - RED detected!");
    RedDetected();
  }
  // GREEN
  else if (h > 10 && h < 26) {
      Serial.println(" - GREEN detected!");
      GreenDetected();
  }
  // YELLOW band (wide)
  bool isYellow = (h > 170 && h < 260);

  // Stability filter: require 3 consecutive yellow readings
  if (isYellow) {
      yellowHits++;
  } else {
      yellowHits = 0;
  }

  if (yellowHits >= 3) {
      yellowHits = 0;      // reset so it only triggers once
      YellowDetected();
      return;
  }
  
  // BLUE
  else if (h >= 26 && h < 40) {
      Serial.println(" - BLUE detected!");
      BlueDetected();
  }

}


void calibration(){
  delay(3000);
  Serial.println("Place all Sensors on Black");
  delay(5000);
  for (int i = 0; i < 6; i++){
    black[i] = analogRead(sensorPins[i]);

  }

  Serial.println("Place all Sensors on White");
  delay(5000);
  for (int i = 0; i < 6; i++){
    white[i] = analogRead(sensorPins[i]);
  }
  Serial.println("done");
  delay(5000);

}




float WeightedAverage(int S[], float X[], int numSensors){
    float numerator = 0.0;
    float denominator = 0.0;

    for (int i = 0; i < numSensors; i++){
        numerator += X[i] * S[i];
        denominator += S[i];  
    }

    if (denominator == 0){
      
        return 0.0;
    }
    return numerator / denominator;
}
