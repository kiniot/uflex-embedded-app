//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file quaternion.cpp
 * @brief Implements the shared quaternion math for the uFlex domain.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/domain/math/quaternion.h"

#include <math.h>

namespace {

constexpr float kMinNormalizableMagnitude = 1e-6f;

} // namespace

Quaternion Quaternion::identity() {
    return Quaternion{1.0f, 0.0f, 0.0f, 0.0f};
}

Quaternion multiply(const Quaternion& lhs, const Quaternion& rhs) {
    return Quaternion{
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
        lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
    };
}

Quaternion conjugate(const Quaternion& quaternion) {
    return Quaternion{quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z};
}

Quaternion normalize(const Quaternion& quaternion) {
    const float magnitude = sqrtf(quaternion.w * quaternion.w + quaternion.x * quaternion.x +
                                  quaternion.y * quaternion.y + quaternion.z * quaternion.z);

    if (magnitude < kMinNormalizableMagnitude) {
        return Quaternion::identity();
    }

    const float inverseMagnitude = 1.0f / magnitude;
    return Quaternion{
        quaternion.w * inverseMagnitude,
        quaternion.x * inverseMagnitude,
        quaternion.y * inverseMagnitude,
        quaternion.z * inverseMagnitude,
    };
}
