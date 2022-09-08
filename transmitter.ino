#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


/*  Configurations/pins on the board
 */
#define  JOYSTICK_VRx   A4
#define  JOYSTICK_VRy   A3
#define  JOYSTICK_SW    2

#define  RADIO_PIN_CE   8
#define  RADIO_PIN_CSN  9
#define  RADIO_TX_ADDR  0x77A88
#define  RADIO_RX_ADDR  0x77B88


/*  Info object transmited via NRF24L01. Max size of this struct is 32 bytes - NRF24L01 buffer limit
 */
struct Joystick_Data {
  int VRx = 0;
  int VRy = 0;
  int SW  = 0;

  #define JOYSTICK_CTRL_BYTE   0xA3
  byte cb = JOYSTICK_CTRL_BYTE; //control byte
};
Joystick_Data jdata;


/*  RF24 object init
 */
RF24 radio(RADIO_PIN_CE, RADIO_PIN_CSN); // CE, CSN


int joystick_calc_dir(int vr) {
  switch (vr) {
    case 0   ... 100:  return -2;
    case 101 ... 461:  return -1;
    case 462 ... 562:  return  0;
    case 563 ... 922:  return  1;
    case 923 ... 1023: return  2;

    default: return 0;
  }
}

bool joystick_check() {

  int VRx  = analogRead(JOYSTICK_VRx);
  int VRy  = analogRead(JOYSTICK_VRy);
  int SW   = digitalRead(JOYSTICK_SW) ? 0 : 1;

  int mapX = joystick_calc_dir(VRx);
  int mapY = joystick_calc_dir(VRy);
  
  if ((mapX != jdata.VRx) ||
      (mapY != jdata.VRy) ||
      (SW   != jdata.SW))
  {
    jdata.VRx = mapX;
    jdata.VRy = mapY;
    jdata.SW = SW;
  
    Serial.print("X: [");
    Serial.print(VRx);
    Serial.print("] ");
    Serial.print(jdata.VRx);
    Serial.print('\n');
    Serial.print("Y: [");
    Serial.print(VRy);
    Serial.print("] ");
    Serial.print(jdata.VRy);
    Serial.print('\n');
    Serial.print("btn: ");
    Serial.print(jdata.SW);
    Serial.print('\n');
    Serial.print("ctrl byte: ");
    Serial.print(jdata.cb);
    Serial.print('\n');
    
    return true;
  }
  return false;
}


void setup() {
  Serial.begin(9600); 
  
  pinMode(JOYSTICK_VRx, INPUT);
  pinMode(JOYSTICK_VRy, INPUT);
  pinMode(JOYSTICK_SW,  INPUT_PULLUP); 
  
  radio.begin();
  radio.openWritingPipe(RADIO_TX_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  if (joystick_check()) {
    radio.write(&jdata, sizeof(Joystick_Data));
  }
  delay(100);
}
