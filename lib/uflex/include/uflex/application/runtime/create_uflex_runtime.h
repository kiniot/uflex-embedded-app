//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_RUNTIME_CREATE_UFLEX_RUNTIME_H
#define UFLEX_APPLICATION_RUNTIME_CREATE_UFLEX_RUNTIME_H

#include "uflex/application/runtime/uflex_runtime.h"

/**
 * @file create_uflex_runtime.h
 * @brief Declares the runtime factory for the active uFlex build target.
 *
 * This factory centralizes compile-time runtime selection so the application
 * layer can work with a single UflexRuntime reference regardless of the
 * currently selected PlatformIO environment.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
UflexRuntime& createUflexRuntime();

#endif // UFLEX_APPLICATION_RUNTIME_CREATE_UFLEX_RUNTIME_H
