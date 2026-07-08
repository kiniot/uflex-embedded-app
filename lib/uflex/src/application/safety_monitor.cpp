//
// Created by Salim Ramirez Mestanza on 28/06/26.
//

/**
 * @file safety_monitor.cpp
 * @brief Implements the armed + hysteresis safety decision.
 *
 * @author Salim Ramirez
 * @date June 28, 2026
 * @version 0.1.0
 */

#include "uflex/application/safety_monitor.h"

#include "uflex/application/joint_targeting.h"

bool SafetyMonitor::evaluate(float angleDegrees, const ActiveSerieContext& context,
                             bool calibrated, bool contextFresh, float hysteresisDegrees) {
    armed = context.hasContext && context.hasMaxSafeAngle && calibrated && contextFresh;
    if (!armed) {
        triggered = false;
        return false;
    }

    if (triggered) {
        // Stay latched until the angle drops a margin below the ceiling (anti-chatter).
        triggered = angleDegrees > (context.maxSafeAngle - hysteresisDegrees);
    } else {
        // Rising edge: reuse the pure ceiling check (single source of truth).
        triggered = exceedsSafeAngle(angleDegrees, context);
    }
    return triggered;
}

bool SafetyMonitor::isTriggered() const {
    return triggered;
}

bool SafetyMonitor::isArmed() const {
    return armed;
}
