/*
 * Use Arduino to control unipolar stepper motor
 * Arduino Nano - ULN2803 - Unipolar Stepper Motor 
 * Reference:
 * https://www.arduino.cc/en/Reference/Stepper
 * https://www.arduino.cc/en/Reference/StepperUnipolarCircuit
 * https://www.arduino.cc/en/Reference/StepperExample
 */

#include <Stepper.h>
// check this value for the motor in use
#define stepsPerRev 400
// initialize the stepper library on pins 2 through 5
Stepper myMotor(stepsPerRev,2,3,4,5);

void setup() {
  // put your setup code here, to run once:
  // set the speed at 30 rpm
  myMotor.setSpeed(30);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("clockwise");
  myMotor.step(stepsPerRev);
  delay(500);
  
  Serial.println("counter-clockwise");
  myMotor.step(-stepsPerRev);
  delay(500);
}
