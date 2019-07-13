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
#define RF_POWER RF24_PA_LOW

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};

// package structure
struct c_pack {
 int device_id;
 int value;
} pack;

void setup()
{
  // ------------ DEBUG ---------------------------
  Serial.begin(9600);
  
  //------------ SETUP RADIO ----------------------
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  //set the address
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.stopListening();
  
  
  //------------ SETUP PINS ------------------------
  for (int i; i < 7; i++)  pinMode(buttons[i], INPUT_PULLUP);
}
void loop()
{
  pack.device_id = 0;
  pack.value = round((analogRead(JOYSTICK_AXIS_X)-512)/32) + (digitalRead(UP_BTN) ? 16 : 0) + (digitalRead(DOWN_BTN) ? -16 : 0);
  if(!radio.write(&pack, sizeof(pack))) {
    Serial.println("failed speed control");
  }
  delay(25);
  pack.device_id = 1;
  pack.value = round((analogRead(JOYSTICK_AXIS_X)-512)/16);
  if(!radio.write(&pack, sizeof(pack))) {
    Serial.println("failed turn control");
  }
  delay(25);


}
