//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H

#include "motion_payload.h"
#include "uflex/domain/devices/motion_state.h"

/**
 * @file motion_payload_mapper.h
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class MotionPayloadMapper {
public:
    static MotionPayload map(const MotionState& motionState);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H
