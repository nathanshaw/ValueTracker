#include "ValueTrackerDouble.h"

double pin_val = 0.0;

ValueTrackerDouble tracker = ValueTrackerDouble(&pin_val);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(A2, INPUT);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  pin_val = analogRead(A2) / 1024;
  tracker.update();
  tracker.printStats();
  delay(100);
}
