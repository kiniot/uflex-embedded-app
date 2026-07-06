//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_SERIE_CONTEXT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_SERIE_CONTEXT_H

/**
 * @file active_serie_context.h
 * @brief Down-channel payload: the active serie context the edge pushes to the kit.
 *
 * Symmetric to the up-channel MotionPayload. The firmware maps `activeJoint` +
 * `activeMovement` to an IMU pair, applies `maxSafeAngle` for local safety, and
 * re-zeros the joint angle when `serieId` changes (the guided-calibration handshake).
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
enum class ActiveJoint : unsigned char {
    None,
    Elbow,
    Wrist,
};

// The serie's movement. Needed on top of the joint because pronation/supination is forearm axial
// rotation (the hand rotates with the forearm), so it must be measured against the upper arm
// (upper-middle), not the hand (middle-lower). Flexion/extension use the joint's default pair.
enum class ActiveMovement : unsigned char {
    None,
    Flexion,
    Extension,
    Pronation,
    Supination,
};

struct ActiveSerieContext {
    ActiveJoint activeJoint = ActiveJoint::None;
    ActiveMovement activeMovement = ActiveMovement::None;
    float maxSafeAngle = 0.0f;
    bool hasMaxSafeAngle = false;
    char serieId[32] = {};
    bool hasContext = false;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_SERIE_CONTEXT_H
