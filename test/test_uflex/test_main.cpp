#include <Arduino.h>
#include <unity.h>

#include "test_cases.h"

void setup() {
    delay(2000);

    UNITY_BEGIN();
    runImuTests();
    runMotionStateTests();
    runRelativeAngleCalculatorTests();
    runUflexDeviceTests();
    runMotionPayloadTests();
    UNITY_END();
}

void loop() {}
