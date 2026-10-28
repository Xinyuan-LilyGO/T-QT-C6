#include <Arduino.h>

#include "AppController.h"

namespace {
app::AppController application;
bool applicationReady = false;
}  // namespace

void setup() {
  applicationReady = application.begin();
  if (!applicationReady) {
    Serial0.println("Fatal initialization error");
  }
}

void loop() {
  if (applicationReady) {
    application.loop();
  } else {
    delay(1000);
  }
}
