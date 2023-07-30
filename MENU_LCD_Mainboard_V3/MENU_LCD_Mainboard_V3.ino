//------- DECLARING DISPLAY -------\\

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//------- DECLARING RADIO -------\\

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int CE_pin = 7;
const int CSN_pin = 8;

RF24 radio(CE_pin, CSN_pin);

// ADRESS
const byte address[6] = "00001";

//------- DECLARING ROTARY ENCODER -------\\

const int CLK = 10;
const int DT = 9;
const int SW = 3;

//------- ROTARY ENCODER STATES -------\\

int lastCLKState;
int currentCLKState;
int DTState;
int SWState;

//------- ENCODER BUTTON STATES -------\\

const int SHORT_PRESS_TIME = 500; // 500 milliseconds

int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

//------- DECLARING SCREEN VARIABLES -------\\

int screen = 1;
const int max_screen = 4;

//------- DECLARING SERVO VARIABLES -------\\

int servo1_value;
int servo2_value;

String current_command;
String last_command;

char cmd[12];

// CUSTOM ARROW CHARACTER
byte arrowRight[8] = {
  0b01000,
  0b01100,
  0b01110,
  0b01111,
  0b01110,
  0b01100,
  0b01000,
  0b00000
};

byte arrowLeft[] = {
  B00010,
  B00110,
  B01110,
  B11110,
  B01110,
  B00110,
  B00010,
  B00000
};

//------- DECLARING BUTTONS FIRE/RAPID -------\\

const int fire_button = 6;
int fire_state;

bool fire = false;

//------- DECLARING SCREEN STATES -------\\

bool home_screen = true;
bool channel_screen = false;
bool servo_screen = false;
bool servo_angle_screen = false;

void channelScreen(int sc) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Channel:");

  lcd.setCursor(0, 1);
  lcd.print("CH1");
  lcd.setCursor(4, 1);
  lcd.print("CH2");
  lcd.setCursor(8, 1);
  lcd.print("CH3");
  lcd.setCursor(12, 1);
  lcd.print("CH4");

  switch (sc) {
    case 1:
      lcd.setCursor(3, 1);
      lcd.write(byte(0));
      break;
    case 2:
      lcd.setCursor(7, 1);
      lcd.write(byte(0));
      break;
    case 3:
      lcd.setCursor(11, 1);
      lcd.write(byte(0));
      break;
    case 4:
      lcd.setCursor(15, 1);
      lcd.write(byte(0));
      break;
  }

  lcd.setCursor(15, 0);
  lcd.print(sc);

}

void servoScreen(int sc) {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Servo 1");
  lcd.setCursor(2, 1);
  lcd.print("Servo 2");

  switch (sc) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.write(byte(1));
      break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.write(byte(1));
      break;
    default:
      sc = 2;
      break;
  }

  lcd.setCursor(15, 0);
  lcd.print(sc);
}

void setServoValue(int val, int sc) {
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (sc) {
    case 1:
      lcd.print("Servo 1 angle:");
      break;
    case 2:
      lcd.print("Servo 2 angle:");
      break;
  }
  lcd.setCursor(0, 1);
  lcd.print(val);
}

void homeScreen(int sc) {
  switch (sc) {
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 1");
      lcd.setCursor(0, 0);
      lcd.write(byte(1));
      lcd.setCursor(2, 1);
      lcd.print("Node 2");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 1");
      lcd.setCursor(0, 1);
      lcd.write(byte(1));
      lcd.setCursor(2, 1);
      lcd.print("Node 2");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 3");
      lcd.setCursor(0, 0);
      lcd.write(byte(1));
      lcd.setCursor(2, 1);
      lcd.print("Node 4");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 3");
      lcd.setCursor(0, 1);
      lcd.write(byte(1));
      lcd.setCursor(2, 1);
      lcd.print("Node 4");
      break;
    default: screen = 4;
  }
}

void fireScreen(int sc) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Ready to fire?");

  lcd.setCursor(3, 1);
  lcd.print("Yes");
  lcd.setCursor(9, 1);
  lcd.print("No");
  
  switch (sc) {
    case 1:
      lcd.setCursor(2, 1);
      lcd.write(byte(1));
      lcd.setCursor(6, 1);
      lcd.write(byte(0));
      break;
    case 2:
      lcd.setCursor(8, 1);
      lcd.write(byte(1));
      lcd.setCursor(11, 1);
      lcd.write(byte(0));
      break;
    default:
      sc = 2;
  }
}

