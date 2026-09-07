#ifndef CAT_GUARDIAN_EVIDENCE_SINK_H
#define CAT_GUARDIAN_EVIDENCE_SINK_H

#include "motion_event.h"

struct evidence_sink {
    void *context;
    int (*record_motion)(
        void *context,
        const struct motion_event *event
    );
};

#endif
