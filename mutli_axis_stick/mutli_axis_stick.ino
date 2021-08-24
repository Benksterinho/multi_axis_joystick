#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

// Debug flag
  bool debug_axis = 0;
  bool debug_btn = 0;
  bool debug_switch = 0;
  bool debug_rot_encoder = 1;
  
// Define Pin Variables
// Multiplexers
  int pin_mux_ctl_1 = 0;
  int pin_mux_ctl_2 = 1;
  int pin_mux_ctl_3 = 2;
  int pin_mux_ctl_4 = 3;
  int pin_sig_joy = 20;
  int pin_sig_btn = 21;

//Rotary Encoder
  int pin_enc_dt = 4;
  int pin_enc_clk = 5;
  int pin_enc_button = 6;

//toggle switches
  int pin_switch_1=7;
  int pin_switch_2=8;
  int pin_switch_3=9;
  int pin_switch_4=13;
  int pin_switch_5=14;
  int pin_switch_6=15;
  int pin_switch_7=16;
  int pin_switch_8=17;

// Variables for encoder
Encoder enc_knob(pin_enc_dt, pin_enc_clk);
long enc_last_pos = -999;
int enc_mode=1;

//Initialize display.
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for Stick Values
  int data_L1_x;
  int data_L1_y;
  int data_L1_z;
  int data_L2_x;
  int data_L2_y;
  int data_R_x;
  int data_R_y;
  int data_R_z;
  int data_SL;
  int data_hat_angle;
  int data_per_SL;
  
  
// Variables for Buttons
  const int numButtons = 14;
  byte allButtons[numButtons];
  int button_map[numButtons];
  
// Variables for switches
  int val_switch_1;
  int val_switch_2;
  int val_switch_3;
  int val_switch_4;
  int val_switch_5;
  int val_switch_6;
  int val_switch_7;
  int val_switch_8;
  int last_val_switch_1 = 0;
  int last_val_switch_2 = 0;
  int last_val_switch_3 = 0;
  int last_val_switch_4 = 1;
  int last_val_switch_5 = 1;
  int last_val_switch_6 = 1;
  int last_val_switch_7 = 1;
  int last_val_switch_8 = 1;

// Variables for Keyboard
  int key_code;
  bool L1_key_pressed=0;
  bool L2_key_pressed=0;  
  
// Variables for Mouse
  float mouse_speed = 0;
  float mouse_x = 0;
  float mouse_y = 0;

  bool mouse_btn_left = 0;
  bool mouse_btn_middle = 0;
  bool mouse_btn_right = 0;

// Timer variable for triggers
  unsigned long joystick_timer=0;
  unsigned long display_timer=0;
  unsigned long encoder_timer=0;

void setup()
  {
     pinMode(pin_sig_btn, INPUT_PULLUP);
     
  // MUX Pins
    pinMode(pin_mux_ctl_1, OUTPUT); 
    pinMode(pin_mux_ctl_2, OUTPUT); 
    pinMode(pin_mux_ctl_3, OUTPUT); 
    pinMode(pin_mux_ctl_4, OUTPUT);

    digitalWrite(pin_mux_ctl_1, LOW);
    digitalWrite(pin_mux_ctl_2, LOW);
    digitalWrite(pin_mux_ctl_3, LOW);
    digitalWrite(pin_mux_ctl_4, LOW);

  // Switch Buttons
    pinMode(pin_switch_1, INPUT_PULLUP);
    pinMode(pin_switch_2, INPUT_PULLUP);
    pinMode(pin_switch_3, INPUT_PULLUP);
    pinMode(pin_switch_4, INPUT_PULLUP);
    pinMode(pin_switch_5, INPUT_PULLUP);
    pinMode(pin_switch_6, INPUT_PULLUP);
    pinMode(pin_switch_7, INPUT_PULLUP);
    pinMode(pin_switch_8, INPUT_PULLUP);

  // Ecoder button
  pinMode(pin_enc_button, INPUT_PULLUP);
  
  
  // Start serial debug output
    while (!Serial);
      Serial.begin(115200);
      Serial.println(F("Startup"));
      
  // Joystick set to manual
    Joystick.useManualSend(true);
    
// Check and set display address
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear display buffer
  display.clearDisplay();

  // Button map
  button_map[0]=8;
  button_map[1]=7;
  button_map[2]=6;
  button_map[3]=5;
  button_map[4]=4;
  button_map[5]=3;
  button_map[6]=1;
  button_map[7]=14;
  button_map[8]=10;
  button_map[9]=11;
  button_map[10]=12;
  button_map[11]=13;
  button_map[12]=2;
  button_map[13]=9;
}


