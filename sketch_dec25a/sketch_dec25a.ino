#include <Arduino_MKRGPS.h>
#include <SD.h>
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
  // Entferne oder reduziere die Wartezeit auf die serielle Verbindung
  // Warte maximal 5 Sekunden
  unsigned long startMillis = millis();
  while (!Serial && millis() - startMillis < 5000) {
    delay(100);
  }

  if (!GPS.begin()) {
    Serial.println("Failed to initialize GPS!");
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Initializing Display.");
  // Start the Display
  writeStartupScreen();


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

void loop() {
  // Blinken der LED zu Beginn des Programms
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

  
  // Überprüfen, ob neue GPS-Daten verfügbar sind
  if (GPS.available()) {

    writeConnectionSuccessfulScreen();
    delay(1000);

    while (GPS.available()) {
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

      // writeInfoScreen(altitude, speed, satellites);

      // Daten in die SD-Karte schreiben
      // writeDataToFile(latitude, longitude, altitude, speed, satellites);
      writeDataToFileGPX(latitude, longitude, altitude, speed, satellites);
    }

    // Überprüfe, ob GPS-Signal verloren gegangen ist
    /* if (!GPS.available()) {
      for (int i = 0; i < 5; ++i) {
        
      }
    } */
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

    writeLoadingScreen();
  }
}

void writeStartupScreen(){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB12_tr); // choose a suitable font
  u8g2.drawStr(0,35,"SKIDOMETER");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(2000);
}

void writeLoadingScreen(){      
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(25,10,"SKIDOMETER");
  u8g2.drawStr(23,35,"No GPS Signal");
  u8g2.drawStr(0,60, "Looking for Signal");
  u8g2.sendBuffer(); 
  delay(500);  
  u8g2.drawStr(0,60, "Looking for Signal .");
  u8g2.sendBuffer(); 
  delay(500);
  u8g2.drawStr(0,60, "Looking for Signal . .");
  u8g2.sendBuffer(); 
  delay(500);
  u8g2.drawStr(0,60, "Looking for Signal . . .");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(500);
}

void writeInfoScreen(float altitude, float speed, int satellites){
  
  char gps_signal[] = "";

  if (satellites <= 2){
    gps_signal = "Weak";
  } else if (satellites > 2 && satellites <= 4){
    gps_signal = "Moderate";
  } else if (satellites > 4 && satellites <= 6){
    gps_signal = "Strong";
  } else if (satellites > 6 && satellites <= 8){
    gps_signal = "Excellent";
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0,10,"SKIDOMETER");
  u8g2.drawStr(0,25,"Altitude:");
  u8g2.setCursor(60,25);
  u8g2.print(altitude);
  u8g2.drawStr(0,35,"Speed:");
  u8g2.setCursor(60,35);
  u8g2.print(speed);
  u8g2.drawStr(0,45,"Signal");
  /* u8g2.setCursor(60,45);
  u8g2.print(satellites); */
  u8g2.drawStr(60,45,gps_signal);
  u8g2.sendBuffer();
  delay(500);
}

void writeConnectionSuccessfulScreen(){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB12_tr); // choose a suitable font
  u8g2.drawStr(5,35,"Connection established!");
  u8g2.drawStr(0,60,"Have fun on the Slopes ^^");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(2000);
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

void writeDataToFileGPX(float latitude, float longitude, float altitude, float speed, int satellites) {
  // Öffne die Datei im Anhänge-Modus
  dataFile = SD.open("gps_data.gpx", FILE_WRITE);

  if (dataFile) {
    // Schreibe den Anfang der GPX-Datei
    dataFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
    dataFile.println("<gpx version=\"1.1\" creator=\"Your GPS Logger\">");

    // Schreibe den Trackpunkt mit den GPS-Daten
    dataFile.print("<wpt lat=\"");
    dataFile.print(latitude, 7);
    dataFile.print("\" lon=\"");
    dataFile.print(longitude, 7);
    dataFile.println("\">");

    dataFile.print("  <ele>");
    dataFile.print(altitude);
    dataFile.println("</ele>");

    dataFile.print("  <speed>");
    dataFile.print(speed);
    dataFile.println("</speed>");

    dataFile.print("  <satellites>");
    dataFile.print(satellites);
    dataFile.println("</satellites>");

    dataFile.println("</wpt>");

    // Schließe den Track und die GPX-Datei
    dataFile.println("</gpx>");
    dataFile.close();
  } else {
    Serial.println("Error opening gps_data.gpx");
  }
}
