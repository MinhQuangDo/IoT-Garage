#if 1
#include "Arduino.h"

#include"GarageHardware.h"

/**
 * Setup the door hardware (all I/O should be configured here)
 *
 * This routine should be called only once from setup()
 */
 const int doorController = D3;
 const int faultSignal = D4;
 const int openSwitch = D5;
 const int closeSwitch = D6;
 const int closingLED = D2;
 const int openingLED = D1;
 const int garageLight = D0;

void setupHardware() {
  Serial.begin(9600);
  pinMode(doorController, INPUT_PULLUP);
  pinMode(faultSignal, INPUT_PULLUP);
  pinMode(openSwitch, INPUT_PULLUP);
  pinMode(closeSwitch, INPUT_PULLUP);
  pinMode(closingLED, OUTPUT);
  pinMode(openingLED, OUTPUT);
  pinMode(garageLight, OUTPUT);

  digitalWrite(closingLED, LOW);
  digitalWrite(openingLED, LOW);
  digitalWrite(garageLight, LOW);

}

/**
 * Return true if the door open/close button is pressed
 *
 * Note: this is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if buttons is currently pressed, false otherwise
 */
boolean isButtonPressed() {
  return digitalRead(doorController) == 0;
}

/**
 * Return true if the door is fully closed
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely closed, false otherwise
 */
boolean isDoorFullyClosed() {
  return digitalRead(closeSwitch) == 0;
}

/**
 * Return true if the door has experienced a fault
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is has experienced a fault
 */
boolean isFaultActive() {
  return digitalRead(faultSignal) == 0;
}

/**
 * Return true if the door is fully open
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely open, false otherwise
 */
boolean isDoorFullyOpen() {
  return digitalRead(openSwitch) == 0;
}

/**
 * This function will start the motor moving in a direction that opens the door.
 *
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 *
 * return void
 */
void startMotorOpening() {
  digitalWrite(openingLED, HIGH);
}

/**
 * This function will start the motor moving in a direction closes the door.
 *
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 *
 * return void
 */
void startMotorClosing() {
  digitalWrite(closingLED, HIGH);
}

/**
 * This function will stop all motor movement.
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void stopMotor() {
  digitalWrite(openingLED, LOW);
  digitalWrite(closingLED, LOW);

}

/**
 * This function will control the state of the light on the opener.
 *
 * Parameter: on: true indicates the light should enter the "on" state;
 *                false indicates the light should enter the "off" state
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void setLight(boolean on) {
  digitalWrite(garageLight, on);
}

/**
 * This function will control the state of the light on the opener.
 *
 * Parameter: cycle (0-100).  0 indicates completely Off, 100 indicates completely on.
 *            intermediate values are the duty cycle (as a percent)
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void setLightPWM(int cyclePct) {
    analogWrite(garageLight, cyclePct * 255 / 100);
}

/**
 * This function will send a debugging message.
 *
 * Parameter: message. The message (no more tha 200 bytes)
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void sendDebug(String message) {
  Serial.println(message);
}

#endif
