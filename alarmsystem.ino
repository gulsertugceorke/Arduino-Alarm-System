
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
// Analog pins can function as digital pins.
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
bool doInitilizeLCD = true;

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey;
char accessCode[] = "1234";
char keyPadInput[] = "zzzz";
unsigned char inputCounter = 0;
unsigned char inputMaxCount = 4;
// --KEY PAD END ------------------------------------------------------

// --Active Buzzer START ----------------------------------------------
int buzzer = 12;//the pin of the active buzzer

int ledPin = 13;  // LED on Pin 13, lights up when PIR is active
int powerledPin = 11; // LED on Pin 11, indicates power on, always HIGH
int pirPin = 10; // Input for HC-S501

int pirValue; // Place to store read PIR Value

boolean shouldBeAlerting = false;
enum BuzzStates {ON, OFF};
BuzzStates buzzState = OFF;
int buzzCyclesCounter;
int buzzCyclesCounterMax = 50;
unsigned long chronoStartBuzzA = millis();
int buzzDurationA = 20;
unsigned long chronoStartBuzzB = millis();
int buzzDurationB = 5;
// --Active Buzzer END ------------------------------------------------------

enum ArmedStates {ARMED, DISARMED};
ArmedStates armedState = DISARMED;
short armedTimerInterval = 5;
unsigned long armedTimer = millis();

bool debug = false;

void setup() {
  Serial.begin(9600);
  while (!Serial); // wait for Serial to connect

  pinMode(buzzer, OUTPUT); // set up pin as output

  pinMode(ledPin, OUTPUT); // set up pin as output
  digitalWrite(ledPin, LOW); // set pin value LOW

  pinMode(powerledPin, OUTPUT); // set pin as output
  digitalWrite(powerledPin, HIGH); // set pin HIGH always.

  pinMode(pirPin, INPUT); // set pin as input

  lcd.begin(16, 2); // set up and register the LCD

}

void loop() {

  initilizeLCD();

  handleKeyPadInput();

  handlePIR();

  alert();

}

const char* getStateString(enum ArmedStates armedState) {
  switch (armedState)
  {
    case ARMED: return "ARMED";
    case DISARMED: return "DISARMED";
  }
}

//
void initilizeLCD() {
  if (doInitilizeLCD) {

    doInitilizeLCD = false;
    lcd.clear();
    String a = getStateString(armedState);

    if (debug) {
      Serial.println("armedState: " + a);
    }

    lcd.setCursor(0, 0);
    lcd.print(a);
    lcd.setCursor(0, 1);
    lcd.print("PRESS * to ARM.");
  }
}

void handleLCD(bool shouldClearLCD) {
  if (shouldClearLCD) {
    lcd.clear();
  }

  String a = getStateString(armedState);

  if (debug) {
    Serial.println("armedState: " + a);
  }

  lcd.setCursor(0, 0);
  lcd.print(a);
  lcd.setCursor(0, 1);


  lcd.print("DISARM KEY:");
  uint8_t cursorStart = 11;
  lcd.setCursor(cursorStart, 1);
  lcd.cursor();

  // overwrites the LCD screen positions with blank space or keypad input
  for (int i = 0; i < 4; i++) {
    if (keyPadInput[i] == 'z') {
      lcd.setCursor(cursorStart + i, 1);
      lcd.print(" ");
    } else {
      lcd.setCursor(cursorStart + i, 1);
      lcd.print(keyPadInput[i]);
    }
  }
}

void handleKeyPadInput() {
  customKey = customKeypad.getKey();
  if (customKey) {

    Serial.print("customKey: ");
    Serial.println(customKey);

    if (armedState == DISARMED ) {
      if (customKey == '*') {
        armedState = ARMED;
        handleLCD(true);
      }
    } else {
      if (customKey == '*') {
        resetCodeInput();
      } else if (customKey == '#') {
        if (strcmp(keyPadInput, accessCode) == 0 ) {
          Serial.println("Keypad input matches access code");
          // Disarm the system
          resetCodeInput();
          armedState = DISARMED;
          doInitilizeLCD = true;
          initilizeLCD();
        } else {
          resetCodeInput();
        }
      } else {
        if (inputCounter <= 3) {
          keyPadInput[inputCounter] = customKey;
          inputCounter++;
          handleLCD(true);
        } else {
          // do nothing
        }
      }
    }
  }
}


void resetCodeInput() {
  Serial.println("reseting keyPadInput");
  for (int i = 0; i < 4; i++) {
    keyPadInput[i] = 'z';
  }
  inputCounter = 0;
  handleLCD(true);
}

void handlePIR() {
  pirValue = digitalRead(pirPin);
}

void alert() {
  if (pirValue > 0 && armedState == ARMED ) {
    shouldBeAlerting = true;
  } else {
    shouldBeAlerting = false;
  }

  if (shouldBeAlerting) {
    Serial.println("shouldBeAlerting");
  }

  handleLed();

  handleBuzz();

}

void handleLed () {
  // handle the alarm LCD
  if (shouldBeAlerting) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void handleBuzz() {
  switch (buzzState) {
    case OFF: {
        if (shouldBeAlerting) {
          digitalWrite(buzzer, HIGH);
          buzzState = ON;

        } else {
          digitalWrite(buzzer, LOW);
          buzzState = OFF;
        }
        break;
      }
    case ON: {
        if (shouldBeAlerting) {
          // create a buzz pattern
          if ((buzzCyclesCounter >= 0) && ( buzzCyclesCounter <= 20 )) {
            if (millis() - chronoStartBuzzA >= buzzDurationA) {
              buzzCyclesCounter++;
              chronoStartBuzzA = millis();
              digitalWrite(buzzer, HIGH);
              buzzState = ON;
              if ( buzzCyclesCounter >= buzzCyclesCounterMax) {
                buzzCyclesCounter = 0;
              }
            } else {
              digitalWrite(buzzer, LOW);
              buzzState = OFF;
            }
          } else {
            if (millis() - chronoStartBuzzB >= buzzDurationB) {
              buzzCyclesCounter++;
              chronoStartBuzzB = millis();
              digitalWrite(buzzer, HIGH);
              buzzState = ON;
              if ( buzzCyclesCounter >= buzzCyclesCounterMax) {
                buzzCyclesCounter = 0;
              }
            } else {
              digitalWrite(buzzer, LOW);
              buzzState = OFF;
            }
          }
        } else {
          digitalWrite(buzzer, LOW);
          buzzState = OFF;
        }
        break;
      }
  }
}
