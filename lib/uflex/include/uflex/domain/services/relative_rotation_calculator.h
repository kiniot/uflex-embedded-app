//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_RELATIVE_ROTATION_CALCULATOR_H
#define UFLEX_DOMAIN_SERVICES_RELATIVE_ROTATION_CALCULATOR_H

#include "uflex/domain/math/quaternion.h"

/**
 * @file relative_rotation_calculator.h
 * @brief Declares the relative joint rotation calculation for the uFlex domain.
 *
 * RelativeRotationCalculator expresses one segment's absolute orientation in
 * terms of another's, producing the rotation a joint applies between two
 * adjacent body segments. Unlike RelativeAngleCalculator (pitch/roll deltas
 * derived from raw accelerometer samples), this operates on the full absolute
 * orientation quaternions produced by OrientationFilter, so it captures
 * rotation around every axis, including the one a single accelerometer cannot
 * observe.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class RelativeRotationCalculator {
public:
    /**
     * @brief Returns the rotation that takes `first`'s orientation to `second`'s,
     * expressed in `first`'s local frame.
     */
    static Quaternion calculate(const Quaternion& first, const Quaternion& second);
};

#endif // UFLEX_DOMAIN_SERVICES_RELATIVE_ROTATION_CALCULATOR_H
