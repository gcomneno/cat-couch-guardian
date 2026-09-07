#ifndef CAT_GUARDIAN_MOTION_SOURCE_H
#define CAT_GUARDIAN_MOTION_SOURCE_H

#include "motion_event.h"

typedef int (*motion_event_handler)(
    const struct motion_event *event,
    void *context
);

#endif
