//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_H

#include "uflex/domain/services/relative_angle_calculator.h"

/**
 * @file motion_payload.h
 * @brief Declares the transport-oriented motion payload for uFlex.
 *
 * MotionPayload contains the subset of processed motion data currently exposed
 * to transport mechanisms such as serial output, BLE, or HTTP. It intentionally
 * stays smaller than the full MotionState by focusing on derived angles.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct MotionPayload {
    RelativeAngle upperMiddleAngle;
    RelativeAngle middleLowerAngle;
    RelativeAngle upperLowerAngle;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_H
