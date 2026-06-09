//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/runtime/create_uflex_runtime.h"

#include "config/build_config.h"
#include "uflex/application/runtime/hw_uflex_runtime.h"
#include "uflex/application/runtime/sim_uflex_runtime.h"

/**
 * @file create_uflex_runtime.cpp
 * @brief Implements compile-time runtime selection for uFlex.
 *
 * The factory returns a single static runtime instance chosen from the active
 * build-target macros so the rest of the application can stay agnostic of the
 * environment-specific wiring.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

UflexRuntime& createUflexRuntime() {
#if defined(UFLEX_TARGET_SIM)
    static SimUflexRuntime runtime;
    return runtime;
#endif

#if defined(UFLEX_TARGET_HW)
    static HwUflexRuntime runtime;
    return runtime;
#endif
}
