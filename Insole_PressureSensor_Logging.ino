#include <SPI.h>
#include <SD.h>

// Pin Definitions
const int sensor1Pin = A0;
const int sensor2Pin = A1;
const int sensor3Pin = A2;
const int sensor4Pin = A3;
const int sensor5Pin = A4;
const int sensor6Pin = A6;
const int chipSelect = 10;

int sensor1Value = 0;
int sensor2Value = 0;
int sensor3Value = 0;
int sensor4Value = 0;
int sensor5Value = 0;
int sensor6Value = 0;

int logCounter = 0; // track file number
String currentFileName = ""; // track file name
File dataFile;

// find next available log name
void generateNewFileName() {
  while (logCounter < 1000) {
    currentFileName = "log" + String(logCounter) + ".csv";
    if (!SD.exists(currentFileName)) {
      break; 
    }
    logCounter++; // tries the next number up
  }
  
  // make new file with 7 columns
  dataFile = SD.open(currentFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time_ms,5th_Metatarsal,4th_Metatarsal,1st_Metatarsal,Heel,Medial_Arch,Lateral_Arch");
    dataFile.close();
    Serial.print("Created file: ");
    Serial.println(currentFileName);
  }
}

void setup() {
  Serial.begin(115200);
  
  delay(1000); // lets SD card module voltage stabilize w/ capacitor

  // SD card init
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    // blink LED as error signal
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
  }
  Serial.println("initialization done.");

  // generates a new log file with sequential number on battery/power bootup
  generateNewFileName();
}

void loop() {
  // read FSRs
  sensor1Value = analogRead(sensor1Pin);
  sensor2Value = analogRead(sensor2Pin);
  sensor3Value = analogRead(sensor3Pin);
  sensor4Value = analogRead(sensor4Pin);
  sensor5Value = analogRead(sensor5Pin);
  sensor6Value = analogRead(sensor6Pin);
  
  // time in milliseconds
  unsigned long currentMillis = millis();

  // log data into SD card
  dataFile = SD.open(currentFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(currentMillis);
    dataFile.print(",");
    dataFile.print(sensor1Value);
    dataFile.print(",");
    dataFile.print(sensor2Value);
    dataFile.print(",");
    dataFile.print(sensor3Value);
    dataFile.print(",");
    dataFile.print(sensor4Value);
    dataFile.print(",");
    dataFile.print(sensor5Value);
    dataFile.print(",");
    dataFile.println(sensor6Value);
    dataFile.close(); 
  }

  // outputs data to serial monitor/plotter for debug and preliminary testing
  Serial.print("5th_Metatarsal:");
  Serial.print(sensor1Value);
  Serial.print(",");
  Serial.print("4th_Metatarsal:");
  Serial.print(sensor2Value);
  Serial.print(",");
  Serial.print("1st_Metatarsal:");
  Serial.print(sensor3Value);
  Serial.print(",");
  Serial.print("Heel:");
  Serial.print(sensor4Value);
  Serial.print(",");
  Serial.print("Medial_Arch:");
  Serial.print(sensor5Value);
  Serial.print(",");
  Serial.print("Lateral_Arch:");
  Serial.println(sensor6Value);

  delay(20); // 20ms delay, total loop is ~41.5ms (~24Hz)
}
