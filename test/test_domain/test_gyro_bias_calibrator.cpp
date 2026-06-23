#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/services/gyro_bias_calibrator.h"

namespace {

void feed(GyroBiasCalibrator& calibrator, int16_t x, int16_t y, int16_t z, uint16_t count) {
    for (uint16_t i = 0; i < count; ++i) {
        calibrator.addSample(x, y, z);
    }
}

void testMeanBiasOfConstantOffset() {
    GyroBiasCalibrator calibrator(100);
    feed(calibrator, 5, -3, 2, 100);

    TEST_ASSERT_TRUE(calibrator.isComplete());
    int16_t bx, by, bz;
    calibrator.getBias(bx, by, bz);
    TEST_ASSERT_EQUAL_INT16(5, bx);
    TEST_ASSERT_EQUAL_INT16(-3, by);
    TEST_ASSERT_EQUAL_INT16(2, bz);
}

void testAveragesNoisySamples() {
    GyroBiasCalibrator calibrator(4);
    calibrator.addSample(8, 0, 0);
    calibrator.addSample(12, 0, 0);
    calibrator.addSample(9, 0, 0);
    calibrator.addSample(11, 0, 0); // mean 10

    int16_t bx, by, bz;
    calibrator.getBias(bx, by, bz);
    TEST_ASSERT_EQUAL_INT16(10, bx);
}

void testNotCompleteBeforeTarget() {
    GyroBiasCalibrator calibrator(3);
    TEST_ASSERT_FALSE(calibrator.addSample(1, 1, 1));
    TEST_ASSERT_FALSE(calibrator.addSample(1, 1, 1));
    TEST_ASSERT_FALSE(calibrator.isComplete());
    TEST_ASSERT_TRUE(calibrator.addSample(1, 1, 1)); // third reaches the target
    TEST_ASSERT_TRUE(calibrator.isComplete());
}

void testZeroBiasWhenNoSamples() {
    GyroBiasCalibrator calibrator(10);
    int16_t bx = 9, by = 9, bz = 9;
    calibrator.getBias(bx, by, bz);
    TEST_ASSERT_EQUAL_INT16(0, bx);
    TEST_ASSERT_EQUAL_INT16(0, by);
    TEST_ASSERT_EQUAL_INT16(0, bz);
}

void testResetClearsAccumulation() {
    GyroBiasCalibrator calibrator(2);
    feed(calibrator, 50, 50, 50, 2);
    calibrator.reset();

    TEST_ASSERT_FALSE(calibrator.isComplete());
    feed(calibrator, 7, 7, 7, 2);
    int16_t bx, by, bz;
    calibrator.getBias(bx, by, bz);
    TEST_ASSERT_EQUAL_INT16(7, bx); // not blended with the pre-reset 50s
}

void testNoOverflowAtMaxMagnitude() {
    GyroBiasCalibrator calibrator(100);
    feed(calibrator, 32767, -32768, 32767, 100);

    int16_t bx, by, bz;
    calibrator.getBias(bx, by, bz);
    TEST_ASSERT_EQUAL_INT16(32767, bx);
    TEST_ASSERT_EQUAL_INT16(-32768, by);
    TEST_ASSERT_EQUAL_INT16(32767, bz);
}

} // namespace

void runGyroBiasCalibratorTests() {
    RUN_TEST(testMeanBiasOfConstantOffset);
    RUN_TEST(testAveragesNoisySamples);
    RUN_TEST(testNotCompleteBeforeTarget);
    RUN_TEST(testZeroBiasWhenNoSamples);
    RUN_TEST(testResetClearsAccumulation);
    RUN_TEST(testNoOverflowAtMaxMagnitude);
}
