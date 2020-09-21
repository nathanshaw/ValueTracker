#include "ValueTrackerDouble.h"

double pin_val = 0.0;

ValueTrackerDouble tracker(*pin_val);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(A2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  pin_val = analogRead(A2) / 1024;
}
