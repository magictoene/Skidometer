#include <Arduino_MKRGPS.h>
#include <SD.h>

File dataFile;

void setup() {
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

      // Daten in die SD-Karte schreiben
      writeDataToFile(latitude, longitude, altitude, speed, satellites);
    }

    // Überprüfe, ob GPS-Signal verloren gegangen ist
    if (!GPS.available()) {
      for (int i = 0; i < 5; ++i) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
      }
    }
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
