#ifndef CAT_GUARDIAN_CORE_H
#define CAT_GUARDIAN_CORE_H

#include "evidence_sink.h"
#include "motion_event.h"

int cat_guardian_handle_motion(
    const struct motion_event *event,
    const struct evidence_sink *sink
);

#endif
