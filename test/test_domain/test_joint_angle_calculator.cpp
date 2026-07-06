#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/services/joint_angle_calculator.h"

namespace {

constexpr float kTolerance = 0.01f;

RelativeAngle angle(float pitchDegrees, float rollDegrees = 0.0f) {
    return RelativeAngle{pitchDegrees, rollDegrees};
}

void testUncalibratedReturnsRawMagnitude() {
    JointAngleCalculator calculator;

    TEST_ASSERT_FALSE(calculator.isCalibrated());
    // Zero is {0, 0} until calibrated, so the flexion is the raw tilt magnitude.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 60.0f, calculator.absoluteFlexionDegrees(angle(60.0f)));
}

void testCalibratedZeroPoseReadsZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(20.0f));

    TEST_ASSERT_TRUE(calculator.isCalibrated());
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, calculator.absoluteFlexionDegrees(angle(20.0f)));
}

void testAngleMeasuredFromCalibratedZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(20.0f));

    // 50 deg pitch with a 20 deg zero -> 30 deg of flexion.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 30.0f, calculator.absoluteFlexionDegrees(angle(50.0f)));
}

void testFlexionIsDistanceInPitchRollSpace() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(10.0f, 10.0f));

    // Pitch +3, roll +4 from the zero -> sqrt(3^2 + 4^2) = 5, regardless of which axis the hinge is on.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 5.0f,
                             calculator.absoluteFlexionDegrees(angle(13.0f, 14.0f)));
}

void testFlexionIsCappedAt180() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(0.0f, 0.0f));

    // A joint cannot exceed 180 deg; a huge (out-of-range) input clamps to 180.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 180.0f, calculator.absoluteFlexionDegrees(angle(200.0f)));
}

void testReCalibrationMovesTheZero() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(20.0f));
    calculator.calibrate(angle(50.0f));

    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, calculator.absoluteFlexionDegrees(angle(50.0f)));
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 20.0f, calculator.absoluteFlexionDegrees(angle(70.0f)));
}

void testResetReturnsToUncalibrated() {
    JointAngleCalculator calculator;
    calculator.calibrate(angle(20.0f));

    calculator.reset();

    TEST_ASSERT_FALSE(calculator.isCalibrated());
    // Back to the {0, 0} zero -> raw magnitude again.
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 60.0f, calculator.absoluteFlexionDegrees(angle(60.0f)));
}

} // namespace

void runJointAngleCalculatorTests() {
    RUN_TEST(testUncalibratedReturnsRawMagnitude);
    RUN_TEST(testCalibratedZeroPoseReadsZero);
    RUN_TEST(testAngleMeasuredFromCalibratedZero);
    RUN_TEST(testFlexionIsDistanceInPitchRollSpace);
    RUN_TEST(testFlexionIsCappedAt180);
    RUN_TEST(testReCalibrationMovesTheZero);
    RUN_TEST(testResetReturnsToUncalibrated);
}
