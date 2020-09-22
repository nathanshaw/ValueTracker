#include <ValueTracker.h>

double pin_val = 0.0;

elapsedMicros em; 

ValueTrackerDouble tracker = ValueTrackerDouble(&pin_val, 0.5);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A2, INPUT);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 500; i++) {
  pin_val = (double)analogRead(A2) / (double)1024.0;
  em = 0;
  tracker.update();
  Serial.print("time : ");
  Serial.println(em);
  tracker.printStats();
  delay(100);
  }
  tracker.reset();
}
