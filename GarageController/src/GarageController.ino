#include "GarageHardware.h"

const String topic = "cse222garageDoor";
bool shouldPublish = false;           //whether the program should call publishState
enum doorStatus {
  OPEN,
  CLOSED,
  OPENING,
  CLOSING,
  SHOULD_BE_OPENING,
  SHOULD_BE_CLOSING,
};
doorStatus doorState;
bool isAutoCloseEnabled = false;
unsigned int autoCloseTime = 30000;
bool isUIClicked = false;             //whether the user click the door button in the UI
bool hasStartTimer = false;           //whether the autoCloseTimer has started
bool haveFault = false;               //whether there is a fault
int lightBrightness = 100;
bool lightOn = false;
unsigned int lightAutoOffTime = 30000;
bool hasStartLightTimer = false;      //whether the light timer has started

//timer to fade the light in 5 seconds
Timer lightOut(50, fadeLight);
int cycle;
void fadeLight() {
  if (cycle <= 0) {
    lightOut.stop();
    setLight(false);
    lightOn = false;
    shouldPublish = true;
  }

  else {
    setLightPWM(cycle);
    cycle--;
  }
}

//function to be called after the auto-close timer has finished,
//switch the door's state to closing if it is open and the auto-close
//is still enabled
void performAutoClose() {
  if (doorState == OPEN && isAutoCloseEnabled) {
    doorState = CLOSING;
    lightOut.stop();
    setLightPWM(lightBrightness);
    lightOn = true;
    startMotorClosing();
    shouldPublish = true;
  }
}

Timer autoCloseTimer(autoCloseTime, performAutoClose, true);

//function to enable/disable auto-close
int setAutoCloseEnabled (String status) {
  if (status == "true") {
    isAutoCloseEnabled = true;
  }
  else isAutoCloseEnabled = false;
  shouldPublish = true;
  return 0;
}

//function called after the autoOffTimer is done
void performAutoOff() {
  if (lightOn) {
    cycle = lightBrightness - 1;
    lightOut.changePeriod(5000 / lightBrightness);
  }
}
Timer autoOffTimer(lightAutoOffTime, performAutoOff, true);

//function to change auto-close time
int setLightAutoOffTime (String time) {
  lightAutoOffTime = time.toInt();
  autoOffTimer.changePeriod(lightAutoOffTime);
  autoOffTimer.stop();
  hasStartLightTimer = false;
  shouldPublish = true;
  return 0;
}

//set UIClicked to true if the user click the door button in the UI
int doorButtonPress(String nothing) {
    isUIClicked = true;
    return 0;
}

//toggle light
int lightButtonPress(String nothing) {
  if (!lightOn) {
    lightOn = true;
    lightOut.stop();
    setLightPWM(lightBrightness);
    autoOffTimer.start();
  }
  else {
    lightOn = false;
    lightOut.stop();
    setLight(false);
  }
  shouldPublish = true;
  return 0;
}

//change light brightness
int lightBrightnessChange(String brightness) {
  lightBrightness = brightness.toInt();
  shouldPublish = true;
  if (lightOn)
    setLightPWM(lightBrightness);
  return 0;
}

int setAutoCloseTime(String time) {
  autoCloseTime = time.toInt();
  autoCloseTimer.changePeriod(autoCloseTime);
  autoCloseTimer.stop();
  hasStartTimer = false;
  shouldPublish = true;
  return 0;
}

//publish 7 information:
// +/ door's state
// +/ is auto-close activated
// +/ how much time before auto-close
// +/ has fault
// +/ light brightness
// +/ light auto-off
// +/ light on/off
int publishState(String nothing) {
  String data = "{\"doorState\":";
  if (doorState == OPENING){
      data += "\"Opening\"";
  }
  else if (doorState == OPEN) {
      data += "\"Open\"";
  }
  else if (doorState == CLOSING) {
      data += "\"Closing\"";
  }
  else if (doorState == CLOSED) {
      data += "\"Closed\"";
  }
  else if (doorState == SHOULD_BE_CLOSING) {
      data += "\"Stopped - Should Be Closing\"";
  }
  else if (doorState == SHOULD_BE_OPENING) {
      data += "\"Stopped - Should Be Opening\"";
  }
  data += ", \"isAutoCloseEnabled\":";
  if (isAutoCloseEnabled)
    data += "true";
  else data += "false";
  data += ", ";
  data += "\"autoCloseTime\":";
  data += autoCloseTime;
  data += ", ";
  data += "\"haveFault\":";
  if (haveFault)
    data += "true";
  else data += "false";

  data += ", ";
  data += "\"lightOn\":";
  if (lightOn)
    data += "true";
  else data += "false";

  data += ", ";
  data += "\"lightBrightness\":";
  data += lightBrightness;
  data += ", ";
  data += "\"lightAutoOffTime\":";
  data += lightAutoOffTime;
  data += "}";

  Serial.println("Publishing:");
  Serial.println(data);
  Particle.publish(topic, data, 60, PRIVATE);
  return 0;
}

