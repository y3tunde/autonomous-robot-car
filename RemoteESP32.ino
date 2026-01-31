#include <esp_now.h>
#include <WiFi.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#define ROW_NUM 4     // four rows
#define COLUMN_NUM 3  // three columns


uint8_t broadcastAddress[] = { 0xec, 0xe3, 0x34, 0xd1, 0x74, 0x90 };

typedef struct struct_message {
  char status;
  char commandArray[30];
  int commandCount;
} struct_message;

struct_message outgoingValues;
struct_message incomingValues;



char commandArray[30];
int cmdIndex = 0;

//KEYPAD CODE
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte pin_rows[ROW_NUM] = { 32, 33, 25, 26 };   // GPIO18, GPIO5, GPIO17, GPIO16 connect to the row pins
byte pin_column[COLUMN_NUM] = { 27, 14, 17 };  // GPIO4, GPIO0, GPIO2 connect to the column pins

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// RS, E, D4, D5, D6, D7 connected to ESP32
LiquidCrystal lcd(21, 22, 18, 19, 23, 5);

#define LED_PIN 13


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingValues, incomingData, sizeof(incomingValues));

  char cmd = incomingValues.status;  // ← Use status field

  lcd.clear();
  if (cmd == '1') {
    lcd.print("FORWARD");
  } else if (cmd == '2') {
    lcd.print("LEFT");
  } else if (cmd == '3') {
    lcd.print("RIGHT");
  } else if (cmd == '4') {
    lcd.print("BACKWARD");
  } else {
    lcd.print("IDLE");
  }
  delay(500);
}


void setup() {
  // initialise the serial monitor
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  lcd.begin(16, 2);  // Initialize 16x2 LCD

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
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  Serial.println(key);

  if (key == '#') {
      for (int i = 0; i < 30; i++) commandArray[i] = 0;
      cmdIndex = 0;
      lcd.clear();
      return;
  }

  if (key == '*') {
      digitalWrite(LED_PIN, HIGH);
      memcpy(outgoingValues.commandArray, commandArray, 30);
      outgoingValues.commandCount = cmdIndex;
      esp_now_send(broadcastAddress, (uint8_t*)&outgoingValues, sizeof(outgoingValues));
      lcd.clear();
      lcd.print("Executing...");
      return;
  }

  if (key == '1' || key == '2' || key == '3' || key == '4') {
      commandArray[cmdIndex] = key;

      lcd.clear();
      lcd.print("Quantity?");

      char qty = 0;
      while (!qty) qty = keypad.getKey();

      commandArray[cmdIndex + 1] = qty;
      cmdIndex += 2;

      lcd.clear();
      return;
  }
}
