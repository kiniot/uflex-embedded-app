#include <unity.h>

#include "test_cases.h"

namespace {

void runAllSuites() {
    runImuTests();
    runMotionStateTests();
    runRelativeAngleCalculatorTests();
    runUflexDeviceTests();
    runMotionPayloadTests();
    runQuaternionTests();
    runJointAngleCalculatorTests();
    runGyroBiasCalibratorTests();
    runOrientationFilterTests();
    runRelativeRotationCalculatorTests();
    runBleMotionTelemetryTests();
    runSampleBatchSerializerTests();
    runJointTargetingTests();
    runActiveContextParserTests();
}

} // namespace

#if defined(ARDUINO)

#include <Arduino.h>

void setup() {
    delay(2000);

    UNITY_BEGIN();
    runAllSuites();
    UNITY_END();
}

void loop() {}

#else

int main(int /*argc*/, char** /*argv*/) {
    UNITY_BEGIN();
    runAllSuites();
    return UNITY_END();
}

#endif
