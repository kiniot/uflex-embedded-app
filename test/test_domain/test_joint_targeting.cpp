#include <unity.h>

#include "test_cases.h"
#include "uflex/application/joint_targeting.h"

namespace {

constexpr float kTolerance = 0.0001f;

MotionState stateWithRotations(const Quaternion& upperMiddle, const Quaternion& middleLower) {
    MotionState state{};
    state.upperMiddleRotation = upperMiddle;
    state.middleLowerRotation = middleLower;
    return state;
}

void assertQuaternionEquals(const Quaternion& expected, const Quaternion& actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.w, actual.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.x, actual.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.y, actual.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.z, actual.z);
}

void testElbowSelectsUpperMiddle() {
    const Quaternion um{1.0f, 0.1f, 0.0f, 0.0f};
    const Quaternion ml{0.0f, 0.0f, 1.0f, 0.0f};

    assertQuaternionEquals(um, activeJointRotation(stateWithRotations(um, ml), ActiveJoint::Elbow));
}

void testWristSelectsMiddleLower() {
    const Quaternion um{1.0f, 0.1f, 0.0f, 0.0f};
    const Quaternion ml{0.0f, 0.0f, 1.0f, 0.0f};

    assertQuaternionEquals(ml, activeJointRotation(stateWithRotations(um, ml), ActiveJoint::Wrist));
}

void testNoneDefaultsToUpperMiddle() {
    const Quaternion um{1.0f, 0.1f, 0.0f, 0.0f};
    const Quaternion ml{0.0f, 0.0f, 1.0f, 0.0f};

    assertQuaternionEquals(um, activeJointRotation(stateWithRotations(um, ml), ActiveJoint::None));
}

MotionState stateWithAngles(const RelativeAngle& upperMiddle, const RelativeAngle& middleLower) {
    MotionState state{};
    state.upperMiddleAngle = upperMiddle;
    state.middleLowerAngle = middleLower;
    return state;
}

void assertAngleEquals(const RelativeAngle& expected, const RelativeAngle& actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.pitchDegrees, actual.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.rollDegrees, actual.rollDegrees);
}

void testElbowSelectsUpperMiddleAngle() {
    const RelativeAngle um{10.0f, 2.0f};
    const RelativeAngle ml{40.0f, 5.0f};
    assertAngleEquals(um, activeJointAngle(stateWithAngles(um, ml), ActiveJoint::Elbow));
}

void testWristSelectsMiddleLowerAngle() {
    const RelativeAngle um{10.0f, 2.0f};
    const RelativeAngle ml{40.0f, 5.0f};
    assertAngleEquals(ml, activeJointAngle(stateWithAngles(um, ml), ActiveJoint::Wrist));
}

void testNoneDefaultsToUpperMiddleAngle() {
    const RelativeAngle um{10.0f, 2.0f};
    const RelativeAngle ml{40.0f, 5.0f};
    assertAngleEquals(um, activeJointAngle(stateWithAngles(um, ml), ActiveJoint::None));
}

void testSafetyFalseWithoutContext() {
    ActiveSerieContext context; // defaults: no context
    TEST_ASSERT_FALSE(exceedsSafeAngle(200.0f, context));
}

void testSafetyFalseWithoutMaxSafeAngle() {
    ActiveSerieContext context;
    context.hasContext = true;
    TEST_ASSERT_FALSE(exceedsSafeAngle(200.0f, context));
}

void testSafetyFalseBelowCeiling() {
    ActiveSerieContext context;
    context.hasContext = true;
    context.hasMaxSafeAngle = true;
    context.maxSafeAngle = 95.0f;
    TEST_ASSERT_FALSE(exceedsSafeAngle(94.99f, context));
}

void testSafetyTrueAtOrAboveCeiling() {
    ActiveSerieContext context;
    context.hasContext = true;
    context.hasMaxSafeAngle = true;
    context.maxSafeAngle = 95.0f;
    TEST_ASSERT_TRUE(exceedsSafeAngle(95.0f, context));
    TEST_ASSERT_TRUE(exceedsSafeAngle(120.0f, context));
}

} // namespace

void runJointTargetingTests() {
    RUN_TEST(testElbowSelectsUpperMiddle);
    RUN_TEST(testWristSelectsMiddleLower);
    RUN_TEST(testNoneDefaultsToUpperMiddle);
    RUN_TEST(testElbowSelectsUpperMiddleAngle);
    RUN_TEST(testWristSelectsMiddleLowerAngle);
    RUN_TEST(testNoneDefaultsToUpperMiddleAngle);
    RUN_TEST(testSafetyFalseWithoutContext);
    RUN_TEST(testSafetyFalseWithoutMaxSafeAngle);
    RUN_TEST(testSafetyFalseBelowCeiling);
    RUN_TEST(testSafetyTrueAtOrAboveCeiling);
}
