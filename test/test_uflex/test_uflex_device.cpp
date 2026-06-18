#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/devices/uflex_device.h"

namespace {

constexpr float kAngleToleranceDegrees = 0.01f;

void assertAngleEquals(const RelativeAngle& expected, const RelativeAngle& actual) {
    TEST_ASSERT_FLOAT_WITHIN(
        kAngleToleranceDegrees, expected.pitchDegrees, actual.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(
        kAngleToleranceDegrees, expected.rollDegrees, actual.rollDegrees);
}

void assertSampleEquals(const ImuSample& expected, const ImuSample& actual) {
    TEST_ASSERT_EQUAL_INT16(expected.accelX, actual.accelX);
    TEST_ASSERT_EQUAL_INT16(expected.accelY, actual.accelY);
    TEST_ASSERT_EQUAL_INT16(expected.accelZ, actual.accelZ);
    TEST_ASSERT_EQUAL_INT16(expected.gyroX, actual.gyroX);
    TEST_ASSERT_EQUAL_INT16(expected.gyroY, actual.gyroY);
    TEST_ASSERT_EQUAL_INT16(expected.gyroZ, actual.gyroZ);
    TEST_ASSERT_EQUAL_INT16(expected.temperature, actual.temperature);
}

void testDeviceExposesConfiguredImusAndZeroInitialState() {
    UflexDevice device({10, 0x68}, {11, 0x69}, {12, 0x6A});
    const MotionState state = device.getMotionState();

    TEST_ASSERT_EQUAL_HEX8(0x68, device.getUpperImu().getI2cAddress());
    TEST_ASSERT_EQUAL_HEX8(0x69, device.getMiddleImu().getI2cAddress());
    TEST_ASSERT_EQUAL_HEX8(0x6A, device.getLowerImu().getI2cAddress());
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.upperMiddleAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.upperMiddleAngle.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.middleLowerAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.middleLowerAngle.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.upperLowerAngle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, state.upperLowerAngle.rollDegrees);
}

void testDeviceRecalculatesAllAnglesWhenImuSamplesChange() {
    UflexDevice device({0, 0x68}, {1, 0x69}, {2, 0x68});
    const ImuSample upperSample{0, 0, 1000, 1, 2, 3, 4};
    const ImuSample middleSample{1000, 0, 0, 5, 6, 7, 8};
    const ImuSample lowerSample{0, 1000, 0, 9, 10, 11, 12};

    device.getUpperImu().updateSample(upperSample);
    device.getMiddleImu().updateSample(middleSample);
    device.getLowerImu().updateSample(lowerSample);

    const MotionState state = device.getMotionState();
    assertSampleEquals(upperSample, state.upperSample);
    assertSampleEquals(middleSample, state.middleSample);
    assertSampleEquals(lowerSample, state.lowerSample);
    assertAngleEquals(
        RelativeAngleCalculator::calculate(upperSample, middleSample),
        state.upperMiddleAngle);
    assertAngleEquals(
        RelativeAngleCalculator::calculate(middleSample, lowerSample),
        state.middleLowerAngle);
    assertAngleEquals(
        RelativeAngleCalculator::calculate(upperSample, lowerSample),
        state.upperLowerAngle);
    assertAngleEquals(state.upperMiddleAngle, device.getUpperMiddleAngle());
    assertAngleEquals(state.middleLowerAngle, device.getMiddleLowerAngle());
    assertAngleEquals(state.upperLowerAngle, device.getUpperLowerAngle());
}

void testUnrelatedEventDoesNotChangeCalculatedAngles() {
    UflexDevice device({0, 0x68}, {1, 0x69}, {2, 0x68});
    const ImuSample upperSample{0, 0, 1000, 0, 0, 0, 0};
    const ImuSample middleSample{1000, 0, 0, 0, 0, 0, 0};

    device.getUpperImu().updateSample(upperSample);
    device.getMiddleImu().updateSample(middleSample);
    const RelativeAngle before = device.getUpperMiddleAngle();

    device.on(Event(999));

    assertAngleEquals(before, device.getUpperMiddleAngle());
}

} // namespace

void runUflexDeviceTests() {
    RUN_TEST(testDeviceExposesConfiguredImusAndZeroInitialState);
    RUN_TEST(testDeviceRecalculatesAllAnglesWhenImuSamplesChange);
    RUN_TEST(testUnrelatedEventDoesNotChangeCalculatedAngles);
}
