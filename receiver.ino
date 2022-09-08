#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

/*  Configurations/pins on the board
 */
#define SERVO_MOTOR_PIN   8

#define L298N_ENA         3 //
#define L298N_IN1         2 // Left motor control pins
#define L298N_IN2         4 //

#define L298N_ENB         6 //
#define L298N_IN3         5 // Right motor control pins
#define L298N_IN4         7 //

#define RADIO_PIN_CE      9
#define RADIO_PIN_CSN     10
#define RADIO_TX_ADDR     0x77B88
#define RADIO_RX_ADDR     0x77A88


/*  Servo
 */
Servo servo;
bool servoOpen = false;
void ServoMotorControl(int SW) {
  if (!SW) {
    return;
  }

  if (servoOpen) {
    servo.write(0);
  }
  else {
    servo.write(90);
  }
  servoOpen = !servoOpen;
}


/*  DC motors control via L298N driver
 */
void DCMotorsControl(int VRx, int VRy){
  int sRightMotor = ( abs(VRx) * 100 ) / ( VRy > 0 ? (abs(VRy) * 2) : 1 );
  int sLeftMotor  = ( abs(VRx) * 100 ) / ( VRy < 0 ? (abs(VRy) * 2) : 1 );

  digitalWrite(L298N_IN1, VRx > 0 ? HIGH : LOW);
  digitalWrite(L298N_IN2, VRx < 0 ? HIGH : LOW);
  
  digitalWrite(L298N_IN3, VRx > 0 ? HIGH : LOW);
  digitalWrite(L298N_IN4, VRx < 0 ? HIGH : LOW);
  
  analogWrite(L298N_ENA, sRightMotor);
  analogWrite(L298N_ENB, sLeftMotor);
}


/*  RF24 object init
 */
RF24 radio(RADIO_PIN_CE, RADIO_PIN_CSN); // CE, CSN


/*  Info object received from NRF24L01. Max size of this struct is 32 bytes - NRF24L01 buffer limit
 */
struct Joystick_Data {
  int VRx;
  int VRy;
  int SW;

  #define JOYSTICK_CTRL_BYTE   0xA3
  byte cb; //control byte
};
Joystick_Data jdata;


void setup() {
  Serial.begin(9600);

  servo.attach(SERVO_MOTOR_PIN);
  
  pinMode(L298N_ENA, OUTPUT);
  pinMode(L298N_IN1, OUTPUT);
  pinMode(L298N_IN2, OUTPUT);
  
  digitalWrite(L298N_IN1, LOW);
  digitalWrite(L298N_IN2, LOW);
  analogWrite(L298N_ENA, 0);
  
  digitalWrite(L298N_IN3, LOW);
  digitalWrite(L298N_IN4, LOW);
  analogWrite(L298N_ENB, 0);
  
  radio.begin();
  radio.openReadingPipe(0, RADIO_RX_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&jdata, sizeof(Joystick_Data));

    Serial.print("X: ");
    Serial.print(jdata.VRx);
    Serial.print(" | Y: ");
    Serial.print(jdata.VRy);
    Serial.print(" | Button: ");
    Serial.print(jdata.SW);
    Serial.print(" | Ctrl byte: ");
    Serial.println(jdata.cb);
  
    if (JOYSTICK_CTRL_BYTE == jdata.cb) {
      DCMotorsControl(jdata.VRx, jdata.VRy);
      ServoMotorControl(jdata.SW);
      delay (100);
    }
  }
}