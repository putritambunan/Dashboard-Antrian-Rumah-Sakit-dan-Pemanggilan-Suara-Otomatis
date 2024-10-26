#include "SoftwareSerial.h"
#include <LiquidCrystal_I2C.h>

// Define the software serial pins for DFPlayer
SoftwareSerial mySerial(10, 11); // RX, TX

// Create DFPlayer Mini object
#define Start_Byte 0x7E
#define Version_Byte 0xFF
#define Command_Length 0x06
#define End_Byte 0xEF
#define Acknowledge 0x00 // Returns info with command 0x41 [0x01: info, 0x00: no info]
#define ACTIVATED LOW

// LCD configuration
int lcdColumns = 16;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Button pins
const int buttonNext = 2;
const int buttonPrevious = 4;
const int buttonPin = 3; // LCD backlight control button
const int buttonAntrian = 5;

// MP3 player state
boolean isPlaying = false; //kode awal untuk dfplayer
int currentTrack = 1;

// LCD display state
int queueNumber = 0; //kondisi awal 0 agar ketika ditekan menjadi 1
int queueNumber1 = 10; // untuk kondisi awal status antrian saat ini
int buttonState = 0;
int lastButtonState = 0;

void playFirst();
void play();
void playNext();
void playPrevious();
void setVolume(int volume);
void execute_CMD(byte CMD, byte Par1, byte Par2);
void displayInitialMessage();
void playQueueNumber(int queueNum);

void setup() {
  // Initialize serial for DFPlayer
  mySerial.begin(9600);
  Serial.begin(9600);

  // Initialize buttons
  pinMode(buttonNext, INPUT);
  digitalWrite(buttonNext, HIGH);
  pinMode(buttonPrevious, INPUT);
  digitalWrite(buttonPrevious, HIGH);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  pinMode(buttonAntrian, INPUT);
  digitalWrite(buttonAntrian, HIGH);

  // Play the first track
  // playFirst();
  isPlaying = true;

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  // Print initial messages
  lcd.print("Ambil Antrian"); //lcd menampilkan ambil antrian
}

void loop() {
  // Handle MP3 player button presses
  if (digitalRead(buttonNext) == ACTIVATED) {
    if (isPlaying) {
      playQueueNumber(queueNumber + 2); //
    }
  }

  if (digitalRead(buttonPrevious) == ACTIVATED) {
    if (isPlaying) {
      playQueueNumber(queueNumber1 + 2);
    }
  }

  // Handle LCD backlight control button (buttonAntrian)
  buttonState = digitalRead(buttonAntrian);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      // Increment queue number
      queueNumber++;
      
      // Reset queue number to 1 if it exceeds 10
      if (queueNumber > 10) {
        queueNumber = 1;
      }

      // Turn on LCD backlight
      lcd.backlight();

      // Display the current queue number
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selamat Datang");
      lcd.setCursor(0, 1);
      lcd.print("Nomor Antrian");
      lcd.setCursor(0, 2);
      lcd.print("Anda Adalah");
      lcd.setCursor(0, 3);
      lcd.print("Nomor: ");
      lcd.print(queueNumber);

      // Delay to prevent bouncing
      delay(5000);//baru diubah

      // Clear only the fourth row
      lcd.setCursor(0, 3);
      lcd.print("              "); // Clear the line

      // Turn off LCD backlight
      lcd.backlight();
      lcd.clear();

      // Print message to press the button again
      lcd.setCursor(0, 0);
      lcd.print("Tekan Tombol");
      lcd.setCursor(0, 1);
      lcd.print("Untuk Menyalakan");
      lcd.setCursor(0, 2);
      lcd.print("Layar");
    }

    lastButtonState = buttonState;
  }

  // Handle LCD backlight control button (buttonPin)
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {

      if (queueNumber == 1) { //mengecek nomor antrian yang dilayani
        queueNumber1 = queueNumber1;
      }
      else if (queueNumber > 1 && queueNumber <= 10) { // lebih besar dari 1 dan lebih kecil dari 10
        queueNumber1 = queueNumber -1;
      } 
      else if (queueNumber > 10) {
        queueNumber1 = 1;
      }
      // Turn on LCD backlight
      lcd.backlight();

      // Display the current queue number
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nomor Antrian");
      lcd.setCursor(0, 1);
      lcd.print("Yang Dilayani");
      lcd.setCursor(0, 2);
      lcd.print("Saat Ini");
      lcd.setCursor(0, 3);
      lcd.print("Nomor: ");
      lcd.print(queueNumber1);
      }

      // Delay to prevent bouncing
      delay(5000);

      // Clear only the fourth row
      lcd.setCursor(0, 3);
      lcd.print("              "); // Clear the line

      // Turn off LCD backlight
      lcd.backlight();
      lcd.clear();

      // Print message to press the button again
      lcd.setCursor(0, 0);
      lcd.print("Tekan Tombol");
      lcd.setCursor(0, 1);
      lcd.print("Untuk Menyalakan");
      lcd.setCursor(0, 2);
      lcd.print("Layar");
    }

    lastButtonState = buttonState;
  }


void execute_CMD(byte CMD, byte Par1, byte Par2) {
  // Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte };
  // Send the command line to the module
  for (byte k = 0; k < 10; k++) {
    mySerial.write(Command_line[k]);
  }
}

void playQueueNumber(int queueNum) {
  execute_CMD(0x03, 0, queueNum); // Perintah untuk memutar track berdasarkan nomor antrian
  delay(6000);
}

// void playFirst() {
//   execute_CMD(0x3F, 0, 0);
//   delay(500);
//   setVolume(30);
//   delay(500);
//   execute_CMD(0x11, 0, 1);
//   delay(500);
// }

// void play() {
//   execute_CMD(0x0D, 0, 1);
//   delay(500);
// }

// void playNext() {
//   execute_CMD(0x01, 0, 1);
//   delay(500);
// }

// void playPrevious() {
//   execute_CMD(0x02, 0, 1);
//   delay(500);
// }

// void setVolume(int volume) {
//   execute_CMD(0x30, 0, volume); // Set the volume (0x00~0x30)
//   delay(2000);
// }
