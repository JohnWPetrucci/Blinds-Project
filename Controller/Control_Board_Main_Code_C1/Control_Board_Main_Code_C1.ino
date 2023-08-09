#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); // Initialize the TFT display

#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false
#define SCREEN_ON_PIN 16

TFT_eSPI_Button mainButton;

void setup() {
  Serial.begin(9600);
  tft.init();
  tft.setRotation(0);
  pinMode(SCREEN_ON_PIN, OUTPUT);
  digitalWrite(SCREEN_ON_PIN, LOW);
  touch_calibrate();
  tft.fillScreen(TFT_BLACK);
  setupMainButton();
}

void loop() {
  checkButtonPresses();
}

void setupMainButton() {
  int buttonX = tft.width() / 2;
  int buttonY = tft.height() / 2;
  mainButton.initButton(&tft, buttonX, buttonY, tft.width(), tft.height(), TFT_BLUE, TFT_WHITE, TFT_WHITE, "Big Button", 2);
  mainButton.drawButton();
}

void turnDisplayOn() {
  digitalWrite(SCREEN_ON_PIN, HIGH);
}

void checkButtonPresses() {
  uint16_t t_x = 0, t_y = 0;
  bool pressed = tft.getTouch(&t_x, &t_y);

  if (pressed && isButtonPressed(mainButton, t_x, t_y)) {
    turnDisplayOn();
    delay(1000);
    tft.fillScreen(TFT_BLACK);
    mainButton.drawButton();
  }
}

bool isButtonPressed(TFT_eSPI_Button &button, uint16_t t_x, uint16_t t_y) {
  return button.contains(t_x, t_y);
}
void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CALIBRATION_FILE)) {
      if (REPEAT_CAL) {
        // Delete if we want to re-calibrate
        digitalWrite(SCREEN_ON_PIN, HIGH);
        SPIFFS.remove(CALIBRATION_FILE);
      } else {
        File f = SPIFFS.open(CALIBRATION_FILE, "r");
        if (f) {
          if (f.readBytes((char *)calData, 14) == 14)
            calDataOK = 1;
          f.close();
        }
      }
    }

    if (calDataOK && !REPEAT_CAL) {
      // calibration data valid
      tft.setTouch(calData);
    } else {
      // data not valid so recalibrate
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(20, 0);
      tft.setTextFont(2);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      tft.println("Touch corners as indicated");

      tft.setTextFont(1);
      tft.println();

      if (REPEAT_CAL) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("Set REPEAT_CAL to false to stop this running again!");
      }

      tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.println("Calibration complete!");

      // store data
      File f = SPIFFS.open(CALIBRATION_FILE, "w");
      if (f) {
        f.write((const uint8_t *)calData, 14);
        f.close();
      }
    }
  }
}
