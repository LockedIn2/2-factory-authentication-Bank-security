#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         8          // Configurable, see typical pin layout above
#define SS_PIN          9         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Initializing servo
Servo servo;

// Access card UID
byte accessUID[4] = {0x27, 0xB6, 0x7D, 0xC9};

// Door status: Open/Close
int open = 1;   

// Id recognition
int id_recognised = 1;

// The time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

// The time when the sensor outputs a low impulse
long unsigned int lowIn;         

// The amount of milliseconds the sensor has to be low 
// before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

const int pirPin = 2;    // The digital pin connected to the PIR sensor's output
const int RFIDGreenLED = 3;
const int RFIDRedLED = 4;
const int ledPin = 7;   // The digital pin connected to the LED
const int buzzerPin = 24; // The digital pin connected to the passive buzzer's positive terminal
const int servoPin = 5;  // Digital pin connected to servo motor

/////////////////////////////
// SETUP
void setup() {
  Serial.begin(9600);
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
  

  servo.attach(servoPin); // Attaching servo
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);  // Set buzzer pin as output
  pinMode(RFIDGreenLED, OUTPUT);    // RFID LED's
  pinMode(RFIDRedLED, OUTPUT);
  digitalWrite(pirPin, LOW);

  // Give the sensor some time to calibrate
  Serial.print("Calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

/////////////////////////////
// LOOP
void loop() {

  // Servo
  if (open == 1)      
    servo.write(90);
  else servo.write(0);    // Turn servo if door is open
// Motion  Tracker
  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin, HIGH);    // Turn on the LED if motion is detected

    if (lockLow) {  
      // Makes sure we wait for a transition to LOW before any further output is made
      lockLow = false;            
      Serial.println("---");
      Serial.print("Motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec"); 
      delay(50);
    }         
    takeLowTime = true;
  } 
  else {
    digitalWrite(ledPin, LOW);     // Turn off the LED if no motion is detected

    if (takeLowTime) {
      lowIn = millis();           // Save the time of the transition from HIGH to LOW
      takeLowTime = false;        // Ensure this is only done at the start of a LOW phase
    }
    
    // If the sensor is LOW for more than the given pause, assume no more motion
    if (!lockLow && millis() - lowIn > pause) {  
      lockLow = true;                        
      Serial.print("Motion ended at ");      // Output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      delay(50);
    }
  }
    

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Check info about the card; PICC_HaltA() is automatically called
	if (mfrc522.uid.uidByte[0] == accessUID[0] && mfrc522.uid.uidByte[1] == accessUID[1] && mfrc522.uid.uidByte[2] == accessUID[2] && mfrc522.uid.uidByte[3] == accessUID[3]){
    Serial.println("Access Granted");
    digitalWrite(RFIDGreenLED, HIGH);
    tone(buzzerPin, 2000);      // 2kHz tone
    delay(200);
    noTone(buzzerPin);
    delay(1800);
    digitalWrite(RFIDGreenLED, LOW);
  }
  else{
    Serial.println("Access Denied");
    digitalWrite(RFIDRedLED, HIGH);
    for (int i = 0; i < 3; i++) {
      tone(buzzerPin, 2000);      // 2kHz tone
      delay(200);                 // 200ms on
      noTone(buzzerPin);          // Turn off the buzzer
      delay(200);                 // 200ms off
    }
    delay(800);
    digitalWrite(RFIDRedLED, LOW);
    

  }
  mfrc522.PICC_HaltA();
}