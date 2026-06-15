#include <unity.h>

#include <EventHandler.h>

#include "test_cases.h"
#include "uflex/domain/sensors/imu.h"

namespace {

class RecordingEventHandler : public EventHandler {
public:
    int eventCount = 0;
    int lastEventId = -1;

    void on(Event event) override {
        ++eventCount;
        lastEventId = event.id;
    }
};

void assertSampleEquals(const ImuSample& expected, const ImuSample& actual) {
    TEST_ASSERT_EQUAL_INT16(expected.accelX, actual.accelX);
    TEST_ASSERT_EQUAL_INT16(expected.accelY, actual.accelY);
    TEST_ASSERT_EQUAL_INT16(expected.accelZ, actual.accelZ);
    TEST_ASSERT_EQUAL_INT16(expected.gyroX, actual.gyroX);
    TEST_ASSERT_EQUAL_INT16(expected.gyroY, actual.gyroY);
    TEST_ASSERT_EQUAL_INT16(expected.gyroZ, actual.gyroZ);
    TEST_ASSERT_EQUAL_INT16(expected.temperature, actual.temperature);
}

void testImuStartsWithConfiguredAddressAndZeroSample() {
    Imu imu(7, 0x69);
    const ImuSample zeroSample{0, 0, 0, 0, 0, 0, 0};

    TEST_ASSERT_EQUAL_HEX8(0x69, imu.getI2cAddress());
    assertSampleEquals(zeroSample, imu.getLastSample());
}

void testImuStoresLatestSampleAndEmitsMotionEvent() {
    RecordingEventHandler handler;
    Imu imu(3, 0x68, &handler);
    const ImuSample firstSample{10, -20, 30, -40, 50, -60, 70};
    const ImuSample secondSample{-100, 200, -300, 400, -500, 600, -700};

    imu.updateSample(firstSample);

    assertSampleEquals(firstSample, imu.getLastSample());
    TEST_ASSERT_EQUAL_INT(1, handler.eventCount);
    TEST_ASSERT_EQUAL_INT(Imu::MOTION_DETECTED_EVENT_ID, handler.lastEventId);

    imu.updateSample(secondSample);

    assertSampleEquals(secondSample, imu.getLastSample());
    TEST_ASSERT_EQUAL_INT(2, handler.eventCount);
    TEST_ASSERT_EQUAL_INT(Imu::MOTION_DETECTED_EVENT_ID, handler.lastEventId);
}

} // namespace

void runImuTests() {
    RUN_TEST(testImuStartsWithConfiguredAddressAndZeroSample);
    RUN_TEST(testImuStoresLatestSampleAndEmitsMotionEvent);
}
