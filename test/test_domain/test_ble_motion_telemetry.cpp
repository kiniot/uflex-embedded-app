#include <string.h>

#include <unity.h>

#include "test_cases.h"
#include "uflex/infrastructure/transport/ble_motion_telemetry_mapper.h"
#include "uflex/infrastructure/transport/ble_motion_telemetry_serializer.h"

namespace {

constexpr float kTolerance = 0.0001f;

void assertQuaternionEquals(const Quaternion& expected, const Quaternion& actual) {
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.w, actual.w);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.x, actual.x);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.y, actual.y);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, expected.z, actual.z);
}

float readFloatAt(const uint8_t* buffer, size_t offset) {
    float value;
    memcpy(&value, buffer + offset, sizeof(float));
    return value;
}

void testMapperCarriesRelativeRotationsAndActuatorStateAsGiven() {
    const MotionState state{
        {1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19, 20, 21},
        {1.25f, -2.5f},
        {3.75f, -4.0f},
        {5.5f, -6.25f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.7071f, 0.7071f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.5f, 0.5f},
    };

    const BleMotionTelemetry telemetry =
        BleMotionTelemetryMapper::map(state, RgbLed::Color::green, true, false, 42, 37.5f, true, 1);

    assertQuaternionEquals(state.upperMiddleRotation, telemetry.upperMiddleRotation);
    assertQuaternionEquals(state.middleLowerRotation, telemetry.middleLowerRotation);
    assertQuaternionEquals(state.upperLowerRotation, telemetry.upperLowerRotation);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(RgbLed::Color::green), telemetry.ledColor);
    TEST_ASSERT_TRUE(telemetry.buzzerActive);
    TEST_ASSERT_FALSE(telemetry.vibrationActive);
    TEST_ASSERT_EQUAL_UINT16(42, telemetry.sequenceNumber);
    TEST_ASSERT_FLOAT_WITHIN(kTolerance, 37.5f, telemetry.jointFlexionDegrees);
    TEST_ASSERT_TRUE(telemetry.isCalibrated);
    TEST_ASSERT_EQUAL_UINT8(1, telemetry.activeJoint);
}

void testSerializerWritesTheDocumentedFixedLayout() {
    const BleMotionTelemetry telemetry{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        static_cast<uint8_t>(RgbLed::Color::yellow),
        true,
        false,
        0x1234,
        90.5f,
        true,
        2,
    };
    uint8_t buffer[BleMotionTelemetrySerializer::WIRE_SIZE_BYTES] = {};

    const int written = BleMotionTelemetrySerializer::toBytes(telemetry, buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL_INT(static_cast<int>(BleMotionTelemetrySerializer::WIRE_SIZE_BYTES), written);

    TEST_ASSERT_EQUAL_FLOAT(1.0f, readFloatAt(buffer, 0));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, readFloatAt(buffer, 4));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, readFloatAt(buffer, 16));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, readFloatAt(buffer, 20));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, readFloatAt(buffer, 32));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, readFloatAt(buffer, 40));

    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(RgbLed::Color::yellow), buffer[48]);
    TEST_ASSERT_EQUAL_UINT8(1, buffer[49]);
    TEST_ASSERT_EQUAL_UINT8(0, buffer[50]);
    // Sequence number 0x1234 must land little-endian: low byte first.
    TEST_ASSERT_EQUAL_UINT8(0x34, buffer[51]);
    TEST_ASSERT_EQUAL_UINT8(0x12, buffer[52]);
    // Appended fields: calibrated flexion (float @53), isCalibrated (@57), activeJoint (@58).
    TEST_ASSERT_EQUAL_FLOAT(90.5f, readFloatAt(buffer, 53));
    TEST_ASSERT_EQUAL_UINT8(1, buffer[57]);
    TEST_ASSERT_EQUAL_UINT8(2, buffer[58]);
}

void testSerializerRejectsBufferSmallerThanWireSize() {
    const BleMotionTelemetry telemetry{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        0,
        false,
        false,
        0,
        0.0f,
        false,
        0,
    };
    uint8_t buffer[BleMotionTelemetrySerializer::WIRE_SIZE_BYTES - 1] = {};

    const int written = BleMotionTelemetrySerializer::toBytes(telemetry, buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL_INT(-1, written);
}

} // namespace

void runBleMotionTelemetryTests() {
    RUN_TEST(testMapperCarriesRelativeRotationsAndActuatorStateAsGiven);
    RUN_TEST(testSerializerWritesTheDocumentedFixedLayout);
    RUN_TEST(testSerializerRejectsBufferSmallerThanWireSize);
}
