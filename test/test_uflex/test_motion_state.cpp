#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/devices/motion_state.h"

namespace {

void testMotionStateKeepsSamplesAndRelativeAngles() {
    const MotionState state{
        {1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19, 20, 21},
        {22.5f, -23.5f},
        {24.5f, -25.5f},
        {26.5f, -27.5f},
    };

    TEST_ASSERT_EQUAL_INT16(1, state.upperSample.accelX);
    TEST_ASSERT_EQUAL_INT16(7, state.upperSample.temperature);
    TEST_ASSERT_EQUAL_INT16(9, state.middleSample.accelY);
    TEST_ASSERT_EQUAL_INT16(13, state.middleSample.gyroZ);
    TEST_ASSERT_EQUAL_INT16(17, state.lowerSample.accelZ);
    TEST_ASSERT_EQUAL_INT16(21, state.lowerSample.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 22.5f, state.upperMiddleAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -23.5f, state.upperMiddleAngle.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 24.5f, state.middleLowerAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -25.5f, state.middleLowerAngle.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 26.5f, state.upperLowerAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -27.5f, state.upperLowerAngle.rollDegrees);
}

} // namespace

void runMotionStateTests() {
    RUN_TEST(testMotionStateKeepsSamplesAndRelativeAngles);
}
