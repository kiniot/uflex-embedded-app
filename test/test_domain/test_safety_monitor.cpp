#include <unity.h>

#include "test_cases.h"
#include "uflex/application/safety_monitor.h"

namespace {

constexpr float kHysteresis = 5.0f;

ActiveSerieContext armedContext(float maxSafe) {
    ActiveSerieContext context;
    context.hasContext = true;
    context.hasMaxSafeAngle = true;
    context.maxSafeAngle = maxSafe;
    return context;
}

void testDisarmedWithoutCalibration() {
    SafetyMonitor monitor;
    TEST_ASSERT_FALSE(monitor.evaluate(200.0f, armedContext(95.0f), false, true, kHysteresis));
    TEST_ASSERT_FALSE(monitor.isArmed());
}

void testDisarmedWhenContextNotFresh() {
    SafetyMonitor monitor;
    TEST_ASSERT_FALSE(monitor.evaluate(200.0f, armedContext(95.0f), true, false, kHysteresis));
}

void testDisarmedWithoutContextOrCeiling() {
    SafetyMonitor monitor;
    ActiveSerieContext noContext;  // defaults: hasContext = false
    TEST_ASSERT_FALSE(monitor.evaluate(200.0f, noContext, true, true, kHysteresis));

    ActiveSerieContext noCeiling;
    noCeiling.hasContext = true;  // but hasMaxSafeAngle stays false
    TEST_ASSERT_FALSE(monitor.evaluate(200.0f, noCeiling, true, true, kHysteresis));
}

void testTriggersAtOrAboveCeilingWhenArmed() {
    SafetyMonitor monitor;
    const ActiveSerieContext context = armedContext(95.0f);
    TEST_ASSERT_FALSE(monitor.evaluate(94.0f, context, true, true, kHysteresis));
    TEST_ASSERT_TRUE(monitor.evaluate(95.0f, context, true, true, kHysteresis));
    TEST_ASSERT_TRUE(monitor.isArmed());
}

void testHysteresisHoldsUntilMarginBelowCeiling() {
    SafetyMonitor monitor;
    const ActiveSerieContext context = armedContext(95.0f);
    TEST_ASSERT_TRUE(monitor.evaluate(95.0f, context, true, true, kHysteresis));  // trigger
    TEST_ASSERT_TRUE(monitor.evaluate(91.0f, context, true, true, kHysteresis));  // 91 > 90 -> hold
    TEST_ASSERT_FALSE(monitor.evaluate(90.0f, context, true, true, kHysteresis)); // 90 not > 90 -> clear
}

void testReArmsAfterClearOnlyAtCeiling() {
    SafetyMonitor monitor;
    const ActiveSerieContext context = armedContext(95.0f);
    monitor.evaluate(95.0f, context, true, true, kHysteresis);  // trigger
    monitor.evaluate(90.0f, context, true, true, kHysteresis);  // clear
    TEST_ASSERT_FALSE(monitor.evaluate(94.0f, context, true, true, kHysteresis));  // below ceiling
    TEST_ASSERT_TRUE(monitor.evaluate(95.0f, context, true, true, kHysteresis));   // re-trigger
}

void testDisarmDropsLatchNoStickyTrigger() {
    SafetyMonitor monitor;
    const ActiveSerieContext context = armedContext(95.0f);
    TEST_ASSERT_TRUE(monitor.evaluate(100.0f, context, true, true, kHysteresis));  // triggered
    TEST_ASSERT_FALSE(monitor.evaluate(100.0f, context, true, false, kHysteresis)); // context lost -> off
    // Back fresh but in the hysteresis band: must NOT resurrect the old trigger.
    TEST_ASSERT_FALSE(monitor.evaluate(92.0f, context, true, true, kHysteresis));
}

} // namespace

void runSafetyMonitorTests() {
    RUN_TEST(testDisarmedWithoutCalibration);
    RUN_TEST(testDisarmedWhenContextNotFresh);
    RUN_TEST(testDisarmedWithoutContextOrCeiling);
    RUN_TEST(testTriggersAtOrAboveCeilingWhenArmed);
    RUN_TEST(testHysteresisHoldsUntilMarginBelowCeiling);
    RUN_TEST(testReArmsAfterClearOnlyAtCeiling);
    RUN_TEST(testDisarmDropsLatchNoStickyTrigger);
}
