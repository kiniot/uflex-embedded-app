//
// Created by Salim Ramirez Mestanza on 28/06/26.
//

#ifndef UFLEX_APPLICATION_SAFETY_MONITOR_H
#define UFLEX_APPLICATION_SAFETY_MONITOR_H

#include "uflex/infrastructure/transport/active_serie_context.h"

/**
 * @file safety_monitor.h
 * @brief Decides whether the local safety actuators (buzzer + vibration) fire.
 *
 * Wraps the pure ceiling check (exceedsSafeAngle) with two guards and hysteresis:
 *  - arming: only acts with a live, max-safe-bearing context and a captured zero
 *    (calibrated). Without those, the angle is meaningless, so it stays disarmed.
 *  - hysteresis: once triggered, it stays on until the angle drops a margin below
 *    the ceiling, so sensor noise/drift at the boundary cannot chatter the buzzer.
 *
 * Stateful (holds the triggered/armed latch); fed one angle per loop cycle.
 *
 * @author Salim Ramirez
 * @date June 28, 2026
 * @version 0.1.0
 */
class SafetyMonitor {
public:
    /**
     * @brief Evaluates one cycle; returns whether the safety actuators should be ON.
     *
     * @param angleDegrees Current joint flexion angle (from the calibrated zero).
     * @param context Active serie context (carries hasMaxSafeAngle / maxSafeAngle).
     * @param calibrated Whether a valid session zero has been captured.
     * @param contextFresh Whether the context is live (not lost/expired by TTL).
     * @param hysteresisDegrees Margin below the ceiling required to clear a trigger.
     */
    bool evaluate(float angleDegrees, const ActiveSerieContext& context,
                  bool calibrated, bool contextFresh, float hysteresisDegrees);

    /** @brief Whether the safety reaction is currently latched ON. */
    bool isTriggered() const;

    /** @brief Whether the monitor is armed (guards satisfied) after the last evaluate. */
    bool isArmed() const;

private:
    bool triggered = false;
    bool armed = false;
};

#endif // UFLEX_APPLICATION_SAFETY_MONITOR_H
