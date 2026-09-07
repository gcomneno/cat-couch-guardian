#ifndef CAT_GUARDIAN_MOTION_EVENT_H
#define CAT_GUARDIAN_MOTION_EVENT_H

#include <stdint.h>

struct motion_event {
    uint64_t sequence;
    const char *source;
};

#endif
