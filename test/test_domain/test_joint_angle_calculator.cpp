#include <unity.h>

#include <math.h>

#include "test_cases.h"
#include "uflex/domain/services/joint_angle_calculator.h"

namespace {

constexpr float kTolerance = 0.01f;

Quaternion rotationAboutX(float degrees) {
    const float halfRadians = degrees * 0.5f * 3.14159265f / 180.0f;
    return Quaternion{cosf(halfRadians), sinf(halfRadians), 0.0f, 0.0f};
}

void testUncalibratedReturnsRawMagnitude() {
    JointAngleCalculator calculator;

    TEST_ASSERT_FALSE(calculator.isCalibrated());
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 60.0f, calculator.absoluteFlexionDegrees(rotationAboutX(60.0f)));
}

void testCalibratedZeroPoseReadsZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(rotationAboutX(20.0f));

    TEST_ASSERT_TRUE(calculator.isCalibrated());
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, calculator.absoluteFlexionDegrees(rotationAboutX(20.0f)));
}

void testAngleMeasuredFromCalibratedZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(rotationAboutX(20.0f));

    // 50 deg pose with a 20 deg zero -> 30 deg of flexion.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 30.0f, calculator.absoluteFlexionDegrees(rotationAboutX(50.0f)));
}

void testReCalibrationMovesTheZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(rotationAboutX(20.0f));
    calculator.calibrate(rotationAboutX(50.0f));

    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, calculator.absoluteFlexionDegrees(rotationAboutX(50.0f)));
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 20.0f, calculator.absoluteFlexionDegrees(rotationAboutX(70.0f)));
}

void testResetReturnsToUncalibrated() {
    JointAngleCalculator calculator;
    calculator.calibrate(rotationAboutX(20.0f));

    calculator.reset();

    TEST_ASSERT_FALSE(calculator.isCalibrated());
    // Back to the identity zero -> raw magnitude again.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 60.0f, calculator.absoluteFlexionDegrees(rotationAboutX(60.0f)));
}

} // namespace

void runJointAngleCalculatorTests() {
    RUN_TEST(testUncalibratedReturnsRawMagnitude);
    RUN_TEST(testCalibratedZeroPoseReadsZero);
    RUN_TEST(testAngleMeasuredFromCalibratedZero);
    RUN_TEST(testReCalibrationMovesTheZero);
    RUN_TEST(testResetReturnsToUncalibrated);
}
