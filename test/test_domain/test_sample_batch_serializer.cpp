#include <unity.h>

#include "test_cases.h"
#include "uflex/infrastructure/transport/sample_batch_serializer.h"

namespace {

void testSerializesSingleSampleBatch() {
    const JointSample samples[] = {{47.32f, 12.07f}};
    const SampleBatchPayload payload{"uflex-kit-001", samples, 1};
    char buffer[256] = {};

    const int written = SampleBatchSerializer::toJson(payload, buffer, sizeof(buffer));

    TEST_ASSERT_GREATER_THAN(0, written);
    TEST_ASSERT_EQUAL_STRING(
        "{\"serial_number\":\"uflex-kit-001\",\"samples\":["
        "{\"target_angle\":47.32,\"proximal_signal\":12.07}]}",
        buffer);
}

void testSerializesMultiSampleBatch() {
    const JointSample samples[] = {{10.00f, 1.00f}, {20.00f, 2.00f}};
    const SampleBatchPayload payload{"kit", samples, 2};
    char buffer[256] = {};

    const int written = SampleBatchSerializer::toJson(payload, buffer, sizeof(buffer));

    TEST_ASSERT_GREATER_THAN(0, written);
    TEST_ASSERT_EQUAL_STRING(
        "{\"serial_number\":\"kit\",\"samples\":["
        "{\"target_angle\":10.00,\"proximal_signal\":1.00},"
        "{\"target_angle\":20.00,\"proximal_signal\":2.00}]}",
        buffer);
}

void testReturnsNegativeWhenBufferTooSmall() {
    const JointSample samples[] = {{47.32f, 12.07f}};
    const SampleBatchPayload payload{"uflex-kit-001", samples, 1};
    char buffer[16] = {};

    TEST_ASSERT_EQUAL_INT(-1, SampleBatchSerializer::toJson(payload, buffer, sizeof(buffer)));
}

} // namespace

void runSampleBatchSerializerTests() {
    RUN_TEST(testSerializesSingleSampleBatch);
    RUN_TEST(testSerializesMultiSampleBatch);
    RUN_TEST(testReturnsNegativeWhenBufferTooSmall);
}
