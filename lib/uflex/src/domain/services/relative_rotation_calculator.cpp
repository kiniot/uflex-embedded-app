//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file relative_rotation_calculator.cpp
 * @brief Implements the relative joint rotation calculation for uFlex.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/relative_rotation_calculator.h"

Quaternion RelativeRotationCalculator::calculate(const Quaternion& first,
                                                  const Quaternion& second) {
    return multiply(conjugate(first), second);
}
