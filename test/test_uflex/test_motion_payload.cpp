#include <string.h>

#include <unity.h>

#include "test_cases.h"
#include "uflex/infrastructure/transport/motion_payload_mapper.h"
#include "uflex/infrastructure/transport/motion_payload_serializer.h"

namespace {

constexpr float kAngleToleranceDegrees = 0.001f;

void assertAngleEquals(const RelativeAngle& expected, const RelativeAngle& actual) {
    TEST_ASSERT_FLOAT_WITHIN(
        kAngleToleranceDegrees, expected.pitchDegrees, actual.pitchDegrees);
    TEST_ASSERT_FLOAT_WITHIN(
        kAngleToleranceDegrees, expected.rollDegrees, actual.rollDegrees);
}

void testMapperCopiesOnlyDerivedAnglesFromMotionState() {
    const MotionState state{
        {1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19, 20, 21},
        {1.25f, -2.5f},
        {3.75f, -4.0f},
        {5.5f, -6.25f},
    };

    const MotionPayload payload = MotionPayloadMapper::map(state);

    assertAngleEquals(state.upperMiddleAngle, payload.upperMiddleAngle);
    assertAngleEquals(state.middleLowerAngle, payload.middleLowerAngle);
    assertAngleEquals(state.upperLowerAngle, payload.upperLowerAngle);
}

void testSerializerProducesExpectedCompactJson() {
    const MotionPayload payload{
        {1.25f, -2.5f},
        {3.75f, -4.0f},
        {5.5f, -6.25f},
    };
    char buffer[256] = {};
    const char* expected =
        "{\"upperMiddle\":{\"pitchDeg\":1.25,\"rollDeg\":-2.50},"
        "\"middleLower\":{\"pitchDeg\":3.75,\"rollDeg\":-4.00},"
        "\"upperLower\":{\"pitchDeg\":5.50,\"rollDeg\":-6.25}}";

    const int written = MotionPayloadSerializer::toJson(payload, buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL_INT(static_cast<int>(strlen(expected)), written);
    TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void testSerializerRejectsBufferWithoutSpaceForCompleteJson() {
    const MotionPayload payload{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
    };
    char buffer[32] = {};

    const int written = MotionPayloadSerializer::toJson(payload, buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL_INT(-1, written);
}

} // namespace

void runMotionPayloadTests() {
    RUN_TEST(testMapperCopiesOnlyDerivedAnglesFromMotionState);
    RUN_TEST(testSerializerProducesExpectedCompactJson);
    RUN_TEST(testSerializerRejectsBufferWithoutSpaceForCompleteJson);
}
