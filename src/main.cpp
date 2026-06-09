#include <Arduino.h>
#include "uflex/application/runtime/create_uflex_runtime.h"
#include "uflex/application/uflex_application.h"

namespace {

UflexApplication app(createUflexRuntime());

} // namespace

void setup() {
  Serial.begin(115200);
  delay(250);
  app.begin();
}

void loop() {
  app.loop();
}
