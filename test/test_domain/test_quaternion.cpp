#include <unity.h>

#include <math.h>

#include "test_cases.h"
#include "uflex/domain/math/quaternion.h"

namespace {

constexpr float kTolerance = 0.0001f;

void assertQuaternionEquals(const Quaternion& expected, const Quaternion& actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.w, actual.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.x, actual.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.y, actual.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.z, actual.z);
}

void testIdentityIsMultiplicativeIdentity() {
    const Quaternion q{0.5f, 0.5f, 0.5f, 0.5f};

    assertQuaternionEquals(q, multiply(Quaternion::identity(), q));
    assertQuaternionEquals(q, multiply(q, Quaternion::identity()));
}

void testConjugateNegatesVectorPartOnly() {
    const Quaternion q{0.5f, 1.0f, -2.0f, 3.0f};

    const Quaternion result = conjugate(q);

    assertQuaternionEquals(Quaternion{0.5f, -1.0f, 2.0f, -3.0f}, result);
}

void testMultiplyByConjugateProducesScalarOnlyResult() {
    const Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    const Quaternion result = multiply(q, conjugate(q));

    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 30.0f, result.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, result.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, result.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, result.z);
}

void testNormalizeRescalesToUnitMagnitude() {
    const Quaternion q{0.0f, 0.0f, 0.0f, 5.0f};

    const Quaternion result = normalize(q);

    assertQuaternionEquals(Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, result);
}

void testNormalizeOfNearZeroQuaternionReturnsIdentity() {
    const Quaternion q{0.0f, 0.0f, 0.0f, 0.0f};

    const Quaternion result = normalize(q);

    assertQuaternionEquals(Quaternion::identity(), result);
}

Quaternion rotationAboutX(float degrees) {
    const float halfRadians = degrees * 0.5f * 3.14159265f / 180.0f;
    return Quaternion{cosf(halfRadians), sinf(halfRadians), 0.0f, 0.0f};
}

Quaternion rotationAboutZ(float degrees) {
    const float halfRadians = degrees * 0.5f * 3.14159265f / 180.0f;
    return Quaternion{cosf(halfRadians), 0.0f, 0.0f, sinf(halfRadians)};
}

void testRotationAngleOfIdentityIsZero() {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, rotationAngleDegrees(Quaternion::identity()));
}

void testRotationAngleMatchesAxisAngle() {
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 60.0f, rotationAngleDegrees(rotationAboutX(60.0f)));
}

void testRotationAngleIgnoresDoubleCover() {
    const Quaternion q = rotationAboutX(60.0f);
    const Quaternion negated{-q.w, -q.x, -q.y, -q.z}; // -q is the same rotation
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 60.0f, rotationAngleDegrees(negated));
}

void testYawOfIdentityIsZero() {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 0.0f, yawDegrees(Quaternion::identity()));
}

void testYawMatchesZRotation() {
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.0f, yawDegrees(rotationAboutZ(30.0f)));
}

} // namespace

void runQuaternionTests() {
    RUN_TEST(testIdentityIsMultiplicativeIdentity);
    RUN_TEST(testConjugateNegatesVectorPartOnly);
    RUN_TEST(testMultiplyByConjugateProducesScalarOnlyResult);
    RUN_TEST(testNormalizeRescalesToUnitMagnitude);
    RUN_TEST(testNormalizeOfNearZeroQuaternionReturnsIdentity);
    RUN_TEST(testRotationAngleOfIdentityIsZero);
    RUN_TEST(testRotationAngleMatchesAxisAngle);
    RUN_TEST(testRotationAngleIgnoresDoubleCover);
    RUN_TEST(testYawOfIdentityIsZero);
    RUN_TEST(testYawMatchesZRotation);
}
