/*
 * Use Arduino Nano to control DRV8825 or A4988 stepper motor driver boards
 * DRV8825 - https://www.pololu.com/product/2133
 * A4988 - Big Easy Driver from Sparkfun -  https://www.sparkfun.com/products/12859
 * Serial input main ref: Serial Input Basics by Robin2 in Arduino Forum 
*/

/* serial command interface documentation
Command format: [text,value]
Commands start with [, and end with ]
A comma , separates text field and value field

Implemented commands
[hello]
[MV,n]
[BLINK,n]

TODO:
query commands

end of command interface documentation */

// global variables for stepper motor driver board
int M0_PIN=11; // Pin D8 is addressed at 8
int M1_PIN=10; //
int M2_PIN=9; //
int STEP_PIN=8; //
int DIR_PIN=7; //

int CW=HIGH;
int CCW=LOW;

int BLINK_PIN=13;

// global variables for serial comm
const byte N_Chars = 16;
char str_comm[N_Chars];
char txt_comm[N_Chars];
char start_marker='[';
char stop_marker=']';
char sep_marker=',';// not in use now
int val_integer=0;
//float val_float=0.0;
boolean flag_comm=false; // true when one complete command is received

void setup() { // put your setup code here, to run once:
  // initialize serial communication
  // Opening serial monitor is effectively pressing reset button?
  Serial.begin(115200);
  Serial.println("Serial Comm w/ Arduino Initialized.");
  
  pinMode(DIR_PIN,OUTPUT);
  pinMode(STEP_PIN,OUTPUT);
  pinMode(M0_PIN,OUTPUT);
  pinMode(M1_PIN,OUTPUT);
  pinMode(M2_PIN,OUTPUT);
  pinMode(BLINK_PIN,OUTPUT);
}

void loop() { // put your main code here, to run repeatedly:
  //Serial.println(millis());
  //msg = String("Loop Count: "+String(loop_count));
  //Serial.println(msg);
  //loop_count=loop_count+1;
  
  Recv_Comm();
  if (flag_comm==true) {
    //Serial.println(str_comm);
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
  val_integer=atoi(strtok_idx);
  
  Serial.println(txt_comm);
  Serial.println(val_integer);
}

void Exec_Comm() {
  if (strcmp(txt_comm,"MV")==0) {
    MOVE(STEP_PIN,val_integer);
    Serial.println("MV command executed.");
  }
  else if (strcmp(txt_comm,"hello")==0) { // txt_comm=="yellow" does not work
    Serial.println("hello there.");
  }
  else if (strcmp(txt_comm,"OFF")==0) {
    Serial.println("OFF received.");
  }
  else if (strcmp(txt_comm,"DIR")==0) {
    DIR(DIR_PIN,val_integer);
    Serial.println("Called DIR() to set direction.");
  }
  else if (strcmp(txt_comm,"STEP")==0) {
    STEP(M0_PIN,M1_PIN,M2_PIN,val_integer);
    Serial.println("Called STEP() to set step size.");
  }
  else if (strcmp(txt_comm,"BLINK")==0) {
    Serial.println("blinking.");
    BLINK(BLINK_PIN,val_integer);
  }
  else { 
    Serial.println("Exec_Comm(): Command not recognized.");
  }
}

// Implement commands below

void DIR(int DIR_PIN, int n) {
  if (n==0) {
    digitalWrite(DIR_PIN,CCW);
  }
  else if (n>0) {
    digitalWrite(DIR_PIN,CW);
  }
}

void STEP(int M0_PIN,int M1_PIN,int M2_PIN,int n) {
  /*
  Microstepping using DRV8825
  Step size: M0,M1,M2 Pin values
  Full: LOW,LOW,LOW
  Half: HIGH,LOW,LOW
  1/4: LOW,HIGH,LOW
  1/8: HIGH,HIGH,LOW
  1/16: LOW,LOW,HIGH
  1/32: HIGH,LOW,HIGH
  */
  if (n==1) {
    digitalWrite(M0_PIN,LOW);
    digitalWrite(M1_PIN,LOW);
    digitalWrite(M2_PIN,LOW);
  }
  else if (n==2) {
    digitalWrite(M0_PIN,HIGH);
    digitalWrite(M1_PIN,LOW);
    digitalWrite(M2_PIN,LOW);
  }
  else if (n==4) {
    digitalWrite(M0_PIN,LOW);
    digitalWrite(M1_PIN,HIGH);
    digitalWrite(M2_PIN,LOW);
  }
  else if (n==8) {
    digitalWrite(M0_PIN,HIGH);
    digitalWrite(M1_PIN,HIGH);
    digitalWrite(M2_PIN,LOW);
  }
  else if (n==16) {
    digitalWrite(M0_PIN,LOW);
    digitalWrite(M1_PIN,LOW);
    digitalWrite(M2_PIN,HIGH);
  }
  else if (n==32) {
    digitalWrite(M0_PIN,HIGH);
    digitalWrite(M1_PIN,LOW);
    digitalWrite(M2_PIN,HIGH);
  }
  else {
    Serial.println("STEP(): Step size not recognized.");
  }
}

void MOVE(int STEP_PIN, int n) {  
  int wait_time=1;
  for (int i=0;i<n;i++) {
    digitalWrite(STEP_PIN,HIGH);
    delay(wait_time);
    digitalWrite(STEP_PIN,LOW);
    delay(wait_time);
  }
}

void BLINK(int BLINK_PIN, int n) {
  int wait_time=1000;
  for (int i=0;i<n;i++) {
    digitalWrite(BLINK_PIN,HIGH);
    delay(wait_time);
    digitalWrite(BLINK_PIN,LOW);
    delay(wait_time);
  }
}
