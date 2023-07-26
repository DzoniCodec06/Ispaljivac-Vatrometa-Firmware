//------- DECLARING RADIO MODULE -------\\

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//------- DECLARING SERVO MODULE -------\\

#include <Servo.h>

//------- DECLARING SERVO AND VALUES -------\\

Servo servo1;
Servo servo2;

const int SERVO_1_PIN = 6; 
const int SERVO_2_PIN = 10;

int set_servo; 
int servo_value; 

int servo_delay = 10; 

//------- DECLARING COMMANDS -------\\

char cmd[12];
String command;

//------- DECLARING LEDS -------\\

int led1_pin = 2;
int led2_pin = 3;
int led3_pin = 4;
int led4_pin = 5;

int led;

int led_list[7] = {0, led1_pin, led2_pin, led3_pin, led4_pin};

//------- SETTING UP NRF RADIO MODULE -------\\

const int CE_pin = 7;
const int CSN_pin = 8;

RF24 radio(CE_pin, CSN_pin); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  
  servo1.write(0);
  delay(10);
  servo2.write(0);
  delay(10);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);
  pinMode(led4_pin, OUTPUT);
  
  digitalWrite(led1_pin, LOW);
  digitalWrite(led2_pin, LOW);
  digitalWrite(led3_pin, LOW);
  digitalWrite(led4_pin, LOW);

  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    
    command = ""; // Reseting String command
    for (int i = 1; i <= 4; i++) digitalWrite(led_list[i], LOW); // Turning all LED's OFF
    
    radio.read(&cmd, sizeof(cmd)); // Read radio received command and storing to char array
    for (int i = 0; i < 11; i++) command += cmd[i]; // Storing charr array into String object
    
    led = cmd[4] - '0'; // Covert char to int
    digitalWrite(led_list[led], HIGH);
    
    set_servo = command.substring(6,7).toInt(); // Servo selection
    servo_value = command.substring(8).toInt(); // Servo angle
    
    if (set_servo == 1) {
      servo1.write(servo_value);
      delay(servo_delay);
    } else if (set_servo == 2) {
      servo2.write(servo_value);
      delay(servo_delay);
    }
  }
}
