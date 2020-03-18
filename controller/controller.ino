//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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

#define JOY_X_CORRECTION_OFFSET 100 // variables to compensate drift in joystick shield hardware (due imperfection of manufacturing)
#define JOY_Y_CORRECTION_OFFSET 100 // variables to compensate drift in joystick shield hardware (due imperfection of manufacturing)

#define JOY_X_THRESHOLD 1  // if output value less or equal to this threshold - it will be set to 0
#define JOY_Y_THRESHOLD 1  // if output value less or equal to this threshold - it will be set to 0

// --------- END CONFIGURATION ---------

int buttons[] = {UP_BTN, DOWN_BTN, LEFT_BTN, RIGHT_BTN, JOYSTICK_BTN, BTN_E, BTN_F};      // map buttons to ids
int button_states[] = {0,0,0,0,0,0,0};                                                    // store previus buttons position

//create an RF24 object
RF24 radio(9, 10);  // CE, CSN

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};

int channel = 0;

// package structure
struct c_pack {
 int device_id;
 int value;
} pack;


bool button_clicked(int button_id)
{
    if (button_states[button_id] != digitalRead(buttons[button_id])) {
      button_states[button_id] = digitalRead(buttons[button_id]);
      return true;
    }
    return false;
}

int read_joy(int joy_id, int offset=0, int threshold=0) {
  int out = round((analogRead(joy_id)+offset-512)/32);
  //int out = analogRead(joy_id);  //for debug purposes
  if(abs(out) <= threshold) return 0;
  return out;
}
  
void send_data() {
  radio.write(&pack, sizeof(pack));
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
  radio.setPALevel(RF24_PA_LOW);
  //set the address
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setChannel(channel);
  radio.stopListening();


  //------------ SETUP PINS ------------------------
  for (int i; i < 7; i++)  pinMode(buttons[i], INPUT_PULLUP);
}

void loop()
{

  pack.device_id = 7;
  pack.value = read_joy(JOYSTICK_AXIS_X, JOY_X_CORRECTION_OFFSET, JOY_X_THRESHOLD);
  send_data();
  delay(10);
  pack.device_id = 8;
  pack.value = read_joy(JOYSTICK_AXIS_Y, JOY_Y_CORRECTION_OFFSET, JOY_X_THRESHOLD);
  send_data();
  delay(10);

  for(int i=0;i<5;i++)
    if(button_clicked(i)) {
      pack.device_id = i;
      pack.value = button_states[i];
      send_data();
      delay(10);
    }

  if(button_clicked(6)) { // BTN_F
    loop_channel(-1);
  }
  else if(button_clicked(5)) {  // BTN_E
    loop_channel(1);
  }
}
