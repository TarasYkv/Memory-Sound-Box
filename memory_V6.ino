#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <PCF8575.h>

#include <DFRobotDFPlayerMini.h>

/***************************************************
Audio Memory Spiel V6
 for Arduino MEGA
 ***************************************************
 Created 31,05,2020
 By [Taras Yuzkiv](kontakt@digital-ies.de)
 ****************************************************/
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "PCF8575.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define ARDUINO_UNO_INTERRUPTED_PIN 3
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Function interrupt
void keyChangedOnPCF8575();

// Set i2c address
PCF8575 pcf8575(0x20, ARDUINO_UNO_INTERRUPTED_PIN, keyChangedOnPCF8575);

//meine Variablen
int ordner=1;//max 99
int lied=1;//max. 20

//methode zum testen
void playMusic(int ordner1, int lied1){
  myDFPlayer.playFolder(ordner1, lied1);
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void showDetail(DFRobotDFPlayerMini &meinPlayer){
  if (meinPlayer.available()) {
    printDetail(meinPlayer.readType(), meinPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  else Serial.println(F("Player nicht Verfügbar"));
}


void setup() {
  Serial.begin(115200);
  Serial3.begin(9600);
  Serial.println();
  Serial.println(F("Initialisierung läuft ... (Dauer 3-5s)"));
  pcf8575.pinMode(P0, INPUT);
  pcf8575.pinMode(P1, INPUT);
  pcf8575.pinMode(P2, INPUT);
  pcf8575.pinMode(P3, INPUT);
  pcf8575.pinMode(P4, INPUT);
  pcf8575.pinMode(P5, INPUT);
  pcf8575.pinMode(P6, INPUT);
  pcf8575.pinMode(P7, INPUT);
  pcf8575.begin();
  Serial.println("START PCF8575");
  randomSeed(analogRead(0));
  zahlenTauschen();
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  //delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  //display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  //display.display();
  //delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
  testdrawstyles("Hallo", 3000, 4, LOGO_HEIGHT/2, LOGO_WIDTH/2); // Animate bitmaps
  testdrawstyles("mein", 3000, 4, LOGO_HEIGHT/2, LOGO_WIDTH/2); // Animate bitmaps
  testdrawstyles("Name", 3000, 4, LOGO_HEIGHT/2, LOGO_WIDTH/2); // Animate bitmaps
  testdrawstyles("ist", 3000, 4, LOGO_HEIGHT/2, LOGO_WIDTH/2); // Animate bitmaps
  testdrawstyles("MEGA- Memory", 3000, 3, LOGO_HEIGHT/2, LOGO_WIDTH/2); // Animate bitmaps
  //testscrolltext();
  display.display();
  


  if (!myDFPlayer.begin(Serial3)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Verbindung kann nicht aufgebaut werden:"));
    Serial.println(F("1.Bitte überprüfe die Verbindung!"));
    Serial.println(F("2.Bitte SD-Karte einfügen!"));
    while(true);
  }
  Serial.println(F("Spielbereit!"));
  //delay(1000);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
}

bool keyChanged = false;
static unsigned long timer=0;
int liedArray1[3] = {1,2,3};
int liedArray2[3] = {1,2,3};
int zustand[5] = {0,0,0,0,0};//die erste Zahl der erste Taster, die zweite Zahl der zweite Taster; die dritte und die vierte Zahl sind die Adressen und die fünfte ist der Zustand
void loop() {
  //showDetail(myDFPlayer);
  keyChangeHere();

  
  if (millis() - timer > 3000) {
    //timer = millis();
    vergleich(zustand);
  }
  win(zustand);
}
void win(int t_array[5]){
  //Serial.println(t_array[4]);
  if(t_array[4]==3){
    playMusic(11,11);
    Serial.println("geschaft");
    for(int i=0;i<random(0,90);i++){
     int a = random(0,3);
     int b = random(0,3);
     int temp = liedArray1[a];
     liedArray1[a] = liedArray1[b];
     liedArray1[b] = temp;

     int c = random(0,3);
     int d = random(0,3);
     int temp1 = liedArray2[c];
     liedArray2[c] = liedArray2[d];
     liedArray2[d] = temp1;
     zustand[4]=0;
    }  
    delay(3000);
  }
}

void vergleich(int temparray[5]){
  if(temparray[0]!=0 && temparray[1]!=0){
    if(temparray[0] == temparray[1] && temparray[2] != temparray[3]){
       Serial.println(temparray[0]);
       Serial.println(temparray[1]);
       playMusic(21,1);
       temparray[0]=0;
       temparray[1]=0;
       temparray[4]++;
       timer = millis();
    }
    else if(temparray[2] != temparray[3]) {
      playMusic(22,1);
      temparray[0]=0;
      temparray[1]=0;
      timer = millis();
    }
    else{
      temparray[0]=0;
      temparray[1]=0;
      timer = millis();
    }
  }
}

void zahlenTauschen(){
  PCF8575::DigitalInput di = pcf8575.digitalReadAll();
  if(di.p6 == 1) {
    for(int i=0;i<random(0,90);i++){
     int a = random(0,3);
     int b = random(0,3);
     int temp = liedArray1[a];
     liedArray1[a] = liedArray1[b];
     liedArray1[b] = temp;

     int c = random(0,3);
     int d = random(0,3);
     int temp1 = liedArray2[c];
     liedArray2[c] = liedArray2[d];
     liedArray2[d] = temp1;
    }
  }
}


void keyChangeHere(){
  if (keyChanged){
    PCF8575::DigitalInput di = pcf8575.digitalReadAll();
    zahlenTauschen();
    if(di.p0 == 1){
      playMusic(liedArray1[0],1);
      //draw321(); //hierdurch kann eine Verzügerung umgesetzt werden, sowie ein Countdown auf dem Display
      if(zustand[0]==0){
        zustand[0] = liedArray1[0];
        zustand[2] = 1;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray1[0];
        zustand[3] = 1;
      }
    }
    if(di.p1 == 1){
      playMusic(liedArray1[1],1);
      if(zustand[0]==0){
        zustand[0] = liedArray1[1];
        zustand[2] = 2;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray1[1];
        zustand[3] = 2;
      }
    }
    if(di.p2 == 1){
      playMusic(liedArray1[2],1);
      if(zustand[0]==0){
        zustand[0] = liedArray1[2];
        zustand[2] = 3;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray1[2];
        zustand[3] = 3;
      }
    }
    if(di.p3 == 1){
      playMusic(liedArray2[0],1);
      if(zustand[0]==0){
        zustand[0] = liedArray2[0];
        zustand[2] = 4;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray2[0];
        zustand[3] = 4;
      }
    }
    if(di.p4 == 1){
      playMusic(liedArray2[1],1);
      if(zustand[0]==0){
        zustand[0] = liedArray2[1];
        zustand[2] = 5;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray2[1];
        zustand[3] = 5;
      }
    }
    if(di.p5 == 1){
      playMusic(liedArray2[2],1);
      if(zustand[0]==0){
        zustand[0] = liedArray2[2];
        zustand[2] = 6;
      }
      else if(zustand[1]==0){
        zustand[1] = liedArray2[2];
        zustand[3] = 6;
      }
    }
    Serial.print("READ VALUE FROM PCF P1: ");
    Serial.print(di.p0);
    Serial.print(" - ");
    Serial.print(di.p1);
    Serial.print(" - ");
    Serial.print(di.p2);
    Serial.print(" - ");
    Serial.print(di.p3);
    Serial.print(" - ");
    Serial.print(di.p4);
    Serial.print(" - ");
    Serial.print(di.p5);
    Serial.print(" On/Off: ");
    Serial.println(di.p6);
    timer=millis();
    keyChanged= false;
  }
}

void keyChangedOnPCF8575(){
  // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
   keyChanged = true;
}
void draw321(){
  testdrawstyles("3", 2500, 7, 50, LOGO_HEIGHT/2); // Animate bitmaps
  testdrawstyles("2", 2500, 7, 50, LOGO_HEIGHT/2);
  testdrawstyles("1", 2500, 7, 50, LOGO_HEIGHT/2);
  testdrawstyles(" ",2500, 7, 50, LOGO_HEIGHT/2);
}

void testdrawstyles(String myChar, int myDelay, int mySize, int w, int h) {
  display.clearDisplay();
  display.setTextSize(mySize);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(w, h);             // Start at top-left corner
  display.println(myChar);
  display.display();
  delay(myDelay);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll und noch viel mehr..."));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