void loop(){
  // Process the Joystick
  if(millis() - joystick_timer > 20){
      joystick_timer = millis();
      
      get_values_switch();
      get_values_joystick();
      
      set_joystick_buttons();
      set_joystick_axis();

      if( val_switch_1 == 0 ) L2_arrows();
      if( val_switch_1 == 1 ) L2_wasd();

      
      enc_button_func(digitalRead(pin_enc_button));
      data_per_SL = map(data_SL,0,1023,0,100); 
      Joystick.send_now();

  // Switch 1
  switch (val_switch_1)
    {
      case 0:
        display.print("Arrows");
        break;
      case 1:
        display.print("wasd");
        break;
    }
  // Switch 2
      if ( val_switch_2 != last_val_switch_2 )
      {
        Keyboard.press(KEY_K);
        Keyboard.release(KEY_K);
        last_val_switch_2 = val_switch_2;
      }
  // Switch 3
      if ( val_switch_3 != last_val_switch_3 )
      {
        Keyboard.press(KEY_M);
        Keyboard.release(KEY_M);
        last_val_switch_3 = val_switch_3;
      }
  // Switch 4
      if ( val_switch_4 != last_val_switch_4 )
      {
        Keyboard.press(KEY_INSERT);
        Keyboard.release(KEY_INSERT);
        last_val_switch_4 = val_switch_4;
      }
  // Switch 5
      if ( val_switch_5 != last_val_switch_5 )
      {
        Keyboard.press(KEY_DELETE);
        Keyboard.release(KEY_DELETE);
        last_val_switch_5 = val_switch_5;
      }
  // Switch 6
      if ( val_switch_6 != last_val_switch_6 )
      {
        Keyboard.press(KEY_I);
        Keyboard.release(KEY_I);
        last_val_switch_6 = val_switch_6;
      }
  // Switch 7
      if ( val_switch_7 != last_val_switch_7 )
      {
        Keyboard.press(KEY_N);
        Keyboard.release(KEY_N);
        last_val_switch_7 = val_switch_7;
      }
  // Switch 8
      if ( val_switch_8 != last_val_switch_8 )
      {
        Keyboard.press(KEY_L);
        Keyboard.release(KEY_L);
        last_val_switch_8 = val_switch_8;
      }
  }
  
 // Process the rotary encoder
  if(millis() - encoder_timer > 500){
    encoder_timer = millis();
    
    switch (enc_mode) 
      {
        case 1:
          enc_arrows_pgup_pgdwn(get_values_rot_encoder());
          break;
        case 2:
          enc_arrows_home_end(get_values_rot_encoder());
          break;
        case 3:
          enc_arrows_insert_delete(get_values_rot_encoder());
          break;
        default:
          enc_arrows_lr(get_values_rot_encoder());
          break;
      }
  }

  // Process the OLED Display
  if(millis() - display_timer > 20){
    display_timer = millis();

    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Mode: ");
    switch (enc_mode) 
      {
        case 1:
          display.print("Radar");
          break;
        case 2:
          display.print("Target");
          break;
        case 3:
          display.print("Utilties");
          break;
        default:
          enc_arrows_lr(get_values_rot_encoder());
          break;
      }
    display.setCursor(0, 10);
 
    drawProgressbar(0, 30, 128, 20, data_per_SL);
    display.drawLine(64, 25, 64, 55, WHITE);
    display.display();  
  }
}

