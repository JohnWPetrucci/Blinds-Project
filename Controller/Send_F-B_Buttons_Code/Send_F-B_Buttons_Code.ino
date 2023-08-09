#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); // Initialize the TFT display

TFT_eSPI_Button button1;
TFT_eSPI_Button button2;

void setup() {
  Serial.begin(9600);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  setupButtons();
}

void loop() {
  uint16_t t_x = 0, t_y = 0;
  bool pressed = tft.getTouch(&t_x, &t_y);

  if (pressed) {
    if (button1.contains(t_x, t_y)) {
      Serial.println("Button 1 pressed - Number: 1");
    } else if (button2.contains(t_x, t_y)) {
      Serial.println("Button 2 pressed - Number: 2");
    }
  }
}

void setupButtons() {
  int buttonWidth = tft.width() / 2;
  int buttonHeight = tft.height() / 2;

  button1.initButton(&tft, 0, 0, buttonWidth, buttonHeight, TFT_BLUE, TFT_WHITE, TFT_WHITE, "1", 2);
  button1.drawButton();

  button2.initButton(&tft, buttonWidth, 0, buttonWidth, buttonHeight, TFT_RED, TFT_WHITE, TFT_WHITE, "2", 2);
  button2.drawButton();
}
