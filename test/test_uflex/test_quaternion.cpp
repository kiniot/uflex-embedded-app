#include <unity.h>

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

} // namespace

void runQuaternionTests() {
    RUN_TEST(testIdentityIsMultiplicativeIdentity);
    RUN_TEST(testConjugateNegatesVectorPartOnly);
    RUN_TEST(testMultiplyByConjugateProducesScalarOnlyResult);
    RUN_TEST(testNormalizeRescalesToUnitMagnitude);
    RUN_TEST(testNormalizeOfNearZeroQuaternionReturnsIdentity);
}
