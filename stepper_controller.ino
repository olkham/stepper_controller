#include <AccelStepper.h>
#include <MultiStepper.h>


//TODO update pin convention to support ESP8266
//Currently using Arduino pin numbers
#define Y_STEP_PIN 2
#define Y_DIR_PIN 5

#define X_STEP_PIN 3
#define X_DIR_PIN 6


const byte numChars = 16;  //-1000,-1000\n
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
bool first_run = true;
int steps = 0;
float max_speed = 10000;
//TODO - when running two motors this speed is effectvly halved...

int x_steps, y_steps;

// Define the stepper motor and the pins that is connected to
AccelStepper x_Stepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN); // (Type of driver: with 2 pins, STEP, DIR)
AccelStepper y_Stepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN); // (Type of driver: with 2 pins, STEP, DIR)

MultiStepper Steppers;

int x_position = 0;
int y_position = 0;

long positions[2];

void setup()
{
  x_Stepper.setMaxSpeed(max_speed);
  y_Stepper.setMaxSpeed(max_speed);

  Steppers.addStepper(x_Stepper);
  Steppers.addStepper(y_Stepper);

  Serial.begin(115200);
  Serial.println("Started...");
}

void loop()
{

  if (newData) {
    newData = false;
    
    positions[0] += (long)x_steps;
    positions[1] += (long)y_steps;
    Steppers.moveTo(positions);
    Steppers.runSpeedToPosition();

    // x_position += x_steps;
    // x_Stepper.moveTo((long)x_position);
    // x_Stepper.setSpeed(max_speed);    
    // y_position += y_steps;
    // y_Stepper.moveTo((long)y_position);
    // y_Stepper.setSpeed(max_speed);    

    x_steps = 0;
    y_steps = 0;
  }

  // if (x_Stepper.distanceToGo() != 0) {
  //   x_Stepper.runSpeedToPosition();
  // }

  // if (y_Stepper.distanceToGo() != 0) {
  //   y_Stepper.runSpeedToPosition();
  // }

  //recvWithEndMarker();
  read2();
}


// void recvWithEndMarker() {
//     static byte ndx = 0;
//     char endMarker = '\n';
//     char rc;
    
//     if (Serial.available() > 0) {
//         rc = Serial.read();

//         if (rc != endMarker) {
//             receivedChars[ndx] = rc;
//             ndx++;
//             if (ndx >= numChars) {
//                 ndx = numChars - 1;
//             }
//         }
//         else {
//             receivedChars[ndx] = '\0'; // terminate the string
//             ndx = 0;
//             newData = true;
//         }
//     }
// }

void read2(){
  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', receivedChars, numChars);
    newData = true;
    receivedChars[bytesRead] = '\0';
    int result = sscanf(receivedChars, "%d,%d", &x_steps, &y_steps);
  }
}
