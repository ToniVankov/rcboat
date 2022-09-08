## This project contains source code and info about RC boat controlled with Arduino.
  There is a two main parts in the project: the boat and the controller(joystick). 

> ## The Boat:
  The boat play the role of the receiver and its source file is called ''**receiver.ino**''.
  Hardware components used for it are:
  - Arduino Uno - used to control all components
  - L298N motor driver - used to contro the DC motors
  - nRF24L01 transceiver module - to receive data from the joystick
  - Two DC motors - for moving the boat
  - One servo motor - for release boxes of fish bait (as it will be used as fishing bait boat)
  - 3D printed body of the boat
  
  At the top of the file there is a config section that the user can change if needed.
  It is about which pin on Arduino board is connected to the rest peripherals.
  ```
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
  ```
  Both addresses for radio communication(RADIO_TX_ADDR and RADIO_RX_ADDR) are the 40-bit variable and can be any value whatsoever.
  For the boat only the receiver address(RADIO_RX_ADDR) is used.

* **Receiver-boat.c**

  Code for test Wireless transceiver NRF24L01 on the receiver.
