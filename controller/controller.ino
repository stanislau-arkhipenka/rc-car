//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int const UP_BTN = 2;
int const DOWN_BTN = 4;
int const LEFT_BTN = 5;
int const RIGHT_BTN = 3;
int const E_BTN = 6;
int const F_BTN = 7;
int const JOYSTICK_BTN = 8;
int const JOYSTICK_AXIS_X = A0;
int const JOYSTICK_AXIS_Y = A1;
int buttons[] = {UP_BTN, DOWN_BTN, LEFT_BTN, RIGHT_BTN, E_BTN, F_BTN, JOYSTICK_BTN};

//create an RF24 object
RF24 radio(9, 10);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00111";

// package structure
struct c_pack {
 int device_id;
 int value;
};

void setup()
{
  
  // SETUP RADIO
  radio.begin();
  //set the address
  radio.openWritingPipe(address);
  //Set module as transmitter
  radio.stopListening();
  
  
  // SETUP PINS
  for (int i; i < 7; i++)  pinMode(buttons[i], INPUT_PULLUP);
}
void loop()
{
  c_pack speed_pack;
  speed_pack.device_id = 0;
  speed_pack.value = round((analogRead(JOYSTICK_AXIS_X)-512)/32) + (digitalRead(UP_BTN) ? 16 : 0) + (digitalRead(DOWN_BTN) ? -16 : 0);
  radio.write(&speed_pack, sizeof(speed_pack));
  delay(25);
  c_pack turn_pack;
  turn_pack.device_id = 1;
  turn_pack.value = round((analogRead(JOYSTICK_AXIS_X)-512)/16);
  radio.write(&turn_pack, sizeof(turn_pack));
  delay(25);


}
