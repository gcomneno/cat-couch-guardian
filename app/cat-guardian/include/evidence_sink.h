#ifndef CAT_GUARDIAN_EVIDENCE_SINK_H
#define CAT_GUARDIAN_EVIDENCE_SINK_H

#include "deterrent_request.h"
#include "motion_suppression.h"

struct evidence_sink {
    void *context;
    int (*record_deterrent_request)(
        void *context,
        const struct deterrent_request *request
    );
    int (*record_motion_suppression)(
        void *context,
        const struct motion_suppression *suppression
    );
};

#endif
