#include "Adafruit_SSD1306/Adafruit_SSD1306.h"

#define OLED_DC     D3
#define OLED_CS     D4
#define OLED_RESET  D5
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
const int doorController = D1;
const int lightController = D0;

int x, minX;
String state;

//check state of door
void myHandler(const char *event, const char *data)
{
  Serial.println(data);
  String info = data;
  String doorState;
  String lightState;
  //get door's state from cloud
  if (info.indexOf("Should Be Closing") != -1) {
    doorState = "stopped (should be closing)";
  } else if (info.indexOf("Should Be Opening") != -1) {
    doorState = "stopped (should be opening)";
  } else if (info.indexOf("Opening") != -1) {
    doorState = "opening";
  } else if (info.indexOf("Closed") != -1) {
    doorState = "closed";
  } else if (info.indexOf("Closing") != -1) {
    doorState = "closing";
  } else if (info.indexOf("Open") != -1) {
    doorState = "open";
  }

  //get light state from cloud
  if (info.indexOf("\"lightOn\":true") != -1) {
    lightState = "on";
  } else lightState = "off";
  state = "Door is " + doorState + ", light is " + lightState;
}

void setup() {
  Particle.subscribe("cse222garageDoor", myHandler,  MY_DEVICES);
  pinMode(doorController, INPUT_PULLUP);
  pinMode(lightController, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(3);       // text size
  display.setTextColor(WHITE); // text color
  display.setTextWrap(false); // turn off text wrapping so we can do scrolling
  x = display.width(); // set scrolling frame to display width
  minX = -1600; // 630 = 6 pixels/character * text size 7 * 15 characters * 2x slower

}

//debounce variable for door button
unsigned long doorLastPressTime = 0;
const unsigned long debounceDelay = 100;
bool doorLastState = false;
bool doorAlreadyDone = false;

//debounce variable for light button
unsigned long lightLastPressTime = 0;
bool lightLastState = false;
bool lightAlreadyDone = false;

void loop() {
  display.clearDisplay();
  display.setCursor(x/2, 7);
  display.print(state);
  display.display();
  if(--x < minX) x = display.width()*2;

  unsigned long currentMillis = millis();
  bool isDoorPressed = (digitalRead(doorController) == 0);
  bool isLightPressed = (digitalRead(lightController) == 0);


  if (isDoorPressed && doorLastPressTime + debounceDelay <= currentMillis && !doorAlreadyDone) {
      Particle.publish("doorPress");            //trigger door webhook
      doorAlreadyDone = true;
  }
  else if (!isDoorPressed && doorLastState) {
    doorLastState = false;
    doorAlreadyDone = false;
  }

  else if (isDoorPressed && !doorLastState) {
    doorLastState = true;
    doorLastPressTime = currentMillis;
  }

  if (isLightPressed && lightLastPressTime + debounceDelay <= currentMillis && !lightAlreadyDone) {
      Particle.publish("lightPress");             //trigger light webhook
      lightAlreadyDone = true;
  }
  else if (!isLightPressed && lightLastState) {
    lightLastState = false;
    lightAlreadyDone = false;
  }

  else if (isLightPressed && !lightLastState) {
    lightLastState = true;
    lightLastPressTime = currentMillis;
  }

}
