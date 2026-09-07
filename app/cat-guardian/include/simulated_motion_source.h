#ifndef CAT_GUARDIAN_SIMULATED_MOTION_SOURCE_H
#define CAT_GUARDIAN_SIMULATED_MOTION_SOURCE_H

#include <stdint.h>

#include "motion_source.h"

int simulated_motion_source_emit(
    uint64_t sequence,
    motion_event_handler handler,
    void *context
);

#endif
