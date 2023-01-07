#include <AccelStepper.h>
#include <MultiStepper.h>

//TODO update pin convention to support ESP8266
//Currently using Arduino pin numbers
// #define Y_STEP_PIN 4
// #define Y_DIR_PIN 0

//node-mcu
// #define STEP_PIN 16
// #define DIR_PIN 5
// #define TRIGGER_PIN1 14  //todo - check pins for D1
// #define TRIGGER_PIN2 12  //todo - check pins for D1

//WEMOS D1 Mini
#define STEP_PIN D0
#define DIR_PIN D1
// #define TRIGGER_PIN1 D2
// #define TRIGGER_PIN2 D3


const int LED = D4;         // LED pin
const int FOCUS_PIN = D2;
const int SHUTTER_PIN = D3;



const byte numChars = 16;  //-1000,-1000\n
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
float max_speed = 50000;
int steps = 0;
int speed = 0;
int trigger = 0;

// Define the stepper motor and the pins that is connected to
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN); // (Type of driver: with 2 pins, STEP, DIR)

int position = 0;

bool camera_actioned = false;
unsigned long camera_trigger_time = 0;
unsigned long timeout = 100; //ms


void configure_pins(){
  pinMode(LED, OUTPUT);
  pinMode(FOCUS_PIN, OUTPUT);
  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(LED, HIGH);
  // pinMode(TRIGGER_PIN1, OUTPUT);
  // digitalWrite(TRIGGER_PIN1, LOW);
}

void setup()
{
  configure_pins();
  stepper.setMaxSpeed(max_speed);
  Serial.begin(115200);
  Serial.println("Started...");
}

void check_trigger()
{
  if (camera_actioned){
    if (millis() - camera_trigger_time > timeout){
      camera_actioned = false;

      digitalWrite(FOCUS_PIN, LOW);
      digitalWrite(SHUTTER_PIN, LOW);
      digitalWrite(LED, HIGH);
    }
  }

  // digitalWrite(TRIGGER_PIN1, LOW);    //make sure it's low first
  // digitalWrite(TRIGGER_PIN1, HIGH);   //v.quick pulse
  // digitalWrite(TRIGGER_PIN1, LOW);    //return to low
  // trigger = 0;
}


void trigger_all(){
  digitalWrite(FOCUS_PIN, HIGH);
  digitalWrite(SHUTTER_PIN, HIGH);
  digitalWrite(LED, LOW);
  camera_actioned = true;
  camera_trigger_time = millis();
}


void update()
{
  if (newData) {
    newData = false;
    position += steps;
    stepper.moveTo((long)position);
    stepper.setSpeed(max_speed);
  }

  if (stepper.distanceToGo() != 0) {
    stepper.runSpeedToPosition();
  }
}

void loop()
{
  update();
  read();
  check_trigger();
}


void read()
{
  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', receivedChars, numChars);
    newData = true;
    receivedChars[bytesRead] = '\0';
    int result = sscanf(receivedChars, "%d,%d,%d", &steps, &speed, &trigger);

    Serial.println(steps);

    if (speed > 0) {
      max_speed = (float)speed;
      Serial.println(speed);
    }

    if (trigger > 0) {
      Serial.println(trigger);
      trigger_all();
    }
  }
}
