//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H

#include "motion_payload.h"
#include "uflex/domain/devices/motion_state.h"

/**
 * @file motion_payload_mapper.h
 * @brief Declares conversion from domain motion state to transport payload.
 *
 * MotionPayloadMapper isolates the mapping between the richer domain state and
 * the lighter transport contract so payload consumers do not depend directly on
 * every field stored by the domain device.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class MotionPayloadMapper {
public:
    /**
     * @brief Maps the current domain motion state to a transport payload.
     */
    static MotionPayload map(const MotionState& motionState);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_MAPPER_H
