//Include statements go below
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

//Objects go below
TFT_eSPI tft; // Initialize the TFT display
TFT_eSPI_Button mainButton;

//define statements
#define CALIBRATION_FILE "/TouchCalData1" //This file stores Calibration Data
#define REPEAT_CAL false //If this is set to true, then calibration will be repeated on setup
#define SCREEN_ON_PIN 16 //Pin 16 will be used to turn the LED screen on and off

//Define the states of the machine
enum StateMachine {
  START_SCREEN,
  HOME_SCREEN,
  WORKER_SCREEN,
  SETTINGS_SCREEN,
};

bool startScreenTrigger = false;

//Create a structure for the button
struct Button {
  int x;
  int y;
  int width;
  int height;
  const char* label;
  uint16_t rectColor;
  uint16_t textColor;
  bool active;
};

const int maxButtons = 10;  // Maximum number of buttons
Button buttons[maxButtons];
int numButtons = 0;  // Actual number of buttons

// Initialize the state machine with the initial state
StateMachine screen = START_SCREEN;

//Add state machine repeat stopper
bool startStopper = false;
bool homeStopper = false;
bool workerStopper = false;
bool settingsStopper = false;

//add variable for blindsnumber for address
int blindsAddressNumber;

//------------------------------------------------------------------------------------------
void setup() {
  //The code below initializes the screen
  tft.init();
  tft.setRotation(0);
  pinMode(SCREEN_ON_PIN, OUTPUT);
  digitalWrite(SCREEN_ON_PIN, LOW);
  //The code below will calibrate the screen if REPEAT_CAL is true
  tft.fillScreen(TFT_BLACK);
  touch_calibrate();
  setStartScreen();
}

//------------------------------------------------------------------------------------------
void loop() {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  
  // State Machine Code Below
  switch (screen) {
    case START_SCREEN:
      if (startStopper == false) {
        setStartScreen();
        startStopper = true;
      }
      if (touched) {
        if (isButtonPressed(buttons[0], touchX, touchY)) {
          startStopper = false;
          clearButtons();
          screen = HOME_SCREEN;
          digitalWrite(SCREEN_ON_PIN, HIGH);
          break;
        }
      }
      break; // Removed redundant assignment: screen = START_SCREEN;

    case HOME_SCREEN:
      if (homeStopper == false) {
        setHomeScreen();
        homeStopper = true;
      }
      if (touched) {
        for (int i = 0; i < numButtons; i++) {
          if (isButtonPressed(buttons[i], touchX, touchY)) {
            homeStopper = false;
            clearButtons();
            blindsAddressNumber = i;
            if (i != 4) { // Adjusted the condition here
              screen = WORKER_SCREEN;
            } else {
              screen = SETTINGS_SCREEN;
            }
            break; // Added break to exit the loop once a button is pressed
          }
        }
      }
      break; // Removed redundant assignment: screen = HOME_SCREEN;

    case WORKER_SCREEN:
     if (workerStopper == false) {
        setWorkerScreen();
        workerStopper = true;
      }
      break; // Removed redundant assignment: screen = WORKER_SCREEN;

    case SETTINGS_SCREEN:
      setSettingsScreen();
      break; // Removed redundant assignment: screen = SETTINGS_SCREEN;
  }
}

//------------------------------------------------------------------------------------------
//The code below is in charge of screens
void setStartScreen() {
  tft.fillScreen(TFT_BLACK);
  addButton(0, 0, 240, 320, "Button 1", TFT_GREEN, TFT_RED);
  drawButtons();
}

void setHomeScreen() {
  tft.fillScreen(TFT_BLACK);
  addButton(0, 0, 240, 60, "Left-Left Blinds", TFT_GREEN, TFT_RED);
  addButton(0, 64, 240, 60, "Left-Right Blinds", TFT_GREEN, TFT_RED);
  addButton(0, 128, 240, 60, "Right-Left Blinds", TFT_GREEN, TFT_RED);
  addButton(0, 192, 240, 60, "Right-Right Blinds", TFT_GREEN, TFT_RED);
  addButton(0, 256, 240, 60, "Button 5", TFT_GREEN, TFT_RED);
  drawButtons();
}

void setWorkerScreen() {
  tft.fillScreen(TFT_BLACK);
  switch (blindsAddressNumber) {
    case 1:
        tft.drawString("Left-Left Blinds", 0, 0);
        break;
        
    case 2:
        tft.drawString("Left-Right Blinds", 0, 0);
        break;
        
    case 3:
        tft.drawString("Right-Left Blinds", 0, 0);
        break;
        
    case 4:
        tft.drawString("Right-Right Blinds", 0, 0);
        break;
    default:
        break;
  addButton(0, 64, 240, 60, "Open Blinds", TFT_GREEN, TFT_RED);
  addButton(0, 128, 240, 60, "Close Blinds", TFT_GREEN, TFT_RED);    
  drawButtons();  
}
  
}

void setSettingsScreen() {
  tft.fillScreen(TFT_BLACK);
}
//------------------------------------------------------------------------------------------
//The code below is in charge of buttons
//addButton allows you to dynamically add buttons
void addButton(int x, int y, int width, int height, const char* label, uint16_t rectColor, uint16_t textColor) {
  if (numButtons < maxButtons) {
    buttons[numButtons] = {x, y, width, height, label, rectColor, textColor};
    numButtons++;
  }
}

//draws all the buttons
void drawButtons() {
  for (int i = 0; i < numButtons; i++) {
    drawButton(buttons[i]);
  }
}

//draws a button
void drawButton(const Button &button) {
  tft.drawRect(button.x, button.y, button.width, button.height, button.rectColor);
  tft.setTextColor(button.textColor);
  
  int labelWidth = tft.textWidth(button.label);
  int labelHeight = tft.fontHeight(2);
  
  int labelX = button.x + (button.width - labelWidth) / 2;
  int labelY = button.y + (button.height - labelHeight) / 2;
  
  tft.drawString(button.label, labelX, labelY, 2);
}

//detects if a button is pressed
bool isButtonPressed(const Button &button, int touchX, int touchY) {
  if (touchX > button.x && touchX < button.x + button.width && touchY > button.y && touchY < button.y + button.height) {
    delay(200); // Debounce delay
    return true;
  }
  
  return false;
}

// Function to clear the buttons array
void clearButtons() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i] = {0, 0, 0, 0, "", 0, 0};
  }
  numButtons = 0;
}

//------------------------------------------------------------------------------------------
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
