/*
  Simple Arduino Chess Clock
  Ronaldo Rezende Jr (@naldin)
  ronaldo.rrj at gmail.com
  08/2018
  https://github.com/naldin/ArduinoChessClock
  http://xyzturbo.blogspot.com

  Features:
   * Complete timer adjust using keypad to hour, minutes and seconds, of 1 sec up to 10 hours
   * Storage last adjust in eeprom
   * Time control by Sudden Death or Increment of up to 99 min
   * Pause timer using any keypad key and release using game buttoon
   * Sound to check that game button was pressed and when game is over

  3D parts:
  https://www.thingiverse.com/thing:3043109

  Libraries used:
  https://www.arduino.cc/en/Reference/EEPROM
  https://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay
  https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>

//defining seconds count
#define SEC (60 +(millis() / 1000))

//define keypad buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//constants
const int buttonWhite = 8;
const int buttonBlack = 9;
const int pinBuzzer = 10;
const int analogPin = A0;

//Variables for keypad and menu
int lcd_key = 0;
int adc_key_in = 0;
int countMenu = 0;
int blinkTime;
int initBlinkTime;
int blinkDelay = 500;
bool exitMenu = true;
bool sidePlayer = true;
bool blinkState = true;

//Variables for seconds count
int cTemp = 0;
int cSecWhite = 0;
int cSecBlack = 0;

//Increment variable
int setInc;

//Variables to white timer
int setSecWhite;
int horWhite;
int minWhite;
int secWhite;

//Variables to black timer
int setSecBlack;
int horBlack;
int minBlack;
int secBlack;

//LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//fix bounce of keys
void debounceKey() {
  while (adc_key_in < 1000) {
    adc_key_in = analogRead(analogPin);
  }
}

//keypad code by:
//https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
//read keypad buttons
int read_LCD_buttons()
{
  adc_key_in = analogRead(analogPin);      // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result

  // For V1.1 us this threshold
  /*if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 450)  return btnDOWN;
    if (adc_key_in < 650)  return btnLEFT;
    if (adc_key_in < 850)  return btnSELECT;
  */

  // For V1.0 comment the other threshold and use the one below:
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;  // when all others fail, return this...
}

//set up menu
void menuSetUp() {

  lcd_key = read_LCD_buttons();  // read the buttons

  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      {
        countMenu++;
        if (countMenu > 4) countMenu = 0;
        debounceKey();
        break;
      }
    case btnLEFT:
      {
        countMenu--;
        if (countMenu < 0) countMenu = 4;
        debounceKey();
        break;
      }
    case btnUP:
      {
        if (countMenu == 0) { //increase hour
          horWhite++;
          if (horWhite > 9) horWhite = 0;
          debounceKey();
        }
        else if (countMenu == 1) { //increase minutes
          minWhite++;
          if (minWhite > 59) minWhite = 0;
          debounceKey();
        }
        else if (countMenu == 2) { //increase seconds
          secWhite++;
          if (secWhite > 59) secWhite = 0;
          debounceKey();
        }
        else if (countMenu == 3) { //increase increment
          setInc++;
          if (setInc > 99) setInc = 0;
          debounceKey();
        }
        else { //change WB to BW
          sidePlayer ^= 1;
          debounceKey();
        }
        break;
      }
    case btnDOWN:
      {
        if (countMenu == 0) { //decrease hour
          horWhite--;
          if (horWhite < 0) horWhite = 9;
          debounceKey();
        }
        else if (countMenu == 1) { //decrease minutes
          minWhite--;
          if (minWhite < 0) minWhite = 59;
          debounceKey();
        }
        else if (countMenu == 2) { //decrease seconds
          secWhite--;
          if (secWhite < 0) secWhite = 59;
          debounceKey();
        }
        else if (countMenu == 3) { //decrease increment
          setInc--;
          if (setInc < 0) setInc = 99;
          debounceKey();
        }
        else { //change WB to BW
          sidePlayer ^= 1;
          debounceKey();
        }
        break;
      }
    case btnSELECT: //exit set up menu
      {
        exitMenu = 0;
        break;
      }
    case btnNONE:
      {
        break;
      }
  }
}

