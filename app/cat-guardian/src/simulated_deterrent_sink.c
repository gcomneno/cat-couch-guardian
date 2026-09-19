#include "simulated_deterrent_sink.h"

int simulated_deterrent_submit(
    void *context,
    const struct deterrent_request *request
)
{
    struct simulated_deterrent_sink *sink = context;

    if (sink == 0 ||
        request == 0 ||
        request->trigger == 0 ||
        request->source == 0) {
        return -1;
    }

    sink->accepted_requests += 1;

    return 0;
}
