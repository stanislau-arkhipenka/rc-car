//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h> 
#include <avr/wdt.h>

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN
#define RF_POWER RF24_PA_HIGH
#define CHANNEL 0

//address through which two modules communicate.
const byte addresses[][6] = {"veh01","con01"};


// TODO SELECT ANOTHER PIN!
#define DISABLE_WD_PIN 2
bool wd_enabled = false;

// package structure
struct c_pack {
 int b_up;
 int b_down;
 int b_left;
 int b_right;
 int b_joy;
 int joy_x; // -32 .. 32
 int joy_y; // -32 .. 32
} pack;

// MAX val
#define joy_max 32 

// ESC config
#define ESC_LEFT_PIN 5  //    DIGITAL PWM
#define ESC_RIGHT_PIN 6 //    DIGITAL PWM
#define ESC_MIN_W 1000 
#define ESC_MAX_W 2000
Servo esc_left;
Servo esc_right;


void setup()
{
  //---------------- DEBUG ---------------------------
  Serial.begin(9600);
  Serial.println("Starting");

  // --------------- CONFIGURE RADIO --------------- 
  radio.begin();
  radio.setPALevel(RF_POWER);
  //set the address
  radio.openWritingPipe(addresses[0]);   //write pipe as veh01
  radio.openReadingPipe(1,addresses[1]); //read pipe from con01
  radio.setChannel(CHANNEL);
  //Set module as receiver
  radio.startListening();

  // --------------- CONFIGURE ESC --------------- 
  esc_left.attach(ESC_LEFT_PIN, ESC_MIN_W, ESC_MAX_W);
  esc_right.attach(ESC_RIGHT_PIN, ESC_MIN_W, ESC_MAX_W);

  led_delay(1500);

  // --------------- CONFIGURE WATCHDOG --------------- 
  pinMode(DISABLE_WD_PIN, INPUT_PULLUP);
  if (digitalRead(DISABLE_WD_PIN) == HIGH) {
    wdt_enable(WDTO_2S);
    wd_enabled = true;
    Serial.println("Watchdog enabled");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Watchdog disabled");
  }
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
    while(radio.available())
    {
      radio.read(&pack, sizeof(pack));
    }
    int left_value = map(max(min(pack.joy_y + pack.joy_x, joy_max), -joy_max), -joy_max, joy_max, ESC_MIN_W, ESC_MAX_W);
    int right_value = map(max(min(pack.joy_y - pack.joy_x, joy_max), -joy_max),-joy_max, joy_max, ESC_MIN_W, ESC_MAX_W);
    esc_left.writeMicroseconds(left_value);
    esc_right.writeMicroseconds(right_value);
    Serial.print(left_value);
    Serial.print(" ");
    Serial.print(right_value);
    Serial.println("");
    
    if (wd_enabled == true) {
      wdt_reset();
    }
  }
}

void led_delay(int t) {
  bool on = false;
  static int step = 100;
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i=0;i<t;i+=step)
  {
    if (on == true) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    on = !on;
    delay(step);
    if (wd_enabled == true) {
      wdt_reset();
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}