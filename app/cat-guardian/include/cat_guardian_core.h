#ifndef CAT_GUARDIAN_CORE_H
#define CAT_GUARDIAN_CORE_H

#include "deterrent_sink.h"
#include "evidence_sink.h"
#include "motion_event.h"

int cat_guardian_handle_motion(
    const struct motion_event *event,
    const struct deterrent_sink *deterrent,
    const struct evidence_sink *evidence
);

#endif
