#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <ESP32Encoder.h>

ESP32Encoder encoder;
ESP32Encoder encoder2;

Servo steeringServo;
int steeringAngle = 90;    // variable to store the servo position
int servoPin = 13;  // the servo is attached to IO_13 on the ESP32

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

// replace 'FF' with the MAC address of ESP32 Device 2 (the receiving ESP32)
uint8_t broadcastAddress[] = {0x08, 0x3a, 0x8d, 0x0d, 0x80, 0x2c};

// this must match the receiver structure
typedef struct struct_message {
    char status;          // '1','2','3','4' or '0' for DONE
    char commandArray[30];
    int commandCount;
} struct_message;

// define variables to store incoming values
char commandArray[30];
int commandCount;

// variable to store if sending data was successful
String success;

struct_message outgoingValues;
struct_message incomingValues;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  memcpy(&incomingValues, incomingData, sizeof(incomingValues));
  memcpy(commandArray, incomingValues.commandArray, 30);
  commandCount = incomingValues.commandCount;//copying the command array from the remote(incomingvalues) into the command array on the robot
  Serial.println(commandArray);
  Serial.println("ROBOT RECEIVED DATA");

  for (int i=0;  i < commandCount; i += 2){
    char movement = commandArray[i];
    char qtyChar = commandArray[i + 1];
    Serial.println(movement);
    Serial.println(qtyChar);
    int quantity = qtyChar - '0';  // convert char to int

    // Send status to remote
    outgoingValues.status = movement;
    esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));

    if (movement == '1'){
      goForwards(quantity);
    }else if (movement == '2'){
      goLeft(quantity);
    }else if (movement == '3'){
      goRight(quantity);
    }else if (movement == '4'){ 
      goBackwards(quantity);
    }

  }
  outgoingValues.status = '0';   // DONE
  esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));
}
 
void setup() {
  // initialise the serial monitor
  Serial.begin(115200);

  // set the device as a Wi-Fi station
  WiFi.mode(WIFI_STA);

  // initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initialising ESP-NOW");
    return;
  }

  // once ESP-NOW is successfully initialised, we will register for send callback to get the status of transmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  // register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // register the peer device
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // add the peer device        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Servo steeringServo;
  int steeringAngle = 90;    // variable to store the servo position
  int servoPin = 13;  // the servo is attached to IO_13 on the ESP32
  steeringServo.setPeriodHertz(50);
  steeringServo.attach(servoPin, 500, 2400);
  steeringServo.write(steeringAngle);
  steeringServo.write(0);
  steeringServo.write(steeringAngle);
  // configure the LED PWM functionalitites and attach the GPIO to be controlled - ensure that this is done before the servo channel is attached
  ledcAttachChannel(enA, freq, resolution, ledChannela);
  ledcAttachChannel(enB, freq, resolution, ledChannelb);

  // allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

	//steeringServo.setPeriodHertz(50);    // standard 50Hz servo
	//steeringServo.attach(servoPin, 500, 2400);   // attaches the servo to the pin using the default min/max pulse widths of 500us and 2400us
  //steeringServo.write(steeringAngle);

  pinMode(INa, OUTPUT);
  pinMode(INb, OUTPUT);
  pinMode(INc, OUTPUT);
  pinMode(INd, OUTPUT);

	//ESP32Encoder::useInternalWeakPullResistors = puType::up;
	// use pin 19 and 18 for the first encoder
	encoder.attachHalfQuad(36, 39);
	// use pin 17 and 16 for the second encoder
	encoder2.attachHalfQuad(34, 35);
  encoder.setCount(0);
}
 
void loop() { 
  //steeringServo.setPeriodHertz(50);
  //steeringServo.attach(servoPin, 500, 2400);
  //in loop values should be being recieved mainly. once received and the ondata receive dunction runs they should send back commands to be displayed on the lcd
  //steeringServo.write(steeringAngle);
  //Serial.println("SERVOTEST");
  //delay(1000);
  //steeringServo.write(0);
  //delay(1000);
  //receive array of commands from remote esp
}

void motors(int leftSpeed, int rightSpeed) {
  // set individual motor speed
  // the direction is set separately
  // constrain the values to within the allowable range
  leftSpeed = constrain(leftSpeed, 0, 180);
  rightSpeed = constrain(rightSpeed, 0, 180);
  
  ledcWrite(enA, leftSpeed);
  ledcWrite(enB, rightSpeed);
  delay(20);
}

void stopMotors() {
  digitalWrite(INa, LOW);
  digitalWrite(INb, LOW);
  digitalWrite(INc, LOW);
  digitalWrite(INd, LOW);
}

void goForwards(int quantity) {
  encoder.setCount(0);
  Serial.print("goForwards, quantity = ");
  Serial.println(quantity);

  outgoingValues.status = '1';   // tell remote "FORWARD"
  esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));

  
  int target = (quantity*38)/2;//using encoder count to calculate distance
  
  while (encoder.getCount() < target){
    Serial.println(encoder.getCount());
    digitalWrite(INa, LOW);
    digitalWrite(INb, HIGH);
    digitalWrite(INc, HIGH);
    digitalWrite(INd, LOW);
    motors(180,180);
  }
  stopMotors();
  delay(1000);
}

void goLeft(int quantity) {
  Serial.print("goLeft, quantity = ");
  Serial.println(quantity);

  outgoingValues.status = '2';   // tell remote "LEFT"
  esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));

  Serial.println("test1");

  steeringServo.setPeriodHertz(50);
  steeringServo.attach(servoPin, 500, 2400);
  steeringServo.write(0);

  motors(255,255);
  digitalWrite(INa, HIGH);
  digitalWrite(INb, LOW);
  digitalWrite(INc, HIGH);
  digitalWrite(INd, LOW);
  delay(680);
  steeringServo.write(90);
  stopMotors();
  delay(1000);

}

void goRight(int quantity) {
  Serial.print("goRight, quantity = ");
  Serial.println(quantity);

  outgoingValues.status = '3';   // tell remote "RIGHT"
  esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));
  
  steeringServo.setPeriodHertz(50);
  steeringServo.attach(servoPin, 500, 2400);
  steeringServo.write(180);
  
  motors(255,255);
  digitalWrite(INa, LOW);
  digitalWrite(INb, HIGH);
  digitalWrite(INc, LOW);
  digitalWrite(INd, HIGH);
  delay(680);
  steeringServo.write(90);
  stopMotors();
  delay(1000);
}

void goBackwards(int quantity) {
  encoder.setCount(0);
  Serial.print("goBackwards, quantity = ");
  Serial.println(quantity);

  outgoingValues.status = '4';   // tell remote "Backwards"
  esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));

  encoder.setCount(0);
  int target = (quantity*38)/2;//using encoder count to calculate distance
  target = target*-1;
  while (encoder.getCount() > target){
    Serial.println(encoder.getCount());
    digitalWrite(INa, HIGH);
    digitalWrite(INb, LOW);
    digitalWrite(INc, LOW);
    digitalWrite(INd, HIGH);
    motors(180,180);
  }
  stopMotors();
  delay(1000);

}