void setup() {
  setupHardware();
  //get the initial state of the door
  if (isDoorFullyOpen())
    doorState = OPEN;
  else if (isDoorFullyClosed())
    doorState = CLOSED;
  else doorState = SHOULD_BE_CLOSING;

  Particle.function("publishState", publishState);
  Particle.function("doorButtonPress", doorButtonPress);
  Particle.function("setAutoCloseEnabled", setAutoCloseEnabled);
  Particle.function("setAutoCloseTime", setAutoCloseTime);
  Particle.function("lightButtonPress", lightButtonPress);
  Particle.function("lightBrightnessChange", lightBrightnessChange);
  Particle.function("setLightAutoOffTime", setLightAutoOffTime);
}

//variables for debounce
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 100;
bool lastState = false;
bool alreadyDone = false;

void loop() {
  unsigned long currentMillis = millis();
  bool isPressed = isButtonPressed();

  if (shouldPublish) {
    publishState("");
    shouldPublish = false;
  }

  switch (doorState) {
    case OPEN:
      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone) || isUIClicked) {
        doorState = CLOSING;
        lightOn = true;
        startMotorClosing();
        lightOut.stop();
        autoOffTimer.stop();
        setLightPWM(lightBrightness);
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (!hasStartTimer && isAutoCloseEnabled) {
        autoCloseTimer.start();
        hasStartTimer = true;
      }

      if (lightOn && !hasStartLightTimer) {
        hasStartLightTimer = true;
        autoOffTimer.start();
      }

      break;

    case CLOSED:
      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone) || isUIClicked) {
        doorState = OPENING;
        startMotorOpening();
        lightOut.stop();
        autoOffTimer.stop();
        setLightPWM(lightBrightness);
        lightOn = true;
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (lightOn && !hasStartLightTimer) {
        hasStartLightTimer = true;
        autoOffTimer.start();
      }

      break;

    case OPENING:
      autoOffTimer.stop();
      if (isDoorFullyOpen()) {
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        doorState = OPEN;
        shouldPublish = true;
        hasStartTimer = false;
      }

      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone) || isUIClicked) {
        doorState = SHOULD_BE_CLOSING;
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (isFaultActive()) {
        doorState = SHOULD_BE_OPENING;
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        sendDebug("fault");
        shouldPublish = true;
        hasStartTimer = false;
        haveFault = true;
      }
      break;

    case CLOSING:
      autoOffTimer.stop();
      if (isDoorFullyClosed()) {
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        doorState = CLOSED;
        shouldPublish = true;
        hasStartTimer = false;
      }

      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone) || isUIClicked) {
        doorState = SHOULD_BE_OPENING;
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (isFaultActive()) {
        doorState = SHOULD_BE_CLOSING;
        stopMotor();
        // cycle = lightBrightness - 1;
        // lightOut.changePeriod(5000 / lightBrightness);
        autoOffTimer.start();
        sendDebug("fault");
        shouldPublish = true;
        hasStartTimer = false;
        haveFault = true;
      }
      break;

    case SHOULD_BE_OPENING:
      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone)|| isUIClicked) {
        doorState = OPENING;
        startMotorOpening();
        lightOut.stop();
        setLightPWM(lightBrightness);
        lightOn = true;
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
        haveFault = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (lightOn && !hasStartLightTimer) {
        hasStartLightTimer = true;
        autoOffTimer.start();
      }

      break;



    case SHOULD_BE_CLOSING:
      if ((isPressed && lastPressTime + debounceDelay <= currentMillis && !alreadyDone)|| isUIClicked) {
        doorState = CLOSING;
        startMotorClosing();
        lightOut.stop();
        setLightPWM(lightBrightness);
        lightOn = true;
        alreadyDone = true;
        isUIClicked = false;
        shouldPublish = true;
        hasStartTimer = false;
        haveFault = false;
      }

      else if (!isPressed && lastState) {
        lastState = false;
        alreadyDone = false;
      }

      else if(isPressed && !lastState) {
        lastState = true;
        lastPressTime = currentMillis;
      }

      if (lightOn && !hasStartLightTimer) {
        hasStartLightTimer = true;
        autoOffTimer.start();
      }

      break;
  }

}
