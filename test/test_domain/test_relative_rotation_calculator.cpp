#include <math.h>

#include <unity.h>

#include "test_cases.h"
#include "uflex/domain/services/relative_rotation_calculator.h"

namespace {

constexpr float kTolerance = 0.001f;

void assertQuaternionEquals(const Quaternion& expected, const Quaternion& actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.w, actual.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.x, actual.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.y, actual.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.z, actual.z);
}

// Builds the quaternion for a rotation of `degrees` around the X axis.
Quaternion rotationAroundX(float degrees) {
    const float halfAngleRadians = (degrees * static_cast<float>(M_PI) / 180.0f) / 2.0f;
    return Quaternion{cosf(halfAngleRadians), sinf(halfAngleRadians), 0.0f, 0.0f};
}

void testIdenticalOrientationsProduceIdentityRotation() {
    const Quaternion orientation = rotationAroundX(35.0f);

    const Quaternion relative = RelativeRotationCalculator::calculate(orientation, orientation);

    assertQuaternionEquals(Quaternion::identity(), relative);
}

void testRelativeToIdentityFirstReturnsSecondUnchanged() {
    const Quaternion second = rotationAroundX(42.0f);

    const Quaternion relative = RelativeRotationCalculator::calculate(Quaternion::identity(), second);

    assertQuaternionEquals(second, relative);
}

void testIdentityAsSecondReturnsConjugateOfFirst() {
    const Quaternion first = rotationAroundX(42.0f);

    const Quaternion relative = RelativeRotationCalculator::calculate(first, Quaternion::identity());

    assertQuaternionEquals(conjugate(first), relative);
}

void testRelativeRotationIsTheAngleDifferenceBetweenTwoJointSegments() {
    const Quaternion upperSegment = rotationAroundX(30.0f);
    const Quaternion middleSegment = rotationAroundX(50.0f);

    const Quaternion relative =
        RelativeRotationCalculator::calculate(upperSegment, middleSegment);

    assertQuaternionEquals(rotationAroundX(20.0f), relative);
}

} // namespace

void runRelativeRotationCalculatorTests() {
    RUN_TEST(testIdenticalOrientationsProduceIdentityRotation);
    RUN_TEST(testRelativeToIdentityFirstReturnsSecondUnchanged);
    RUN_TEST(testIdentityAsSecondReturnsConjugateOfFirst);
    RUN_TEST(testRelativeRotationIsTheAngleDifferenceBetweenTwoJointSegments);
}
