//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file active_context_parser.cpp
 * @brief Implements the hand-rolled active-context JSON parser.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/active_context_parser.h"

#include <stdlib.h>
#include <string.h>

namespace {

// Returns a pointer to the first non-space character of the value for `quotedKey`
// (e.g. "\"serie_id\""), or nullptr when the key is absent.
const char* valueAfterKey(const char* json, const char* quotedKey) {
    const char* keyPos = strstr(json, quotedKey);
    if (keyPos == nullptr) {
        return nullptr;
    }
    const char* colon = strchr(keyPos + strlen(quotedKey), ':');
    if (colon == nullptr) {
        return nullptr;
    }
    const char* value = colon + 1;
    while (*value == ' ') {
        ++value;
    }
    return value;
}

bool isJsonNull(const char* value) {
    return value == nullptr || strncmp(value, "null", 4) == 0;
}

// Copies a JSON string value (pointing at the opening quote) into dest, unquoted.
void copyJsonString(const char* value, char* dest, size_t destSize) {
    dest[0] = '\0';
    if (value == nullptr || *value != '"') {
        return;
    }
    ++value; // skip the opening quote
    size_t i = 0;
    while (value[i] != '\0' && value[i] != '"' && i + 1 < destSize) {
        dest[i] = value[i];
        ++i;
    }
    dest[i] = '\0';
}

ActiveJoint parseJoint(const char* value) {
    if (value == nullptr || *value != '"') {
        return ActiveJoint::None;
    }
    if (strncmp(value + 1, "ELBOW", 5) == 0) {
        return ActiveJoint::Elbow;
    }
    if (strncmp(value + 1, "WRIST", 5) == 0) {
        return ActiveJoint::Wrist;
    }
    return ActiveJoint::None;
}

ActiveMovement parseMovement(const char* value) {
    if (value == nullptr || *value != '"') {
        return ActiveMovement::None;
    }
    if (strncmp(value + 1, "FLEXION", 7) == 0) {
        return ActiveMovement::Flexion;
    }
    if (strncmp(value + 1, "EXTENSION", 9) == 0) {
        return ActiveMovement::Extension;
    }
    if (strncmp(value + 1, "PRONATION", 9) == 0) {
        return ActiveMovement::Pronation;
    }
    if (strncmp(value + 1, "SUPINATION", 10) == 0) {
        return ActiveMovement::Supination;
    }
    return ActiveMovement::None;
}

} // namespace

void parseActiveContext(const char* json, ActiveSerieContext& out) {
    out = ActiveSerieContext{};
    if (json == nullptr) {
        return;
    }

    out.activeJoint = parseJoint(valueAfterKey(json, "\"active_joint\""));
    out.activeMovement = parseMovement(valueAfterKey(json, "\"active_movement\""));

    const char* maxValue = valueAfterKey(json, "\"max_safe_angle\"");
    if (!isJsonNull(maxValue)) {
        out.maxSafeAngle = static_cast<float>(atof(maxValue));
        out.hasMaxSafeAngle = true;
    }

    copyJsonString(valueAfterKey(json, "\"serie_id\""), out.serieId, sizeof(out.serieId));
    out.hasContext = out.serieId[0] != '\0';
}
