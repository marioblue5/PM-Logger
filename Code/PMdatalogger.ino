#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h> // Include library for RTC
#include <pm2008_i2c.h>
#include <EEPROM.h>

PM2008_I2C pm2008_i2c;

// Pin configuration for SD card
const char* dataName= "6_data.csv";
const char* bdataName= "6_bdata.csv";
const uint8_t chipSelect = 10; // Chip select pin for the SD card
const uint8_t redLED = 6; //Digital IO port
const uint8_t blueLED = 5; // Digital IO port
const uint8_t yellowLED = 4; // Digital IO port
uint8_t k = 0; // counter for correct data cycles
uint8_t j = 1; // counter for battery voltage
float battery_status;
float batteryPercentageTotal= 0;
// Keep track whether the sensor needs to warmup or not.
bool warmFlag = true;
// Create an RTC object
RTC_PCF8523 rtc;

// EEPROM address and magic value
const int EEPROM_ADDR = 0;

// Simple hash function for compile date and time
uint16_t generateCompileHash() {
  const char* compileDate = __DATE__;
  const char* compileTime = __TIME__;
  uint16_t hash = 0;
  while (*compileDate) hash = hash * 31 + *compileDate++;
  while (*compileTime) hash = hash * 31 + *compileTime++;
  return hash;
}

void pulseBlue() {
  digitalWrite(blueLED, LOW); // Turn on the blue LED
  delay(250); // Wait for 0.25 seconds
  digitalWrite(blueLED, HIGH); // Turn off the blue LED
}

void delayUntilNextInterval() {
  DateTime now = rtc.now();
  int currentSecond = now.second();
  int remainingTime = 15 - (currentSecond % 15);
  unsigned long start = millis();
  unsigned long waitTime = remainingTime * 1000;

  while (millis() - start < waitTime) {
    // Perform any background tasks or just wait
    delay(10); // Small delay to avoid busy waiting
  }
}

void setup() {
  // Open serial communications
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED,OUTPUT);
  digitalWrite(redLED,HIGH);
  digitalWrite(blueLED,HIGH);
  digitalWrite(yellowLED,HIGH);
  #ifdef PM2008N
  // wait for PM2008N to be changed to I2C mode
  delay(10000);
#endif
  pm2008_i2c.begin();
  pm2008_i2c.command();
  delay(1000);
  Serial.begin(9600);

  // Check if the RTC is running, otherwise initialize it
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    digitalWrite(redLED,LOW);
    while (true) {
      digitalWrite(redLED,LOW);
      delay(1000);
      digitalWrite(redLED,HIGH);
      delay(1000);
    }
  }

  if (!rtc.initialized()) {
    Serial.println(F("RTC is NOT initialized!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Check if the RTC lost power and if it needs the time set
  if (!rtc.isrunning()|| rtc.lostPower()) {
    Serial.println(F("RTC is NOT running!"));
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    rtc.start();
  }
  // See if the card is present and can be initialized
  while (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // Don't do anything more
    digitalWrite(redLED,LOW);
    delay(500);
  }

  // Generate hash of compile date and time
  uint16_t currentHash = generateCompileHash();

  // Read the magic value from EEPROM
  uint16_t storedHash;
  EEPROM.get(EEPROM_ADDR, storedHash);

  // Check if the stored hash matches the current hash
  if (storedHash != currentHash) {
    Serial.println(F("First run after code upload, updating RTC time..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    rtc.start(); // Ensure the RTC is started

    // Store the new hash in EEPROM
    EEPROM.put(EEPROM_ADDR, currentHash);
  } else {
    Serial.println(F("RTC time not updated, hashes are the same!"));
  }

  DateTime now = rtc.now();
  Serial.print(F("Current RTC time: "));
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  digitalWrite(redLED,HIGH);
  Serial.println(F("SD Card initialized."));
  initializeFile(dataName);
  initializeFile(bdataName);
}

void initializeFile(const char* filename) {
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    if (dataFile.size() == 0) {
      dataFile.println("Year,Month,Day,Hour,Minute,Second,PM 1.0 (GRIMM),PM 2.5 (GRIMM),PM 10 (GRIMM),0.3 um,0.5 um,1 um,2.5 um,5 um,10 um,battery");
    }
    dataFile.close();
  } else {
    Serial.print(F("Error opening "));
    Serial.println(filename);
  }
}

void logData(const char* filename, float batteryPercentage) {
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.print(now.year(), DEC);
    dataFile.print(',');
    dataFile.print(now.month(), DEC);
    dataFile.print(',');
    dataFile.print(now.day(), DEC);
    dataFile.print(',');
    dataFile.print(now.hour(), DEC);
    dataFile.print(',');
    dataFile.print(now.minute(), DEC);
    dataFile.print(',');
    dataFile.print(now.second(), DEC);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.pm1p0_grimm);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.pm2p5_grimm);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.pm10_grimm);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_0p3_um);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_0p5_um);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_1_um);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_2p5_um);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_5_um);
    dataFile.print(',');
    dataFile.print(pm2008_i2c.number_of_10_um);
    dataFile.print(',');
    dataFile.println(batteryPercentage);
    dataFile.close();
    Serial.print(F("Successfully saved "));
    Serial.println(filename);
  } else {
    Serial.print(F("Error opening "));
    Serial.println(filename);
    digitalWrite(redLED,LOW);
    delay(6000);
    digitalWrite(redLED,HIGH);
    delay(100);
    Serial.print(F("Trying to fix SD card: "));
    if (SD.begin(chipSelect)) {
      Serial.println(F("Success!"));
    } else{
      Serial.println(F("Failed :(, try restarting"));
    }
  }
}

void loop() {
  delayUntilNextInterval();
  uint8_t ret = pm2008_i2c.read();
  int sensorValue = analogRead(A1);
  float batteryVoltage = sensorValue * (5.0 / 1023.0);
  float batteryPercentage = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100;
  batteryPercentage = constrain(batteryPercentage, 0, 100);

  batteryPercentageTotal += batteryPercentage * 0.2;
  if (j == 5) {
    if (batteryPercentageTotal <= 40) {
      digitalWrite(yellowLED, LOW);
    } else {
      digitalWrite(yellowLED, HIGH);
    }
    Serial.print("Current battery % total is: ");
    Serial.println(batteryPercentageTotal);
    batteryPercentageTotal = 0;
    j = 1;
  }
  j++;

  if (ret == 0) {
    if (warmFlag) {
      for (int i = 1; i < 61; i++) {
        pulseBlue();
        delay(750);
        Serial.print(F("Warming up! "));
        Serial.print(i);
        Serial.println(F("/60"));
      }
      Serial.println(F("Warmup flag set to false!"));
      warmFlag = false;
    }
    logData(dataName, batteryPercentage);
    digitalWrite(blueLED, LOW);
  } else {
    warmFlag = true;
    Serial.println(F("Assuming sensor has been flipped off, turning the warm up flag on"));
    digitalWrite(blueLED, HIGH);
  }
  if (ret == 0) {
  logData(bdataName, batteryPercentage);
  }
  Serial.println(F("Loop finished, waiting until the next 15th second:"));
  delay(100);
}
