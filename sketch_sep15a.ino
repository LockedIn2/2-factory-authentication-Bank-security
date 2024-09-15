#include <Servo.h>

const int redPin = 12;
const int greenPin = 13;
const int servoPin = 11;

Servo servo;

void setup() {
  Serial.begin(9600);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  servo.attach(servoPin); // Attaching servo
}

void loop() {
  if (Serial.available() > 0){
    String msg = Serial.readString();

    if (msg == "FAIYAZ"){
      digitalWrite(greenPin, HIGH);
      digitalWrite(redPin, LOW);
      servo.write(0);

    }
    else{
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, HIGH);
      servo.write(90);
    }
  }
}

