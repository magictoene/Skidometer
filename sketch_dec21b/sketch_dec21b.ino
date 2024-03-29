#include <Arduino_MKRGPS.h>
#include <SD.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // if you use Hardware I2C port, full framebuffer, size = 1024 bytes
File dataFile;


void setup() {
  u8g2.begin();
  
  Serial.begin(9600);
  
  while (!Serial) {
    ; 
  }

  if (!GPS.begin()) {
    Serial.println("Failed to initialize GPS!");
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  writeStartupScreen();

  // Initialisiere die SD-Karte
  if (!SD.begin()) {
    Serial.println("Failed to initialize SD card!");
    while (1);
  }

  // Öffne eine Datei zum Schreiben (die Datei wird erstellt oder überschrieben)
  dataFile = SD.open("gps_data.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println("Latitude,Longitude,Altitude,Speed,Satellites");
    dataFile.close();
  } else {
    Serial.println("Error opening gps_data.txt");
  }
}

void writeDataToFile(float latitude, float longitude, float altitude, float speed, int satellites) {
  // Öffne die Datei im Anhänge-Modus
  dataFile = SD.open("gps_data.txt", FILE_WRITE);

  if (dataFile) {
    // Schreibe die Daten in die Datei
    dataFile.print(latitude, 7);
    dataFile.print(",");
    dataFile.print(longitude, 7);
    dataFile.print(",");
    dataFile.print(altitude);
    dataFile.print(",");
    dataFile.print(speed);
    dataFile.print(",");
    dataFile.println(satellites);

    // Schließe die Datei
    dataFile.close();
  } else {
    Serial.println("Error opening gps_data.txt");
  }
}

void writeStartupScreen()
{
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB12_tr); // choose a suitable font
  u8g2.drawStr(5,35,"SKIDOMETER");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(2000);
}

void writeLoadingScreen(){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(25,10,"SKIDOMETER");
  u8g2.drawStr(0,35, "Looking for Signal");
  u8g2.sendBuffer(); 
  delay(500);  
  u8g2.drawStr(0,35, "Looking for Signal .");
  u8g2.sendBuffer(); 
  delay(500);
  u8g2.drawStr(0,35, "Looking for Signal . .");
  u8g2.sendBuffer(); 
  delay(500);
  u8g2.drawStr(0,35, "Looking for Signal . . .");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(500);
}

void writeInfoScreen(){

}

void writeConnectionSuccessfulScreen(){
  
}

void loop() {

  // Blinken der LED zu Beginn des Programms
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
  writeLoadingScreen();
  // writeInfoScreen();

  // Überprüfen, ob neue GPS-Daten verfügbar sind
  if (GPS.available()) {

    // writeConnectionSuccessfulScreen();
   
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      
      // GPS-Daten lesen und anzeigen
      float latitude   = GPS.latitude();
      float longitude  = GPS.longitude();
      float altitude   = GPS.altitude();
      float speed      = GPS.speed();
      int   satellites = GPS.satellites();

      // GPS-Werte ausgeben
      Serial.print("Standort: ");
      Serial.print(latitude, 7);
      Serial.print(", ");
      Serial.println(longitude, 7);

      Serial.print("Höhe: ");
      Serial.print(altitude);
      Serial.println("m");

      Serial.print("Geschwindigkeit: ");
      Serial.print(speed);
      Serial.println(" km/h");

      Serial.print("Anzahl der Satelliten: ");
      Serial.println(satellites);

      Serial.println();

      // Daten in die SD-Karte schreiben
      writeDataToFile(latitude, longitude, altitude, speed, satellites);
    }
  }
  else {
    writeLoadingScreen();
  }
}



