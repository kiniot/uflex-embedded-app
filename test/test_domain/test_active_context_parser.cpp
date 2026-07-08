#include <unity.h>

#include "test_cases.h"
#include "uflex/infrastructure/transport/active_context_parser.h"

namespace {

void testParsesActiveElbowContext() {
    ActiveSerieContext out;
    parseActiveContext(
        "{\"serial_number\":\"uflex-kit-001\",\"active_joint\":\"ELBOW\","
        "\"active_movement\":\"FLEXION\",\"max_safe_angle\":95.0,\"serie_id\":\"ser-1\"}",
        out);

    TEST_ASSERT_TRUE(out.hasContext);
    TEST_ASSERT_EQUAL(static_cast<int>(ActiveJoint::Elbow), static_cast<int>(out.activeJoint));
    TEST_ASSERT_EQUAL(static_cast<int>(ActiveMovement::Flexion), static_cast<int>(out.activeMovement));
    TEST_ASSERT_TRUE(out.hasMaxSafeAngle);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 95.0f, out.maxSafeAngle);
    TEST_ASSERT_EQUAL_STRING("ser-1", out.serieId);
}

void testParsesPronationMovement() {
    ActiveSerieContext out;
    parseActiveContext(
        "{\"active_joint\":\"WRIST\",\"active_movement\":\"PRONATION\","
        "\"max_safe_angle\":null,\"serie_id\":\"s1\"}",
        out);

    TEST_ASSERT_EQUAL(static_cast<int>(ActiveMovement::Pronation), static_cast<int>(out.activeMovement));
}

void testMissingMovementIsNone() {
    ActiveSerieContext out;
    // No active_movement key (older edge) -> None, so the firmware falls back to joint-only selection.
    parseActiveContext("{\"active_joint\":\"ELBOW\",\"serie_id\":\"s2\"}", out);

    TEST_ASSERT_EQUAL(static_cast<int>(ActiveMovement::None), static_cast<int>(out.activeMovement));
}

void testParsesWrist() {
    ActiveSerieContext out;
    parseActiveContext(
        "{\"active_joint\":\"WRIST\",\"max_safe_angle\":110.5,\"serie_id\":\"abc\"}", out);

    TEST_ASSERT_EQUAL(static_cast<int>(ActiveJoint::Wrist), static_cast<int>(out.activeJoint));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 110.5f, out.maxSafeAngle);
}

void testNullContextClears() {
    ActiveSerieContext out;
    parseActiveContext(
        "{\"serial_number\":\"uflex-kit-001\",\"active_joint\":null,"
        "\"max_safe_angle\":null,\"serie_id\":null}",
        out);

    TEST_ASSERT_FALSE(out.hasContext);
    TEST_ASSERT_EQUAL(static_cast<int>(ActiveJoint::None), static_cast<int>(out.activeJoint));
    TEST_ASSERT_FALSE(out.hasMaxSafeAngle);
    TEST_ASSERT_EQUAL_STRING("", out.serieId);
}

void testUnknownJointFallsBackToNoneButKeepsSerie() {
    ActiveSerieContext out;
    parseActiveContext(
        "{\"active_joint\":\"SHOULDER\",\"max_safe_angle\":null,\"serie_id\":\"s9\"}", out);

    TEST_ASSERT_EQUAL(static_cast<int>(ActiveJoint::None), static_cast<int>(out.activeJoint));
    TEST_ASSERT_TRUE(out.hasContext);
    TEST_ASSERT_EQUAL_STRING("s9", out.serieId);
}

} // namespace

void runActiveContextParserTests() {
    RUN_TEST(testParsesActiveElbowContext);
    RUN_TEST(testParsesWrist);
    RUN_TEST(testParsesPronationMovement);
    RUN_TEST(testMissingMovementIsNone);
    RUN_TEST(testNullContextClears);
    RUN_TEST(testUnknownJointFallsBackToNoneButKeepsSerie);
}
