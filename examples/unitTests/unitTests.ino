#include "DataLog.h"
#include <ArduinoUnit.h>
#include "UnitTests.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Testing Start");
}

void loop() {
  Test::run();
}