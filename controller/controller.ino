//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define UP_BTN          2 // 0
#define DOWN_BTN        4 // 1
#define LEFT_BTN        5 // 2
#define RIGHT_BTN       3 // 3
#define JOYSTICK_BTN    8 // 4
#define BTN_E           6 // 5
#define BTN_F           7 // 6

#define JOYSTICK_AXIS_X A0   // 7
#define JOYSTICK_AXIS_Y A1   // 8

// ---------- CONFIGURATION ----------
#define RF_POWER RF24_PA_LOW // TRANSMISSION POWER. Possible values: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

// --------- END CONFIGURATION ---------

int buttons[] = {UP_BTN, DOWN_BTN, LEFT_BTN, RIGHT_BTN, JOYSTICK_BTN, BTN_E, BTN_F};      // map buttons to ids
int button_states[] = {0,0,0,0,0,0,0};                                                    // store previus buttons position

//create an RF24 object
RF24 radio(9, 10);  // CE, CSN

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};


// --------- RUNTIME CONFIGS ------------
int channel = 0;
int x_val = 0;
int y_val = 0;

int calibr_conf_x[3] = {0,0,0}; // center, min, max
int calibr_conf_y[3] = {0,0,0}; // center, min, max

int mode = 0; // 0 - calibration, 1 - operation

// package structure
struct c_pack {
 int b_up;
 int b_down;
 int b_left;
 int b_right;
 int b_joy;
 int joy_x;
 int joy_y;
} pack;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool button_clicked(int button_id)
{
    if (button_states[button_id] != digitalRead(buttons[button_id])) {
      button_states[button_id] = digitalRead(buttons[button_id]);
      if (digitalRead(buttons[button_id]) == LOW) {
        return true;
      }
    }
    return false;
}

int raw_read_joy(int joy_id)
{
  return analogRead(joy_id)-512;
}

int read_joy(int joy_id, int joy_conf[3]) {
  int out = map(raw_read_joy(joy_id)-joy_conf[0],joy_conf[1],joy_conf[2],-32,32);
  //int out = analogRead(joy_id);  //for debug purposes
  return out;
}
  
void send_data() {
  radio.write(&pack, sizeof(pack));
}

void draw_display() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("chan: "); display.println(channel);
  display.print("X:"); display.print(x_val); display.print("Y:"); display.println(y_val);
  display.display();

}

void loop_channel(int direction) {
  channel += direction;
  if (channel > 124)
    channel = 0;
  else if (channel < 0)
    channel = 125;
  radio.setChannel(channel);
  delay(10);
}

void setup()
{
  //------------ SETUP RADIO ----------------------
  radio.begin();
  radio.setPALevel(RF_POWER);
  //set the address
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setChannel(channel);
  radio.stopListening();

  //------------ SETUP PINS ------------------------
  for (int i; i < 7; i++)  pinMode(buttons[i], INPUT_PULLUP);

  //------------ SETUP DISPLAY --------------------
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

}

void loop()
{
  if (mode == 0) {
    calibrate();
  }
  else
  {
    x_val = read_joy(JOYSTICK_AXIS_X, calibr_conf_x);
    y_val = read_joy(JOYSTICK_AXIS_Y, calibr_conf_y);
    pack.b_up = digitalRead(UP_BTN);
    pack.b_down = digitalRead(DOWN_BTN);
    pack.b_left = digitalRead(LEFT_BTN);
    pack.b_right = digitalRead(RIGHT_BTN);
    pack.b_joy = digitalRead(JOYSTICK_BTN);
    pack.joy_x = x_val;
    pack.joy_y = y_val;
    
    send_data();
    delay(10);
  
    if(button_clicked(6)) {
      loop_channel(-1);
    }
    else if(button_clicked(5)) {
      loop_channel(1);
    }
    draw_display();
  }
}


void calibrate()
{
  bool tmp_b = true;
  while(tmp_b) {
      calibr_conf_x[0] = raw_read_joy(JOYSTICK_AXIS_X);
      calibr_conf_y[0] = raw_read_joy(JOYSTICK_AXIS_Y);
    if (button_clicked(0))
      tmp_b = false;

    if (button_clicked(3)) { // cancel calibration
      calibr_conf_x[1] = -512;
      calibr_conf_y[1] = -512;
      calibr_conf_x[2] = 512;
      calibr_conf_y[2] = 512;
      mode = 1;
      return;
    }

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("CENTER");
    display.println("A ok, B no");
    display.print(raw_read_joy(JOYSTICK_AXIS_X)); display.print(", "); display.println(raw_read_joy(JOYSTICK_AXIS_Y));
    display.display();
  }
  tmp_b = true;
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("MAX VALUES");
  display.println("A - ok");
  display.display();
  while(tmp_b) {
    if (button_clicked(0))
      tmp_b = false;
      x_val = raw_read_joy(JOYSTICK_AXIS_X);
      y_val = raw_read_joy(JOYSTICK_AXIS_Y);

      if (calibr_conf_x[1] > x_val)
        calibr_conf_x[1] = x_val;
      if (calibr_conf_x[2] < x_val)
        calibr_conf_x[2] = x_val;
      if (calibr_conf_y[1] > y_val)
        calibr_conf_y[1] = y_val;
      if (calibr_conf_y[2] < y_val)
        calibr_conf_y[2] = y_val; 
  }
  mode = 1;
  
}
