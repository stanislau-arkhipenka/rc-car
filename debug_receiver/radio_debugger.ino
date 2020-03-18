#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h> 

#define LED 13 // PIN TO SHOW Received package

#define CHANNEL 0
#define RF_POWER RF24_PA_LOW

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

char buttons[][9] = {"UPPP_BTN","DOWN_BTN","LEFT_BTN","RIGH_BTN","JOYS_BTN","EEEE_BTN","FFFF_BTN","XXX_AXIS","YYY_AXIS"};

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};


// package structure
struct c_pack {
 int device_id;
 int value;
} pack;

void setup()
{
  //---------------- DEBUG ---------------------------
  Serial.begin(9600);

  // --------------- CONFIGURE RADIO --------------- 
  radio.begin();
  radio.setPALevel(RF_POWER);
  //set the address
  radio.openWritingPipe(addresses[0]);   //write pipe as veh01
  radio.openReadingPipe(1,addresses[1]); //read pipe from con01
  
  radio.setChannel(CHANNEL);
  radio.startListening();
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
    digitalWrite(LED, HIGH);
    while(radio.available())
    {
      radio.read(&pack, sizeof(pack));
    }
    Serial.print(buttons[pack.device_id]);
    Serial.print(": ");
    Serial.println(pack.value);
    
    digitalWrite(LED, LOW);

  }
}
