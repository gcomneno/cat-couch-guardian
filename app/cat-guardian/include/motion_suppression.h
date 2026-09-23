#ifndef CAT_GUARDIAN_MOTION_SUPPRESSION_H
#define CAT_GUARDIAN_MOTION_SUPPRESSION_H

#include <stdint.h>

struct motion_suppression {
    uint64_t sequence;
    uint64_t observed_at_ms;
    const char *reason;
    const char *source;
};

#endif
