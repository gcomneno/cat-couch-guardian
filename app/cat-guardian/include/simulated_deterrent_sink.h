#ifndef CAT_GUARDIAN_SIMULATED_DETERRENT_SINK_H
#define CAT_GUARDIAN_SIMULATED_DETERRENT_SINK_H

#include "deterrent_sink.h"

struct simulated_deterrent_sink {
    int accepted_requests;
};

int simulated_deterrent_submit(
    void *context,
    const struct deterrent_request *request
);

#endif