void fireingScreen() {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Firing!!!");
  for (int i = 0; i <= 15; i++) {
    lcd.setCursor(i, 1);
    lcd.print(".");
    delay(150);
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Fired");
  lcd.setCursor(2,1);
  lcd.print("Successfully");
  delay(2000);
  homeScreen(screen);
}

void setup() {
  Serial.begin(9600);

  pinMode(fire_button, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, arrowLeft);
  lcd.createChar(1, arrowRight);

  lcd.setCursor(3, 0);
  lcd.print("Fireworks");
  lcd.setCursor(4, 1);
  lcd.print("Igniter");
  delay(2000);
  lcd.clear();
  homeScreen(screen);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  lastCLKState = digitalRead(CLK);
}

void loop() {
  currentCLKState = digitalRead(CLK);

  fire_state = digitalRead(fire_button);

  if (fire_state == LOW && !fire && home_screen) {
    fire = true;
    home_screen = false;
    fireScreen(screen);
    delay(250);
  }

  if (currentCLKState != lastCLKState && currentCLKState == 1) {
    DTState = digitalRead(DT);
    if (DTState != currentCLKState) { // CW
      if (screen < max_screen && !servo_screen && !servo_angle_screen && !fire) screen++;
      if (home_screen) homeScreen(screen);
      else if (channel_screen) channelScreen(screen);
      else if (servo_screen) {
        if (screen < 2 && !servo_angle_screen) screen++;
        servoScreen(screen);
      }
      else if (servo_angle_screen) {
        if (screen == 1) {
          if (servo1_value < 180) servo1_value += 5;
          setServoValue(servo1_value, screen);
        } else if (screen == 2) {
          if (servo2_value < 180) servo2_value += 5;
          setServoValue(servo2_value, screen);
        }
      }
      else if (fire) {
        if (screen < 2) screen++;
        fireScreen(screen);
        Serial.println(screen);
      }
    } else {                          // CCW
      if (screen > 1 && !servo_angle_screen) screen--;
      if (home_screen)  homeScreen(screen);
      else if (channel_screen) channelScreen(screen);
      else if (servo_screen) servoScreen(screen);
      else if (servo_angle_screen) {
        if (screen == 1) {
          if (servo1_value > 0) servo1_value -= 5;
          setServoValue(servo1_value, screen);
        } else if (screen == 2) {
          if (servo2_value > 0) servo2_value -= 5;
          setServoValue(servo2_value, screen);
        }
      }
      else if (fire) {
        fireScreen(screen);
        Serial.println(screen);
      }
    }
  }

  SWState = digitalRead(SW);

  if (lastState == HIGH && SWState == LOW) pressedTime = millis();  // button is pressed
  else if (lastState == LOW && SWState == HIGH) {                   // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if ( pressDuration < SHORT_PRESS_TIME ) {
      if (home_screen) {
        current_command += "N" + String(screen);
        home_screen = false;
        channel_screen = true;
        screen = 1;
        channelScreen(screen);
      } else if (!home_screen && channel_screen) {
        channel_screen = false;
        current_command += "CH" + String(screen);
        servo_screen = true;
        screen = 1;
        servoScreen(screen);
      } else if (!channel_screen && servo_screen) {
        servo_screen = false;
        current_command += "S" + String(screen);
        servo_angle_screen = true;
        if (screen == 1) setServoValue(servo1_value, screen);
        else if (screen == 2) setServoValue(servo2_value, screen);
      } else if (fire && !home_screen) {
        if (screen == 1) {
          Serial.println("Yes");
          fireingScreen();
          fire = false;
          home_screen = true;
        } else if (screen == 2) {
          Serial.println("No");
          fire = false;
          home_screen = true;
          homeScreen(screen);
        }
      }
    } else {
      if (current_command.length() == 7 && screen == 1) {
        current_command += "A" + String(servo1_value);
        last_command = current_command;
        current_command = "";
      }
      else if (current_command.length() == 7 && screen == 2) {
        current_command += "A" + String(servo2_value);
        last_command = current_command;
        current_command = "";
      }
      else current_command = "";
      //Serial.println(last_command);
      
      /*
      for (int i = 0; i < last_command.length(); i++) {
        cmd[0] = last_command.charAt(i);
        radio.write(cmd, sizeof(cmd));
      }*/
      /*
      for (int i = 0; i < last_command.length(); i++) {
        if (i < 8) cmd[i] = last_command[i];
        else radio_angle = last_command[i];
      }*/

      last_command.toCharArray(cmd, 12);

      radio.write(&cmd, sizeof(cmd));
      
      home_screen = true;
      channel_screen = false;
      servo_screen = false;
      servo_angle_screen = false;
      homeScreen(screen);
    }
  }

  lastState = SWState;
  lastCLKState = currentCLKState;
}
