//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file motion_payload_mapper.cpp
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/motion_payload_mapper.h"

MotionPayload MotionPayloadMapper::map(const MotionState& motionState) {
    return MotionPayload{
        motionState.upperMiddleAngle,
        motionState.middleLowerAngle,
        motionState.upperLowerAngle,
    };
}
