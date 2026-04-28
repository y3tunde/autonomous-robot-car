#include <Wire.h>

#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal

// === PIN ASSIGNMENTS ===
int sensorPins[6] = {36, 33, 32, 35, 34, 39}; 

// === VARIABLE ASSIGNMENTS ===
int16_t x = 0;
int16_t y = 0;
int z = 90;

int leftSpeed = 150, rightSpeed = 150, servoAngle = 90;
int baseSpeed = 130, setpoint = 0;

float previousError = 0, integral = 0;
float Kp = 1.3, Ki = 0.05, Kd =0.07, K = 1.2;

float P, I, D, u, error, derivative;

float X[6] = { 25, 15, 5, -5, -15, -25 };
//float X[6] = { 40, 20, 10, -10, -20, -40 };
int black[6];
int white[6];




void setup() {
  Serial.begin(9600);   // Fast output
  Wire.begin();   // join i2c bus (address optional for the master) - on the ESP32 the default I2C pins are 21 (SDA) and 22 (SCL)
  delay(500);

  calibration();
}




void loop() {

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
  Serial.print("Xpk = ");
  Serial.print(Xpk);
  Serial.print("  error = ");
  Serial.print(error);
  Serial.print("  P = ");
  Serial.print(P);
  Serial.print("  I = ");
  Serial.print(I);
  Serial.print("  D = ");
  Serial.println(D);
  u = P + I + D;
  previousError = error;

  //Set and send Motor and steering values
  servoAngle = 90 + 8*u;
  servoAngle = constrain(servoAngle, 0, 180);
  z = servoAngle;

  Serial.print("  u = ");
  Serial.print(u);
  Serial.print("  Xpk = ");
  Serial.print(Xpk);
  Serial.print("  servoAngle = ");
  Serial.println(servoAngle);

  leftSpeed = baseSpeed + (K*u) ;
  rightSpeed = baseSpeed - (K*u) ;
  leftSpeed = constrain(leftSpeed, 100, 170);
  rightSpeed = constrain(rightSpeed, 100, 170);
  x = leftSpeed;
  y = rightSpeed;

  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device 
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


