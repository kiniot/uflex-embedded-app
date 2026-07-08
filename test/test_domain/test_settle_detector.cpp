#include <unity.h>

#include "test_cases.h"
#include "uflex/application/settle_detector.h"

namespace {

constexpr float kTolerance = 0.0001f;

ImuSample sampleWithGyro(int16_t gx, int16_t gy, int16_t gz) {
    ImuSample sample{};
    sample.gyroX = gx;
    sample.gyroY = gy;
    sample.gyroZ = gz;
    return sample;
}

void testGyroMagnitudeIsEuclidean() {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 5.0f, gyroMagnitude(sampleWithGyro(3, 4, 0)));
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, gyroMagnitude(sampleWithGyro(0, 0, 0)));
}

void testSettlesOnlyAfterRequiredStillCycles() {
    SettleDetector detector(10.0f, 3);

    TEST_ASSERT_FALSE(detector.update(5.0f));  // 1
    TEST_ASSERT_FALSE(detector.update(5.0f));  // 2
    TEST_ASSERT_TRUE(detector.update(5.0f));   // 3 -> settled
    TEST_ASSERT_TRUE(detector.settled());
}

void testThresholdIsInclusive() {
    SettleDetector detector(10.0f, 1);
    TEST_ASSERT_TRUE(detector.update(10.0f));  // at the threshold counts as still
}

void testMotionResetsTheStreak() {
    SettleDetector detector(10.0f, 3);
    detector.update(5.0f);
    detector.update(5.0f);
    TEST_ASSERT_FALSE(detector.update(50.0f));  // a spike above threshold resets the count
    TEST_ASSERT_FALSE(detector.update(5.0f));   // streak starts over
    TEST_ASSERT_FALSE(detector.update(5.0f));
    TEST_ASSERT_TRUE(detector.update(5.0f));
}

void testResetClearsTheStreak() {
    SettleDetector detector(10.0f, 2);
    detector.update(5.0f);
    detector.reset();
    TEST_ASSERT_FALSE(detector.update(5.0f));  // back to 1 after reset
    TEST_ASSERT_TRUE(detector.update(5.0f));
}

} // namespace

void runSettleDetectorTests() {
    RUN_TEST(testGyroMagnitudeIsEuclidean);
    RUN_TEST(testSettlesOnlyAfterRequiredStillCycles);
    RUN_TEST(testThresholdIsInclusive);
    RUN_TEST(testMotionResetsTheStreak);
    RUN_TEST(testResetClearsTheStreak);
}
