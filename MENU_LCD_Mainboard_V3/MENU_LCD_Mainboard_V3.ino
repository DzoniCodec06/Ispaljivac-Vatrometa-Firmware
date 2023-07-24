//------- DECLARING DISPLAY -------\\

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

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

  lcd.setCursor(15,0);
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

  lcd.setCursor(15,0);
  lcd.print(sc);
}

void setServoValue(int val, int sc) {
  lcd.clear();
  lcd.setCursor(0,0);
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

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, arrowLeft);
  lcd.createChar(1, arrowRight);

  lcd.setCursor(0, 0);
  lcd.print("Firework");
  delay(2000);
  lcd.clear();
  homeScreen(screen);

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  lastCLKState = digitalRead(CLK);
}

void loop() {
  currentCLKState = digitalRead(CLK);

  if (currentCLKState != lastCLKState && currentCLKState == 1) {
    DTState = digitalRead(DT);
    if (DTState != currentCLKState) { // CW
      if (screen < max_screen && !servo_screen && !servo_angle_screen) screen++;
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
    }
  }

  SWState = digitalRead(SW);

  if (lastState == HIGH && SWState == LOW) pressedTime = millis();  // button is pressed
  else if (lastState == LOW && SWState == HIGH) {                   // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if ( pressDuration < SHORT_PRESS_TIME ) {
      if (home_screen) {
        home_screen = false;
        channel_screen = true;
        screen = 1;
        channelScreen(screen);
      } else if (!home_screen && channel_screen) {
        channel_screen = false;
        servo_screen = true;
        screen = 1;
        servoScreen(screen);
      } else if (!channel_screen && servo_screen) {
        servo_screen = false;
        servo_angle_screen = true;
        if (screen == 1) setServoValue(servo1_value, screen);
        else if (screen == 2) setServoValue(servo2_value, screen);
      }
    } else {
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
