/*
 * Use Arduino to control unipolar stepper motor
 * Arduino Nano - ULN2803 - Unipolar Stepper Motor 
 * 
 * Yinsheng Guo, 20181130
 * 
 * Reference:
 * https://www.arduino.cc/en/Reference/Stepper
 * https://www.arduino.cc/en/Reference/StepperUnipolarCircuit
 * https://www.arduino.cc/en/Reference/StepperExample
 * 
 * Notes:  
 * This code uses the Stepper.h library. 
 * Most useful info are in the Stepper.h and Stepper.cpp source code.
 * Source code location - C:\Program Files (x86)\Arduino\libraries\Stepper\src
 * 
 * Stepper.h does not have microstepping capability.
 * 
 * Stepper.h works with both Darlington array (ULN2803) and H-bridge (L293),
 * and with both unipolar and bipolar motors.
 * 
 * The Stepper class has only three pubic methods (beside the constructor):
 *   void setSpeed(long whatSpeed)
 *   void step(int number_of_steps)
 *   int version(void)
 *   
 * Questions:
 * Stepper motor coil wiring and charging cycles 
 * connection/sequence/oriention of the coils - wiring order does not matter?
 * voltage states for generating holding torque 
 */


/* Serial Command Interface Documentation
Main ref: Serial Input Basics by Robin2 in Arduino Forum
 
Command format: [text,value]
Commands start with [, and end with ]
A comma , separates text field and value field

Implemented commands
[hello]
[MV,n]
[BLINK,n]

TODO:
query commands
step counter
add command to turn off holding torque
*/

#include <Stepper.h>

// Serial Comm: set up global variables
const byte N_Chars = 16;
char str_comm[N_Chars];
char txt_comm[N_Chars];
char start_marker='[';
char stop_marker=']';
char sep_marker=',';// not in use now
int val_integer=0;
//float val_float=0.0;
boolean flag_comm=false; // true when one complete command is received

// Stepper: set up steps per revolution and Arduino control pins
#define stepsPerRev 400 // check this value for the motor in use
// initialize the stepper library on pins 2 through 5
#define M0 12
#define M1 11
#define M2 10
#define M3 9
#define BLINK_PIN 13
Stepper myMotor(stepsPerRev,M0,M1,M2,M3);

void setup() { // put your setup code here, to run once:
  myMotor.setSpeed(30); // set the speed at 30 rpm
  pinMode(BLINK_PIN,OUTPUT);
  Serial.begin(115200);
  Serial.println("Serial Comm w/ Arduino Initialized.");
}

void loop() { // put your main code here, to run repeatedly:
  Recv_Comm();
  if (flag_comm==true) {
    Parse_Comm();
    Exec_Comm();
    flag_comm=false;
  }
  delay(100);
}

void Recv_Comm() {
  char rc;
  static byte idx=0;//index of the str_comm array
  static boolean flag_reading=false;//true if received start_marker, reset to false after received end_marker
  
  //using if here: reading one char every loop() iteration
  //using while here: reading every available char in one loop() iteration
  while (Serial.available() > 0 && flag_comm==false) {
    rc=Serial.read();
    if (flag_reading==true) {
      if (rc!=stop_marker) {
        str_comm[idx]=rc;
        idx++;
        if (idx>=N_Chars) {idx=N_Chars-1;} //prevent overflow
        //Serial.print(idx);
        //Serial.print(String(str_comm)+" ");
      }
      else { // executed when rc==stop_marker
        str_comm[idx]='\0';// terminate the string with Null
        idx=0;
        flag_reading=false;// reset the reading flag to false
        flag_comm=true;
      }
    }
    else if (rc==start_marker) {flag_reading=true;}
  }
}

void Parse_Comm() {
  char copy_comm[N_Chars];
  char * strtok_idx;
  
  strcpy(copy_comm,str_comm);
  strtok_idx=strtok(copy_comm,",");//","
  strcpy(txt_comm,strtok_idx);
  
  strtok_idx=strtok(NULL,","); //","
  val_integer=atoi(strtok_idx); // negative integer works too
  
  Serial.println(txt_comm);
  Serial.println(val_integer);
}

void Exec_Comm() {
  if (strcmp(txt_comm,"MV")==0) {
    myMotor.step(val_integer);
    Serial.println("MV command executed.");
  }
  else if (strcmp(txt_comm,"hello")==0) { // txt_comm=="yellow" does not work
    Serial.println("hello there.");
  }
  else if (strcmp(txt_comm,"REST")==0) {
    REST();
    Serial.println("OFF received.");
  }
  else if (strcmp(txt_comm,"BLINK")==0) {
    Serial.println("blinking.");
    BLINK(val_integer);
  }
  else { 
    Serial.println("Exec_Comm(): Command not recognized.");
  }
}

void REST(){
  // turn off holding torque
  digitalWrite(M0,LOW);
  digitalWrite(M1,LOW);
  digitalWrite(M2,LOW);
  digitalWrite(M3,LOW);
}

void BLINK(int n) {
  int wait_time=1000;
  for (int i=0;i<n;i++) {
    digitalWrite(BLINK_PIN,HIGH);
    delay(wait_time);
    digitalWrite(BLINK_PIN,LOW);
    delay(wait_time);
  }
}