//printing white values to LCD and print timer to set up menu
void printTimerWhite() {

  //used to timer blink
  blinkTime = millis() - blinkDelay;

  //printing hour blinking
  //used only in set up
  if (exitMenu == 1 && countMenu == 0) {
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      lcd.setCursor(0, 1);
      lcd.print(horWhite);
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  //printing hour withouy blink
  else {
    lcd.setCursor(0, 1);
    lcd.print(horWhite);
  }
  lcd.setCursor(1, 1);
  lcd.print(":");

  //printing minutes blinking
  //used only in set up
  if (exitMenu == 1 && countMenu == 1) {
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(2, 1);
      lcd.print("  ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      //this fix the print value below 10 moving the cursor one point and put 0 to left
      if ((minWhite < 10) && (minWhite >= 0)) {
        lcd.setCursor(2, 1);
        lcd.print(0);
        lcd.setCursor(3, 1);
        lcd.print(minWhite);
      }
      else {
        lcd.setCursor(2, 1);
        lcd.print(minWhite);
      }
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  //printing minutes without blink
  else {
    //this fix the print value below 10 moving the cursor one point and put 0 to left
    if ((minWhite < 10) && (minWhite >= 0)) {
      lcd.setCursor(2, 1);
      lcd.print(0);
      lcd.setCursor(3, 1);
      lcd.print(minWhite);
    }
    else {
      lcd.setCursor(2, 1);
      lcd.print(minWhite);
    }
  }
  lcd.setCursor(4, 1);
  lcd.print(":");

  //print seconds blinking
  //used only in set up
  if (exitMenu == 1 && countMenu == 2) {
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(5, 1);
      lcd.print("  ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      //this fix the print value below 10 moving the cursor one point and put 0 to left
      if ((secWhite < 10) && (secWhite >= 0)) {
        lcd.setCursor(5, 1);
        lcd.print(0);
        lcd.setCursor(6, 1);
        lcd.print(secWhite);
      }
      else {
        lcd.setCursor(5, 1);
        lcd.print(secWhite);
      }
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  //print seconds without blink
  else {
    //this fix the print value below 10 moving the cursor one point and put 0 to left
    if ((secWhite < 10) && (secWhite >= 0)) {
      lcd.setCursor(5, 1);
      lcd.print(0);
      lcd.setCursor(6, 1);
      lcd.print(secWhite);
    }
    else {
      lcd.setCursor(5, 1);
      lcd.print(secWhite);
    }
  }
}

//printing black values to LCD
void printTimerBlack() {

  //printing hour
  lcd.setCursor(9, 1);
  lcd.print(horBlack);
  lcd.setCursor(10, 1);
  lcd.print(":");

  //printing minutes
  //this fix the print value below 10 moving the cursor one point and put 0 to left
  if ((minBlack < 10) && (minBlack >= 0)) {
    lcd.setCursor(11, 1);
    lcd.print(0);
    lcd.setCursor(12, 1);
    lcd.print(minBlack);
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print(minBlack);
  }
  lcd.setCursor(13, 1);
  lcd.print(":");

  //print seconds
  //this fix the print value below 10 moving the cursor one point and put 0 to left
  if ((secBlack < 10) && (secBlack >= 0)) {
    lcd.setCursor(14, 1);
    lcd.print(0);
    lcd.setCursor(15, 1);
    lcd.print(secBlack);
  }
  else {
    lcd.setCursor(14, 1);
    lcd.print(secBlack);
  }
}

//print menu set up
void printMenu() {

  lcd.setCursor(0, 0);
  lcd.print("SET UP W/ KEYPAD");
  lcd.setCursor(8, 1);
  lcd.print("Inc");

  //used to timer blink
  blinkTime = millis() - blinkDelay;

  //print increment value to set up
  if (countMenu == 3) {

    //print increment value blinking
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(11, 1);
      lcd.print("  ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      //this fix the print value below 10 moving the cursor one point and put 0 to left
      if ((setInc < 10) && (setInc >= 0)) {
        lcd.setCursor(11, 1);
        lcd.print(0);
        lcd.setCursor(12, 1);
        lcd.print(setInc);
      }
      else {
        lcd.setCursor(11, 1);
        lcd.print(setInc);
      }
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  //print increment value without blink
  else {
    //this fix the print value below 10 moving the cursor one point and put 0 to left
    if ((setInc < 10) && (setInc >= 0)) {
      lcd.setCursor(11, 1);
      lcd.print(0);
      lcd.setCursor(12, 1);
      lcd.print(setInc);
    }
    else {
      lcd.setCursor(11, 1);
      lcd.print(setInc);
    }
  }

  //print side player (WB or BW) blinking
  if (exitMenu == 1 && countMenu == 4) {
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(14, 1);
      lcd.print("  ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      if (sidePlayer == 1) {
        lcd.setCursor(14, 1);
        lcd.print("WB");
      }
      else {
        lcd.setCursor(14, 1);
        lcd.print("BW");
      }
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  //print side player (WB or BW) without blink
  else {
    if (sidePlayer == 1) {
      lcd.setCursor(14, 1);
      lcd.print("WB");
    }
    else {
      lcd.setCursor(14, 1);
      lcd.print("BW");
    }
  }
}

//printing labels
void printLabels() {

  lcd.clear();

  if (sidePlayer == 1) { //seleted in menuSetUp
    lcd.setCursor(0, 0);
    lcd.print("WHITE");
    lcd.setCursor(11, 0);
    lcd.print("BLACK");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("BLACK");
    lcd.setCursor(11, 0);
    lcd.print("WHITE");
  }
  lcd.setCursor(7, 1);
  lcd.print("||");
  lcd.setCursor(6, 0);
  lcd.print("|  |");

  //this fix the print value below 10 moving the cursor one point and put 0 to left
  if ((setInc < 10) && (setInc >= 0)) {
    lcd.setCursor(7, 0);
    lcd.print(0);
    lcd.print(setInc);
    lcd.setCursor(8, 0);
  }
  else {
    lcd.setCursor(7, 0);
    lcd.print(setInc);
  }

  tone(pinBuzzer, 500, 300);
  digitalWrite(LED_BUILTIN, LOW);

  //printing timers values
  printTimerWhite();
  printTimerBlack();
}

void pauseGameWhite() {

  //with game paused in timer white, for exit is necessary push button black
  while (digitalRead(buttonBlack) == 0) {

    //used to timer blink
    blinkTime = millis() - blinkDelay;

    //print < caracter blinking
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(5, 0);
      lcd.print(" ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      lcd.setCursor(5, 0);
      lcd.print("<");
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  lcd.setCursor(5, 0);
  lcd.print(" ");
}

void pauseGameBlack() {

  //with game paused in timer white, for exit is necessary push button black
  while (digitalRead(buttonWhite) == 0) {

    //used to timer blink
    blinkTime = millis() - blinkDelay;

    //print > caracter blinking
    if (blinkTime >= initBlinkTime && blinkState == 1) {
      lcd.setCursor(10, 0);
      lcd.print(" ");
      blinkState = 0;
      initBlinkTime = millis();
    }
    if (blinkTime >= initBlinkTime && blinkState == 0) {
      lcd.setCursor(10, 0);
      lcd.print(">");
      blinkState = 1;
      initBlinkTime = millis();
    }
  }
  lcd.setCursor(10, 0);
  lcd.print(" ");
}

//running timer white
void timerWhite() {

  //cSecWhite is a counter that will increase with millis() due SEC define.
  //The variable cSecWhite is updated in Arduino loop() (cSecWhite = SEC - cTemp).

  //seconds timer (secWhite) = seconds selected in menu set up (setSecWhite) minus
  //cSecWhite that will increase with milles() due SEC define.
  secWhite = setSecWhite - cSecWhite;

  printTimerWhite(); //update values

  //* making the timer *//
  //if seconds timer below 0(-1) and minutes >=0 plus counter seconds with 59 to
  //setSecWhite update the value and remove 1 in minutes timer
  if ((secWhite == -1) && (minWhite >= 0)) {
    setSecWhite = cSecWhite + 59; //update setSecWhite
    minWhite--;
  }
  //if seconds and minutes timer below 0(-1) and hour is > 0 plus 59 to
  //setSecWhite update the value,put minutes to 59 and remove 1 to hour
  if ((secWhite == -1) && (minWhite == -1) && (horWhite > 0)) {
    setSecWhite = cSecWhite + 59;
    minWhite = 59;
    horWhite--;
  }

  //if seconds, minutes and hour timers go to 0 loop forever and print end game
  if ((secWhite == 0) && (minWhite == 0) && (horWhite == 0)) {
    tone(pinBuzzer, 500, 1000);
    while (1) {
      lcd.setCursor(10, 0);
      lcd.print("      ");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      lcd.setCursor(10, 0);
      lcd.print("WINNER");
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }
}

//running timer black
void timerBlack() {

  secBlack = setSecBlack - cSecBlack;

  printTimerBlack();

  //timer
  if ((secBlack == -1) && (minBlack >= 0)) {
    setSecBlack = cSecBlack + 59;
    minBlack--;
  }
  if ((secBlack == -1) && (minBlack == -1) && (horBlack > 0)) {
    setSecBlack = cSecBlack + 59;
    minBlack = 59;
    horBlack--;
  }

  //end game
  if ((secBlack == 0) && (minBlack == 0) && (horBlack == 0)) {
    tone(pinBuzzer, 500, 1000);
    while (1) {
      lcd.setCursor(0, 0);
      lcd.print("      ");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("WINNER");
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }
}

//increment to white
void incWhite() {
  if (setInc > 0) {
    secWhite = secWhite + setInc; //increment seconds to seconds timer
    cSecWhite = setSecWhite - secWhite; //start new count

    //if seconds timer + increment > 59 and < 120 add 1 to minutes and remove 60 seconds to seconds timer
    if (secWhite > 59 && secWhite < 120) {
      secWhite = secWhite - 60;
      cSecWhite = setSecWhite - secWhite;
      minWhite++;
    }
    //if seconds timer + increment > 119 add 2 to minutes and remove 120 seconds to seconds timer
    else if (secWhite > 119) {
      secWhite = secWhite - 120;
      cSecWhite = setSecWhite - secWhite;
      minWhite = minWhite + 2;
    }
    //if minutes timer > 59 add 1 to hour and remove 60 to minutes timer
    if (minWhite > 59) {
      minWhite = minWhite - 60;
      horWhite++;
    }
    printTimerWhite(); //print new values
  }
}

//increment to black
void incBlack() {
  if (setInc > 0) {
    secBlack = secBlack + setInc;
    cSecBlack = setSecBlack - secBlack;
    if (secBlack > 59 && secBlack < 120) {
      secBlack = secBlack - 60;
      cSecBlack = setSecBlack - secBlack;
      minBlack++;
    }
    else if (secBlack > 119) {
      secBlack = secBlack - 120;
      cSecBlack = setSecBlack - secBlack;
      minBlack = minBlack + 2;
    }
    if (minBlack > 59) {
      minBlack = minBlack - 60;
      horBlack++;
    }
    printTimerBlack();
  }
}

void readEeprom() {

  //used in case Arduino was used in another aplication
  if (EEPROM.read(0) > 59) EEPROM.write(0, 0);
  if (EEPROM.read(1) > 59) EEPROM.write(1, 0);
  if (EEPROM.read(2) > 9) EEPROM.write(2, 0);
  if (EEPROM.read(3) > 99) EEPROM.write(3, 0);
  if (EEPROM.read(4) > 1) EEPROM.write(4, 1);

  //load values to variables
  secWhite = EEPROM.read(0);
  minWhite = EEPROM.read(1);
  horWhite = EEPROM.read(2);
  setInc = EEPROM.read(3);
  sidePlayer = EEPROM.read(4);
}

void writeEeprom() {

  //write values that have been selected to eeprom
  EEPROM.write(0, secWhite);
  EEPROM.write(1, minWhite);
  EEPROM.write(2, horWhite);
  EEPROM.write(3, setInc);
  EEPROM.write(4, sidePlayer);
}

//Arduino setup
void setup() {

  //setup pins
  pinMode(buttonWhite, INPUT); //button white
  pinMode(buttonBlack, INPUT); //button black
  pinMode(analogPin, INPUT);   //used in keypad
  pinMode(pinBuzzer, OUTPUT);  //buzzer
  pinMode(LED_BUILTIN, OUTPUT);//led pin 13

  //start LCD
  lcd.begin(16, 2);
  lcd.clear();

  //start message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Simple Arduino");
  lcd.setCursor(0, 1);
  lcd.print("Chess Clock");
  delay(2000);
  lcd.clear();

  digitalWrite(LED_BUILTIN, HIGH);

  readEeprom(); //read eeprom values and load to variables

  while (exitMenu) {    //start set up menu
    menuSetUp();        //keypad set up
    printMenu();        //print menu values
    printTimerWhite(); //print setup timer in left below
  }

  writeEeprom(); //write values selected to eeprom

  //setting up values for white and black to start timers
  horBlack = horWhite;
  minBlack = minWhite;
  secBlack = secWhite;
  setSecBlack = secWhite;
  setSecWhite = secWhite;

  printLabels(); //printing labels
}

//Arduino loop
void loop() {

  adc_key_in = analogRead(analogPin); //read keypad

  //push black button run white timer
  if (digitalRead(buttonBlack)) {
    tone(pinBuzzer, 150, 100);
    cTemp = SEC - cSecWhite; //difference between SEC and previous seconds count
    while (digitalRead(buttonWhite) == 0 && adc_key_in > 1000) { //if white button or any keypad pushed exit loop
      cSecWhite = SEC - cTemp; //second count for white timer
      timerWhite(); //run timer white
      adc_key_in = analogRead(analogPin); //read keypad
    }
    if (adc_key_in < 1000) { //if any keypad pushed go to pause
      pauseGameWhite();
    }
    else {
      incWhite(); //increment seconds
    }
  }

  //push white button run black timer
  if (digitalRead(buttonWhite)) {
    tone(pinBuzzer, 150, 100);
    cTemp = SEC - cSecBlack;
    while (digitalRead(buttonBlack) == 0 && adc_key_in > 1000) {
      cSecBlack = SEC - cTemp;
      timerBlack();
      adc_key_in = analogRead(analogPin);
    }
    if (adc_key_in < 1000) {
      pauseGameBlack();
    }
    else {
      incBlack();
    }
  }
}

