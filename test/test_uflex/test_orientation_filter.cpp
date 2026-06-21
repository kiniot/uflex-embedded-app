#include <math.h>

#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/services/orientation_filter.h"

namespace {

constexpr float kTolerance = 0.01f;

struct Vec3 {
    float x;
    float y;
    float z;
};

Vec3 normalized(Vec3 v) {
    const float magnitude = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (magnitude < 1e-6f) {
        return Vec3{0.0f, 0.0f, 0.0f};
    }
    return Vec3{v.x / magnitude, v.y / magnitude, v.z / magnitude};
}

void assertVec3Equals(Vec3 expected, Vec3 actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.x, actual.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.y, actual.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.z, actual.z);
}

// At steady state (zero gyroscope input held long enough to converge), the
// filter's orientation estimate must be self-consistent with the measured
// accelerometer direction: rotating the world "up" vector into the body frame
// through the estimated orientation should reproduce the (normalized)
// accelerometer reading. This checks the filter's defining invariant without
// reaching into its internals, using only the public Quaternion math.
Vec3 estimatedUpInBodyFrame(const Quaternion& orientation) {
    const Quaternion worldUp{0.0f, 0.0f, 0.0f, 1.0f};
    const Quaternion estimate = multiply(multiply(conjugate(orientation), worldUp), orientation);
    return Vec3{estimate.x, estimate.y, estimate.z};
}

ImuSample stationarySample(int16_t accelX, int16_t accelY, int16_t accelZ, int16_t magX = 0,
                           int16_t magY = 0, int16_t magZ = 0) {
    return ImuSample{accelX, accelY, accelZ, 0, 0, 0, 0, magX, magY, magZ};
}

void convergeOn(OrientationFilter& filter, const ImuSample& sample, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        filter.update(sample, 0.01f);
    }
}

void testStartsAtIdentityOrientation() {
    OrientationFilter filter;

    const Quaternion orientation = filter.getOrientation();

    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 1.0f, orientation.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.z);
}

void testAlreadyLevelDeviceStaysAtIdentity() {
    OrientationFilter filter(20.0f);
    // accelZ-only matches the identity orientation's own gravity assumption
    // (see OrientationFilter docs), so no correction should be needed.
    const ImuSample level = stationarySample(0, 0, 1000);

    convergeOn(filter, level, 50);

    assertVec3Equals(Vec3{0.0f, 0.0f, 1.0f}, estimatedUpInBodyFrame(filter.getOrientation()));
}

void test6DofFallbackConvergesAccelDirectionWithoutMagnetometer() {
    OrientationFilter filter(20.0f);
    // Device tipped so gravity now reads along body +X; magnetometer left at
    // zero, forcing the 6-DOF (accel+gyro only) fallback path.
    const ImuSample tilted = stationarySample(1000, 0, 0);

    convergeOn(filter, tilted, 300);

    assertVec3Equals(normalized(Vec3{1.0f, 0.0f, 0.0f}),
                     estimatedUpInBodyFrame(filter.getOrientation()));
}

void test9DofPathConvergesAccelDirectionWithMagnetometerPresent() {
    OrientationFilter filter(20.0f);
    const ImuSample tiltedWithHeading = stationarySample(1000, 0, 0, 0, 1000, 0);

    convergeOn(filter, tiltedWithHeading, 300);

    const Quaternion finalOrientation = filter.getOrientation();
    TEST_ASSERT_TRUE(isfinite(finalOrientation.w));
    TEST_ASSERT_TRUE(isfinite(finalOrientation.x));
    TEST_ASSERT_TRUE(isfinite(finalOrientation.y));
    TEST_ASSERT_TRUE(isfinite(finalOrientation.z));
    assertVec3Equals(normalized(Vec3{1.0f, 0.0f, 0.0f}), estimatedUpInBodyFrame(finalOrientation));
}

void testAllZeroSampleDoesNotCorruptOrientation() {
    OrientationFilter filter;
    const ImuSample allZero = stationarySample(0, 0, 0);

    const Quaternion orientation = filter.update(allZero, 0.01f);

    TEST_ASSERT_TRUE(isfinite(orientation.w));
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 1.0f, orientation.w);
}

void testResetReturnsToIdentity() {
    OrientationFilter filter(20.0f);
    convergeOn(filter, stationarySample(1000, 0, 0), 100);

    filter.reset();

    const Quaternion orientation = filter.getOrientation();
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 1.0f, orientation.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, orientation.z);
}

} // namespace

void runOrientationFilterTests() {
    RUN_TEST(testStartsAtIdentityOrientation);
    RUN_TEST(testAlreadyLevelDeviceStaysAtIdentity);
    RUN_TEST(test6DofFallbackConvergesAccelDirectionWithoutMagnetometer);
    RUN_TEST(test9DofPathConvergesAccelDirectionWithMagnetometerPresent);
    RUN_TEST(testAllZeroSampleDoesNotCorruptOrientation);
    RUN_TEST(testResetReturnsToIdentity);
}
