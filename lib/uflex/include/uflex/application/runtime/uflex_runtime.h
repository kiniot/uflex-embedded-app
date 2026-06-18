//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_RUNTIME_UFLEX_RUNTIME_H
#define UFLEX_APPLICATION_RUNTIME_UFLEX_RUNTIME_H

#include "uflex/domain/devices/uflex_device.h"

/**
 * @file uflex_runtime.h
 * @brief Declares the application runtime abstraction for uFlex.
 *
 * UflexRuntime defines the small contract used by the application layer to
 * initialize the active environment, advance it during the firmware loop, and
 * expose the shared domain device independently of whether the target is
 * simulation or real hardware.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class UflexRuntime {
public:
    virtual bool begin() = 0;
    virtual bool update() = 0;
    virtual void applyOutputs() = 0;
    virtual UflexDevice& getDevice() = 0;
    virtual ~UflexRuntime() = default;
};

#endif // UFLEX_APPLICATION_RUNTIME_UFLEX_RUNTIME_H