void drawProgressbar(int x,int y, int width,int height, int progress)
{
   progress = progress > 100 ? 100 : progress; // set the progress value to 100
   progress = progress < 0 ? 0 :progress; // start the counting to 0-100
   float bar = ((float)(width-1) / 100) * progress;
   display.drawRect(x, y, width, height, WHITE);
   display.fillRect(x+2, y+2, bar , height-4, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
}

void enc_button_func(int new_button_val){
  static int old_button_val;
  if (new_button_val != old_button_val){
    if ( new_button_val == 0)
    {
      enc_mode=enc_mode+1;
      if (enc_mode > 2) enc_mode=1;
    }
    old_button_val = new_button_val;
  }  
}

void set_joystick_axis(){
  Joystick.X(data_R_x);
  Joystick.Y(data_R_y);
  Joystick.Z(1023-data_SL);
  Joystick.Zrotate(data_R_z);
  Joystick.sliderLeft(data_L1_x);
  Joystick.sliderRight(data_L1_y);
}

void set_joystick_buttons(){
  for (int i=0; i<numButtons; i++) {
 /*   Serial.print(F("Btn "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(allButtons[i]);
    Serial.print(F(" | "));*/
    if (allButtons[i] < 100) 
      { 
        if ( debug_btn == 1 )
          {
            Serial.print(F("Btn "));
            Serial.print(i);
            Serial.print(" - PRESSED ");
            Serial.print(F(" | "));
            Serial.println();
          }

        Joystick.button(button_map[i], 1);
      } 
    if (allButtons[i] >= 100)
     {
      Joystick.button(button_map[i], 0);
     }
  }
  //Serial.println(F(" | "));
}
  
int readMux_JOY(int channel){
  int controlPin[] = {pin_mux_ctl_1, pin_mux_ctl_2, pin_mux_ctl_3, pin_mux_ctl_4};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(pin_sig_joy);

  //return the value
  return val;
}

int readMux_BTN(int channel){
  int controlPin[] = {pin_mux_ctl_1, pin_mux_ctl_2, pin_mux_ctl_3, pin_mux_ctl_4};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  
  //read the value at the SIG pin
  int val = analogRead(pin_sig_btn);

  //return the value
  return val;
}

int log_2_lin(int log_val)
{
    int val = 0;
    if (log_val >= 512) val = 9;
    else if (log_val >= 256) val = 8;
    else if (log_val >= 128) val = 7;
    else if (log_val >= 64) val = 6;
    else if (log_val >= 20) val = 5;
    else if (log_val >= 16) val = 4;
    else if (log_val >= 8) val = 3;
    else if (log_val >= 4) val = 2;
    else if (log_val >= 2) val = 1;
    return val;
}

void L2_wasd()
{
    if ( data_L2_y < 200 && data_L2_x < 800 && data_L2_x > 200 )
    {
      // forward
      key_code=1;
      Keyboard.press(KEY_W);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 800 && data_L2_y < 800 && data_L2_y > 200 )
    {
      // right
      key_code=2;
      Keyboard.press(KEY_D);
      L2_key_pressed=1;
     }
   else if ( data_L2_y > 800 && data_L2_x < 800 && data_L2_x > 200 ) 
    {
       // back
      key_code=3;
      Keyboard.press(KEY_S);
      L2_key_pressed=1;
     }
   else if ( data_L2_x < 200 && data_L2_y < 800 && data_L2_y > 200 ) 
    {
      // left
      key_code=4;
      Keyboard.press(KEY_A);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 200 && data_L2_x < 800 && data_L2_y > 200 && data_L2_y < 800 ) 
   {
      if( L2_key_pressed == 1  )
      {
        switch (key_code) 
        {
          case 1:
            Keyboard.release(KEY_W);
          break;
          case 2:
            Keyboard.release(KEY_D);
          break;
          case 3:
            Keyboard.release(KEY_S);
          break;
          case 4:
            Keyboard.release(KEY_A);
          break;
          default:
          // Statement(s)
          break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
        }
        L2_key_pressed=0;
      }
   };
}

 void L2_special()
{
    if ( data_L2_y < 200 && data_L2_x < 800 && data_L2_x > 200 )
    {
      // forward
      key_code=1;
      Keyboard.press(KEY_HOME);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 800 && data_L2_y < 800 && data_L2_y > 200 )
    {
      // right
      key_code=2;
      Keyboard.press(KEY_PAGE_UP);
      L2_key_pressed=1;
     }
   else if ( data_L2_y > 800 && data_L2_x < 800 && data_L2_x > 200 ) 
    {
       // back
      key_code=3;
      Keyboard.press(KEY_END);
      L2_key_pressed=1;
     }
   else if ( data_L2_x < 200 && data_L2_y < 800 && data_L2_y > 200 ) 
    {
      // left
      key_code=4;
      Keyboard.press(KEY_PAGE_DOWN);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 200 && data_L2_x < 800 && data_L2_y > 200 && data_L2_y < 800 ) 
   {
      if( L2_key_pressed == 1  )
      {
        switch (key_code) 
        {
          case 1:
            Keyboard.release(KEY_HOME);
          break;
          case 2:
            Keyboard.release(KEY_PAGE_UP);
          break;
          case 3:
            Keyboard.release(KEY_END);
          break;
          case 4:
            Keyboard.release(KEY_PAGE_DOWN);
          break;
          default:
          // Statement(s)
          break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
        }
        L2_key_pressed=0;
      }
   }
};

 void L2_arrows()
{
    if ( data_L2_y < 200 && data_L2_x < 800 && data_L2_x > 200 )
    {
      // forward
      key_code=1;
      Keyboard.press(KEY_UP);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 800 && data_L2_y < 800 && data_L2_y > 200 )
    {
      // right
      key_code=2;
      Keyboard.press(KEY_RIGHT);
      L2_key_pressed=1;
     }
   else if ( data_L2_y > 800 && data_L2_x < 800 && data_L2_x > 200 ) 
    {
       // back
      key_code=3;
      Keyboard.press(KEY_DOWN);
      L2_key_pressed=1;
     }
   else if ( data_L2_x < 200 && data_L2_y < 800 && data_L2_y > 200 ) 
    {
      // left
      key_code=4;
      Keyboard.press(KEY_LEFT);
      L2_key_pressed=1;
     }
   else if ( data_L2_x > 200 && data_L2_x < 800 && data_L2_y > 200 && data_L2_y < 800 ) 
   {
      if( L2_key_pressed == 1  )
      {
        switch (key_code) 
        {
          case 1:
            Keyboard.release(KEY_UP);
          break;
          case 2:
            Keyboard.release(KEY_RIGHT);
          break;
          case 3:
            Keyboard.release(KEY_DOWN);
          break;
          case 4:
            Keyboard.release(KEY_LEFT);
          break;
          default:
          // Statement(s)
          break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
        }
        L2_key_pressed=0;
      }
   };
  }
  
void L1_hat()
{
  if ( data_L1_y < 200 && data_L1_x < 800 && data_L1_x > 200 ) data_hat_angle=0;
  else if ( data_L1_y < 200 && data_L1_x > 800 ) data_hat_angle=45;
  else if ( data_L1_x > 800 && data_L1_y < 800 && data_L1_y > 200 ) data_hat_angle=90;
  else if ( data_L1_x > 800 && data_L1_y > 800 ) data_hat_angle=135;
  else if ( data_L1_y > 800 && data_L1_x < 800 && data_L1_x > 200 ) data_hat_angle=180;
  else if ( data_L1_x < 200 && data_L1_y > 800 ) data_hat_angle=225;
  else if ( data_L1_x < 200 && data_L1_y < 800 && data_L1_y > 200 ) data_hat_angle=270;
  else if ( data_L1_x < 200 && data_L1_y < 200 )data_hat_angle=315;
  else if ( data_L1_x > 200 && data_L1_x < 800 && data_L1_y > 200 && data_L1_y < 800 )data_hat_angle=-1;
  Joystick.hat(data_hat_angle);
}

 void L1_arrows()
{
    if ( data_L1_y < 200 && data_L1_x < 800 && data_L1_x > 200 )
    {
      // forward
      key_code=1;
      Keyboard.press(KEY_UP);
      L1_key_pressed=1;
     }
   else if ( data_L1_x > 800 && data_L1_y < 800 && data_L1_y > 200 )
    {
      // right
      key_code=2;
      Keyboard.press(KEY_RIGHT);
      L1_key_pressed=1;
     }
   else if ( data_L1_y > 800 && data_L1_x < 800 && data_L1_x > 200 ) 
    {
       // back
      key_code=3;
      Keyboard.press(KEY_DOWN);
      L1_key_pressed=1;
     }
   else if ( data_L1_x < 200 && data_L1_y < 800 && data_L1_y > 200 ) 
    {
      // left
      key_code=4;
      Keyboard.press(KEY_LEFT);
      L1_key_pressed=1;
     }
   else if ( data_L1_x > 200 && data_L1_x < 800 && data_L1_y > 200 && data_L1_y < 800 ) 
   {
      if( L1_key_pressed == 1  )
      {
        switch (key_code) 
        {
          case 1:
            Keyboard.release(KEY_UP);
          break;
          case 2:
            Keyboard.release(KEY_RIGHT);
          break;
          case 3:
            Keyboard.release(KEY_DOWN);
          break;
          case 4:
            Keyboard.release(KEY_LEFT);
          break;
          default:
          // Statement(s)
          break; 
        }
        L1_key_pressed=0;
      }
   };
  }

void L1_wasd()
{
    if ( data_L1_y < 200 && data_L1_x < 800 && data_L1_x > 200 )
    {
      // forward
      key_code=1;
      Keyboard.press(KEY_W);
      L1_key_pressed=1;
     }
   else if ( data_L1_x > 800 && data_L1_y < 800 && data_L1_y > 200 )
    {
      // right
      key_code=2;
      Keyboard.press(KEY_D);
      L1_key_pressed=1;
     }
   else if ( data_L1_y > 800 && data_L1_x < 800 && data_L1_x > 200 ) 
    {
       // back
      key_code=3;
      Keyboard.press(KEY_S);
      L1_key_pressed=1;
     }
   else if ( data_L1_x < 200 && data_L1_y < 800 && data_L1_y > 200 ) 
    {
      // left
      key_code=4;
      Keyboard.press(KEY_A);
      L1_key_pressed=1;
     }
   else if ( data_L1_x > 200 && data_L1_x < 800 && data_L1_y > 200 && data_L1_y < 800 ) 
   {
      if( L1_key_pressed == 1  )
      {
        switch (key_code) 
        {
          case 1:
            Keyboard.release(KEY_W);
          break;
          case 2:
            Keyboard.release(KEY_D);
          break;
          case 3:
            Keyboard.release(KEY_S);
          break;
          case 4:
            Keyboard.release(KEY_A);
          break;
          default:
          // Statement(s)
          break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
        }
        L1_key_pressed=0;
      }
   };
}

long get_values_rot_encoder(){
  long new_enc_pos;
  new_enc_pos = enc_knob.read();
  return new_enc_pos;
}

void enc_arrows_lr(long new_value){
  static long old_value;
  if ( new_value != old_value) {
    if ( new_value < old_value ){
      // Turn Left
      Keyboard.press(KEY_LEFT);
      Keyboard.release(KEY_LEFT);
      }
    else {
      // Turn Right
      Keyboard.press(KEY_RIGHT);
      Keyboard.release(KEY_RIGHT);
    }
    if ( debug_rot_encoder == 1 )
  {
    Serial.println(new_value);
  } 
    old_value = new_value;
  }
}

void enc_arrows_home_end(long new_value){
  static long old_value;
  if ( new_value != old_value) {
    if ( new_value < old_value ){
      // Turn Left
      Keyboard.press(KEY_END);
      Keyboard.release(KEY_END);
      }
    else {
      // Turn Right
      Keyboard.press(KEY_HOME);
      Keyboard.release(KEY_HOME);
    }
    if ( debug_rot_encoder == 1 )
  {
    Serial.println(new_value);
  } 
    old_value = new_value;
  }
}

void enc_arrows_insert_delete(long new_value){
  static long old_value;
  if ( new_value != old_value) {
    if ( new_value < old_value ){
      // Turn Left
      Keyboard.press(KEY_DELETE);
      Keyboard.release(KEY_DELETE);
      }
    else {
      // Turn Right
      Keyboard.press(KEY_INSERT);
      Keyboard.release(KEY_INSERT);
    }
    if ( debug_rot_encoder == 1 )
  {
    Serial.println(new_value);
  } 
    old_value = new_value;
  }
}

void enc_arrows_pgup_pgdwn(long new_value){
  static long old_value;
  if ( new_value != old_value) {
    if ( new_value < old_value ){
      // Turn Left
      Keyboard.press(KEY_PAGE_DOWN);
      Keyboard.release(KEY_PAGE_DOWN);
      }
    else {
      // Turn Right
      Keyboard.press(KEY_PAGE_UP);
      Keyboard.release(KEY_PAGE_UP);
    }
    if ( debug_rot_encoder == 1 )
  {
    Serial.println(new_value);
  } 
    old_value = new_value;
  }
}

void get_values_switch(){
  // Get value of switches
    val_switch_1=digitalRead(pin_switch_1);
    val_switch_2=digitalRead(pin_switch_2);
    val_switch_3=digitalRead(pin_switch_3);
    val_switch_4=digitalRead(pin_switch_4);
    val_switch_5=digitalRead(pin_switch_5);
    val_switch_6=digitalRead(pin_switch_6);
    val_switch_7=digitalRead(pin_switch_7);
    val_switch_8=digitalRead(pin_switch_8);
    
  if ( debug_switch == 1 )
  {
    Serial.print("Switch 1: ");
    Serial.print(val_switch_1);
    Serial.print(" - Switch 2: ");
    Serial.print(val_switch_2);
    Serial.print(" - Switch 3: ");
    Serial.println(val_switch_3);
  } 
}

void get_values_joystick(){
  for(int i = 0; i < 15; i ++)
  {
    switch (i) 
    {
      case 0:
        data_L1_x = readMux_JOY(i);
      break;
      
      case 1:
        data_L1_y = readMux_JOY(i);
      break;
      
      case 2:
       data_L1_z = map(log_2_lin(readMux_JOY(i)),0,9,0,1023);
       
      break;

      case 3:
        data_L2_x = readMux_JOY(i);
      break;
      
      case 4:
         data_L2_y = readMux_JOY(i);
      break;

      case 5:
         data_SL = readMux_JOY(i);
      break;

      case 6:
        data_R_x = readMux_JOY(i);
      break;
      
      case 7:
        data_R_y = readMux_JOY(i);
      break;
      
      case 8:
        data_R_z = map(log_2_lin(readMux_JOY(i)),0,9,0,1023);
      break;
      }
     allButtons[i] = readMux_BTN(i);
    };

  if ( debug_axis == 1 )
    {
      Serial.print(F("L1_X: "));
      Serial.print(data_L1_x);
      Serial.print(F(" - L1_Y: "));
      Serial.print(data_L1_y);
      Serial.print(F(" - L1_Z: "));
      Serial.print(data_L1_z);
      Serial.print(F(" | "));
      Serial.print(F(" - L2_X: "));
      Serial.print(data_L2_x);
      Serial.print(F(" - L1_Y: "));
      Serial.print(data_L2_y);
      Serial.print(F(" | "));
      Serial.print(F(" - SL: "));
      Serial.print(data_SL);
      Serial.print(F(" | "));
      Serial.print(F("R_X: "));
      Serial.print(data_R_x);
      Serial.print(F(" - R_Y: "));
      Serial.print(data_R_y);
      Serial.print(F(" - R_Z: "));
      Serial.println(data_R_z);
    };
}
