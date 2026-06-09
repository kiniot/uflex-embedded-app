#ifndef UFLEX_CONFIG_BUILD_CONFIG_H
#define UFLEX_CONFIG_BUILD_CONFIG_H

/**
 * @file build_config.h
 * @brief Declares build-target configuration macros for uFlex.
 *
 * This header centralizes the compile-time target selection used across the project so the
 * application can adapt its integrations for simulation or real hardware while keeping a shared
 * core domain model.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#if defined(UFLEX_TARGET_SIM) && defined(UFLEX_TARGET_HW)
#error "UFLEX_TARGET_SIM and UFLEX_TARGET_HW cannot be enabled at the same time."
#endif

#if !defined(UFLEX_TARGET_SIM) && !defined(UFLEX_TARGET_HW)
#error "A uFlex build target must be defined."
#endif

#if defined(UFLEX_TARGET_SIM)
#define UFLEX_BUILD_TARGET_NAME "simulation"
#endif

#if defined(UFLEX_TARGET_HW)
#define UFLEX_BUILD_TARGET_NAME "hardware"
#endif

#endif // UFLEX_CONFIG_BUILD_CONFIG_H
