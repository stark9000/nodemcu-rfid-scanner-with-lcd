
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 16, 2);

byte SS_PIN = D3;
byte RST_PIN = D4;
byte BUZZER = D8;

#include <SPI.h>
#include <MFRC522.h>

byte nuidPICC[4];

/*
  lcd module:
  5v       pin -> to -> 5v
  gnd      pin -> to -> gnd
  scl      pin -> to -> D1
  sda      pin -> to -> D2

  rfid card module :
  3.3v     pin -> to -> 3.3v
  gnd      pin -> to -> gnd

  sda(ss)  pin -> to -> D3
  rst      pin -> to -> D4

  sck      pin -> to -> D5
  mosi     pin -> to -> D7
  miso     pin -> to -> D6

  buzzer:
  buz      pin -> to -> D8
*/

MFRC522 mfrc522(SS_PIN, RST_PIN);

String content = "";

void setup() {

  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(BUZZER, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print("      RFID");
  lcd.setCursor(0, 1);
  lcd.print("     Scanner");

  digitalWrite(BUZZER, HIGH);
  delay(1000);
  digitalWrite(BUZZER, LOW);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Scan RFID");

}
void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  for (int i = 12; i < 16; i++) {
    lcd.setCursor(i, 0);
    lcd.print(" ");
  }

  if (mfrc522.uid.uidByte[0] != nuidPICC[0] || mfrc522.uid.uidByte[1] != nuidPICC[1] || mfrc522.uid.uidByte[2] != nuidPICC[2] || mfrc522.uid.uidByte[3] != nuidPICC[3] ) {
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = mfrc522.uid.uidByte[i];
    }
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
    lcd.setCursor(0, 1);
    lcd.print("ID:");
    lcd.setCursor(3, 1);
    lcd.print(content);
    Serial.println(content);
    content = "";
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);
  }
  else {
    Serial.println("Card read previously.");
    lcd.setCursor(13, 0);
    lcd.print("CAM");//card already marked
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
}
