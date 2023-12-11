/*
  Scan

  This example scans for Bluetooth® Low Energy peripherals and prints out their advertising details:
  address, local name, advertised service UUID's.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <LiquidCrystal_I2C.h>

bool disconnected = false;
bool isScanning = false;
const int redPin = 17;
const int greenPin = 18;
const int bluePin = 19;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set RGB pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // set lcd screen
  lcd.begin();  
  lcd.backlight();


  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    lcd.print("Starting Bluetoo");
    lcd.setCursor(0,1);
    lcd.print("th failed");  
    delay(200); 
    while (1);
  }

  lcd.clear();
  lcd.print("Waiting Bluetoot");
  lcd.setCursor(0,1);
  lcd.print("h connection"); 
  delay(2000);
  Serial.println("Bluetooth® Low Energy Central - Peripheral Explorer");
  // start scanning for peripherals
  BLE.scan();
}

void loop() {
  if (!BLE.connected() && !isScanning) {
    //setup();
    Serial.println("Connection to BLE failed!");
    BLE.scan();
    lcd.clear();
    lcd.print("Waiting Bluetoot");
    lcd.setCursor(0,1);
    lcd.print("h connection"); 
    isScanning = true;
  }
  else {
    BLEDevice peripheral = BLE.available();

    if (peripheral) {
      Serial.print("Found ");
      Serial.print(peripheral.address());
      Serial.print(" '");
      Serial.print(peripheral.localName());
      Serial.print("' ");
      Serial.print(peripheral.advertisedServiceUuid());
      Serial.println();

      if (peripheral.localName() == "SpeechData") {
        BLE.stopScan();
        peripheral.connect();
        Serial.println("Connected peripheral");
        lcd.clear();
        lcd.print("Bluetooth connec");
        lcd.setCursor(0,1);
        lcd.print("ted successfully");
        disconnected = true;
        isScanning = false;
      }

      while (peripheral.connected()) {
        peripheral.discoverAttributes();
        BLEService service = peripheral.service("d36d58de-dd08-41ff-9b86-5afd37baea97");
        BLECharacteristic characteristic = service.characteristic("cc3b0b57-5f84-49a0-acc6-e002fde01d80");
        readCharacteristicValue(characteristic);
        delay(200);  // Adjust the delay as needed
      }
    }
    if (disconnected) {
      lcd.clear();
      lcd.print("Bluetooth connec");
      lcd.setCursor(0,1);
      lcd.print("tion lost");
      disconnected = false;
      delay(4000);
    }
  }
}

void readCharacteristicValue(BLECharacteristic characteristic) {
    if (characteristic.canRead()) {
      // Read the characteristic value
      characteristic.read();
      int value = characteristic.value()[0];

      // Control RGB LEDs based on the received value
      if (value == 2) {
        // Turn on Red LED
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
      } 
      else if (value == 5) {
        // Turn off all LEDs
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, LOW);
      } 
      else if (value == 4) {
        // Turn off all LEDs
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, HIGH);
      }
      else if (value == 0) {
        // Turn off all LEDs
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
      }
    }
}
