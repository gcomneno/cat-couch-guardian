#include "simulated_motion_source.h"

int simulated_motion_source_emit(
    uint64_t sequence,
    motion_event_handler handler,
    void *context
)
{
    const struct motion_event event = {
        .sequence = sequence,
        .source = "simulated",
    };

    if (handler == 0) {
        return -1;
    }

    return handler(&event, context);
}
