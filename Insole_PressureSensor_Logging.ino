#include <SPI.h>
#include <SD.h>

// Pin Definitions
const int sensor1Pin = A0;
const int sensor2Pin = A1;
const int sensor3Pin = A2;
const int sensor4Pin = A3;
const int sensor5Pin = A4;
const int sensor6Pin = A6; // Note: A6 is analog input ONLY on Arduino Nano
const int chipSelect = 10; // SD card CS pin

// Variables
int sensor1Value = 0;
int sensor2Value = 0;
int sensor3Value = 0;
int sensor4Value = 0;
int sensor5Value = 0;
int sensor6Value = 0;

int logCounter = 0;          // Tracks file suffix number
String currentFileName = ""; // Holds active file name (e.g. "log0.csv")
File dataFile;

// Function to find the next available log file name automatically
void generateNewFileName() {
  while (logCounter < 1000) {
    currentFileName = "log" + String(logCounter) + ".csv";
    // If the file doesn't exist yet, we can use this number!
    if (!SD.exists(currentFileName)) {
      break; 
    }
    logCounter++; // Try the next number up
  }
  
  // Initialize the new file with a header column for all 6 sensors
  dataFile = SD.open(currentFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time_ms,5th_Metatarsal,4th_Metatarsal,1st_Metatarsal,Heel,Medial_Arch,Lateral_Arch");
    dataFile.close();
    Serial.print("Created active file: ");
    Serial.println(currentFileName);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Optional: Safe delay to let the SD card module power stabilize
  delay(1000); 

  // Initialize SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    // Blink the LED continuously on error
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH); // Turn LED on
      delay(500);                      // Wait half a second
      digitalWrite(LED_BUILTIN, LOW);  // Turn LED off
      delay(500);                      // Wait half a second
    }
  }
  Serial.println("initialization done.");

  // Generate a new sequential file immediately on battery/power bootup
  generateNewFileName();
}

void loop() {
  // Read all 6 Sensor Signals
  sensor1Value = analogRead(sensor1Pin);
  sensor2Value = analogRead(sensor2Pin);
  sensor3Value = analogRead(sensor3Pin);
  sensor4Value = analogRead(sensor4Pin);
  sensor5Value = analogRead(sensor5Pin);
  sensor6Value = analogRead(sensor6Pin);
  
  // Get time in milliseconds since the Arduino powered on
  unsigned long currentMillis = millis();

  // 1. Log Data to active SD file
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

  // 2. Output data to Arduino Serial Monitor / Plotter
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
  Serial.println(sensor6Value); // New line only at the end of the line

  delay(20); // 20ms sample rate (~50Hz sampling)
}
