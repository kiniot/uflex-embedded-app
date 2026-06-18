#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/services/relative_angle_calculator.h"

namespace {

constexpr float kAngleToleranceDegrees = 0.01f;

void testIdenticalSamplesProduceZeroRelativeAngle() {
    const ImuSample sample{1200, -300, 800, 10, 20, 30, 40};

    const RelativeAngle angle = RelativeAngleCalculator::calculate(sample, sample);

    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, angle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, angle.rollDegrees);
}

void testSecondSamplePitchIsMeasuredRelativeToFirst() {
    const ImuSample neutral{0, 0, 1000, 0, 0, 0, 0};
    const ImuSample positivePitch{1000, 0, 0, 0, 0, 0, 0};

    const RelativeAngle forward = RelativeAngleCalculator::calculate(neutral, positivePitch);
    const RelativeAngle reverse = RelativeAngleCalculator::calculate(positivePitch, neutral);

    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 90.0f, forward.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, forward.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, -90.0f, reverse.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, reverse.rollDegrees);
}

void testSecondSampleRollIsMeasuredRelativeToFirst() {
    const ImuSample neutral{0, 0, 1000, 0, 0, 0, 0};
    const ImuSample positiveRoll{0, 1000, 0, 0, 0, 0, 0};

    const RelativeAngle forward = RelativeAngleCalculator::calculate(neutral, positiveRoll);
    const RelativeAngle reverse = RelativeAngleCalculator::calculate(positiveRoll, neutral);

    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, forward.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 90.0f, forward.rollDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, reverse.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, -90.0f, reverse.rollDegrees);
}

void testGyroscopeAndTemperatureDoNotAffectRelativeAngle() {
    const ImuSample first{100, 200, 300, 1, 2, 3, 4};
    const ImuSample second{100, 200, 300, -1000, 2000, -3000, 4000};

    const RelativeAngle angle = RelativeAngleCalculator::calculate(first, second);

    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, angle.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kAngleToleranceDegrees, 0.0f, angle.rollDegrees);
}

} // namespace

void runRelativeAngleCalculatorTests() {
    RUN_TEST(testIdenticalSamplesProduceZeroRelativeAngle);
    RUN_TEST(testSecondSamplePitchIsMeasuredRelativeToFirst);
    RUN_TEST(testSecondSampleRollIsMeasuredRelativeToFirst);
    RUN_TEST(testGyroscopeAndTemperatureDoNotAffectRelativeAngle);
}
