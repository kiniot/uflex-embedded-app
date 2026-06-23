//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_CONTEXT_PARSER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_CONTEXT_PARSER_H

#include "uflex/infrastructure/transport/active_serie_context.h"

/**
 * @file active_context_parser.h
 * @brief Parses the edge's active-context JSON into an ActiveSerieContext.
 *
 * Hand-rolled (the firmware pulls in no JSON library) and Arduino-free so it is
 * host-unit-testable. Tolerant of null fields: a missing/null serie clears the
 * context; a null max_safe_angle leaves local safety disabled.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
void parseActiveContext(const char* json, ActiveSerieContext& out);

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_ACTIVE_CONTEXT_PARSER_H
