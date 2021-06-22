# Arduino-Alarm-System
The green led lights continuously from the beginning of the project. 
When the movement is detected, the red led turns on and the buzzer starts to beep.
The system returns to its initial state by entering a password numbered 1234 into the system via keyped.
LiquidCrystal.h library is used for lcd display.
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5) initialize the library with the numbers of the interface pins and 
analog pins can function as digital pins.
Keypad.h library is used for keypad usage.
After define the cymbols on the buttons of the keypads.
rowPins[ROWS] = {9, 8, 7, 6} connect to the row pinouts of the keypad.
colPins[COLS] = {5, 4, 3, 2} connect to the column pinouts of the keypad.
We are starting an instance of the New Keypad class.
We enter the password 1234 into the system via the accessCode [] = "1234" keyped.
buzzer = 12; the pin of the active buzzer.
ledPin = 13; LED on Pin 13, lights up when PIR is active
powerledPin = 11; LED on Pin 11, indicates power on, always HIGH.
pirPin = 10; Input for HC-S501.
pirValue; Place to store read PIR Value.
We start the serial communication in the setup section and wait for the connection to be established.
pinMode(buzzer, OUTPUT) command show us set up pin as output.
pinMode(ledPin, OUTPUT) command show us set up pin as output.
digitalWrite(ledPin, LOW) command show us set pin value LOW.
pinMode(powerledPin, OUTPUT) command show us set pin as output.
digitalWrite(powerledPin, HIGH) command show us set pin HIGH always.
pinMode(pirPin, INPUT) command show us set pin as input.
lcd.begin(16, 2) command show us set up and register the LCD.
