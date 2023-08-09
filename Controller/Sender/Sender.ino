#include <esp_now.h>
#include <WiFi.h>

const int buttonPin = 14;  // the number of the pushbutton pin
const int buttonPin2 = 12;  // the number of the pushbutton pin

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xC0, 0x49, 0xEF, 0x63, 0xC3, 0x50};
int buttonState = 0;
int buttonState2 = 0;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int left; // Motor Turns One Way
  int right; // Motor Turns Opposite Way
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
    // read the state of the switch/button:
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
  
  if(buttonState == LOW){
    Serial.println("BS1 The state changed from LOW to HIGH");
  myData.left = 100;
  myData.right = 0;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);
}

  if(buttonState2 == LOW){
    Serial.println("BS2 The state changed from LOW to HIGH");
  myData.left = 0;
  myData.right = 1;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);
}

delay(10);
}
