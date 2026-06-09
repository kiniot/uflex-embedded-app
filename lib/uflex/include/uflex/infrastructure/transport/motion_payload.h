//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_H

#include "uflex/domain/services/relative_angle_calculator.h"

/**
 * @file motion_payload.h
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
