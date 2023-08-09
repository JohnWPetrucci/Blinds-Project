#include <esp_now.h>
#include <WiFi.h>

int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 
int dutyCycle = 200;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int left;  // Motor Turns One Way
  int right; // Motor Turns Opposite Way
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Left: ");
  Serial.println(myData.left);
  Serial.print("Right: ");
  Serial.println(myData.right);
}

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully initialized, register the callback function to receive data
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (myData.left == 100) {
    // Move the DC motor forward at maximum speed
    Serial.println("Moving Forward");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    analogWrite(enable1Pin, dutyCycle); // Set the motor speed

    delay(1000);

    // Stop the DC motor
    Serial.println("Motor stopped");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    analogWrite(enable1Pin, 0); // Set the motor speed to 0 (stop)
    myData.left = 0;
    delay(25);
  }
  if (myData.right == 1) {
    // Move the DC motor forward at maximum speed
    Serial.println("Moving Forward");
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    analogWrite(enable1Pin, dutyCycle); // Set the motor speed

    delay(1000);

    // Stop the DC motor
    Serial.println("Motor stopped");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    analogWrite(enable1Pin, 0); // Set the motor speed to 0 (stop)
    myData.right = 0;
    delay(25);
  }
}
