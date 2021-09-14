#include "Joystick.h"

// Create Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK,
  2, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

// Debug flag
  bool debug = 1;


// Variables for Stick Values
  int stick_x;
  int stick_y;
  int btn_1;
  int btn_2;

// Define the pins
  int pin_stick_x = A2;
  int pin_stick_y = A1;
  int pin_stick_btn = 3;
  int pin_button = 2;
  
// Timer variable for triggers
  unsigned long joystick_timer=0;
  int joystick_trigger = 20;
    
void setup() {
  Joystick.begin();
  
  //  Buttons
    pinMode(pin_stick_btn, INPUT_PULLUP);
    pinMode(pin_button, INPUT_PULLUP);
  // Start serial debug output
    while (!Serial);
      Serial.begin(115200);
      Serial.println(F("Startup"));
}

void loop() {
   // Process the Joystick
  if(millis() - joystick_timer > joystick_trigger)
  {
      joystick_timer = millis();
      
      stick_x = analogRead(pin_stick_x);
      stick_y = analogRead(pin_stick_y);
      btn_1 = digitalRead(pin_stick_btn);
      btn_2 = digitalRead(pin_button);

      if ( debug == 1 )
        {
          Serial.print(F("Stick_X: "));
          Serial.print(stick_x);
          Serial.print(F(" - Stick_Y: "));
          Serial.print(stick_y);
          Serial.print(F(" - BTN_1: "));
          Serial.print(btn_1);
          Serial.print(F(" - BTN_2: "));
          Serial.println(btn_2);
        };
  };
      Joystick.setXAxis(stick_x);
      Joystick.setYAxis(stick_y);
      if (  btn_1 == 1 )
        {
          Joystick.releaseButton(0);
        }
      else
        {
          Joystick.pressButton(0);
        }
      if (  btn_2 == 1 )
        {
          Joystick.releaseButton(1);
        }
      else
        {
          Joystick.pressButton(1);
        }
}
