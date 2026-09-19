#ifndef CAT_GUARDIAN_DETERRENT_SINK_H
#define CAT_GUARDIAN_DETERRENT_SINK_H

#include "deterrent_request.h"

struct deterrent_sink {
    void *context;
    int (*submit)(
        void *context,
        const struct deterrent_request *request
    );
};

#endif
