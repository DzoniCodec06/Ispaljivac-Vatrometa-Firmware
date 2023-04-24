#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>
Servo servo1;

const int CLK = 2;
const int DT = 3;
const int SW = 4;
const int RESSET_BTN = 5;
const int LED_PIN = 6;
const int POT_PIN = 8;
int resset;

int lastCLKState;
int currentCLKState;
int DTState;
int SWState;
bool btnClicked;

int screen = 1;
const int max_screens = 3;

int angle = 0;
int pot_read = 0;

bool selectedScreen = false;
bool led_con = false;
bool srv_con = false;

int counter = 0;

String led_status[2] = {"ON", "OFF"}; 

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(RESSET_BTN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  Serial.begin(9600);

  servo1.attach(7);
  
  lastCLKState = digitalRead(CLK);
}

void loop() {
  currentCLKState = digitalRead(CLK);
  SWState = digitalRead(SW);
  resset = digitalRead(RESSET_BTN);
  if (currentCLKState != lastCLKState && currentCLKState == 1) {
        
        DTState = digitalRead(DT);
      
        if (DTState != currentCLKState) {
          if (screen < max_screens) {
            screen ++;
            counter = 1;
          }
        } else {
          if (screen > 1) {
            screen --;
            counter = 0;
          }
        }
        
        if (selectedScreen == false) {
            checkScreens(screen);
        }
        if (led_con == true) {
            LEDControl();
        } else if (srv_con == true) {
          servoControl();
        }
  }
   
  if (SWState == LOW && selectedScreen == false) {
    selectAction(screen); 
  } else if (resset == LOW && selectedScreen == true) {
    led_con = false;
    ressetDisplay();
  }
  
  lastCLKState = currentCLKState;
}

void servoControl() {
  pot_read = analogRead(POT_PIN);
  angle = map(pot_read, 0, 1023, 0, 180);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Control Servo");
  lcd.setCursor(2, 1);
  lcd.print("Angle: ");
  lcd.setCursor(10, 1);
  lcd.print(angle);
  servo1.write(angle);
}

void LEDControl() { 
    lcd.clear(); 
    lcd.setCursor(2, 0);
    lcd.print("Control LED");
    lcd.setCursor(2, 1);
    lcd.print("Status: ");
    lcd.setCursor(10, 1);
    lcd.print(led_status[counter]);
    if(counter == 0) {
      digitalWrite(LED_PIN, HIGH);
    } else if (counter == 1) {
      digitalWrite(LED_PIN, LOW);
    }
}


void IRControl() {
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Control IR LED");
}

/*
void rotateEncoder() {
  currentCLKState = digitalRead(CLK);
  if (currentCLKState != lastCLKState && currentCLKState == 1) {
      DTState = digitalRead(DT);
      
      if (DTState != currentCLKState) {
          counter = 1;
      } else {
          counter = 0;
      }
  }
  
  lastCLKState = currentCLKState;
}
*/

void checkScreens(int screens) {
  switch(screens) {
    case 0:
        screens = 1;
    case 1: 
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Select:");
        lcd.setCursor(10, 0);
        lcd.print("Servo");
        break;
    case 2: 
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Select:");
        lcd.setCursor(10, 0);
        lcd.print("IR");
        break;
    case 3:
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Select:");
        lcd.setCursor(10, 0);
        lcd.print("LED");
        break;
    default: 
        screens = 3;
  }
}


void selectAction(int screens) {
  switch(screens) {
    case 1:
        selectedScreen = true;
        servoControl();
        srv_con = true;
        break;
    case 2:
        selectedScreen = true;
        IRControl();
        break;
    case 3: 
        selectedScreen = true;
        LEDControl();
        led_con = true;
        break;
  }
}

void ressetDisplay() {
    selectedScreen = false;
    lcd.clear();
    screen = 1;
    lcd.setCursor(2,0);
    lcd.print("Select again");
}
