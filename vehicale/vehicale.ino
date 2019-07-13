//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h> 

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN
#define RF_POWER RF24_PA_LOW

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};


// package structure
struct c_pack {
 int device_id;
 int value;
} pack;
#define LED 13 // PIN TO SHOW Received package

// device_id = 0 ---> speed.
// value ---> -32 .. 0 .. 32
#define I1 3  // Control pin 1 for motor 1 - DIGITAL
#define I2 4  // Control pin 2 for motor 1 - DIGITAL
#define E1 5  // Enable Pin for motor 1    - ANALOG    (PWM)

// device_id = 1 ---> turn
// value ---> -32 .. 0 .. 32
int device_1_pin = 6; //                     DIGITAL  (PWM)
Servo device_1;


void setup()
{
  //---------------- DEBUG ---------------------------
  Serial.begin(9600);
  // --------------- CONFIGURE DEVICE_0 --------------- 
  pinMode(E1, OUTPUT);
  pinMode(I1, OUTPUT);
  pinMode(I1, OUTPUT);
  // --------------- CONFIGURE DEVICE_1 --------------- 
  device_1.attach(device_1_pin);
  
  
  
  // --------------- CONFIGURE RADIO --------------- 
  radio.begin();
  radio.setPALevel(RF_POWER);
  //set the address
  radio.openWritingPipe(addresses[0]);   //write pipe as veh01
  radio.openReadingPipe(1,addresses[1]); //read pipe from con01
  
  //Set module as receiver
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
    //Serial.println(pack.device_id);
    //Serial.println(pack.value);
    if(pack.device_id == 0) {
      if(pack.value > -1 && pack.value < 1) {
        // STOP
        analogWrite(E1, 0);
        digitalWrite(I1, HIGH);
        digitalWrite(I2, HIGH);
      }
      else if(pack.value < -1) {
        // BACKWARD
       analogWrite(E1, pack.value*-8-1);
       digitalWrite(I1, HIGH);
       digitalWrite(I2, LOW);
      }
      else if(pack.value > 1) {
        // FORWARD
       analogWrite(E1, pack.value*8-1);
       digitalWrite(I1, LOW);
       digitalWrite(I2, HIGH);        
      }
    } else if (pack.device_id == 1) {
      // TURN
      device_1.write((32+pack.value)*3);
    }
    digitalWrite(LED, LOW);
  }
}
