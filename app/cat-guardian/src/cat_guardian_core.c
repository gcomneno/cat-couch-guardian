#include "cat_guardian_core.h"

int cat_guardian_handle_motion(
    const struct motion_event *event,
    const struct evidence_sink *sink
)
{
    if (event == 0 ||
        event->source == 0 ||
        sink == 0 ||
        sink->record_motion == 0) {
        return -1;
    }

    return sink->record_motion(sink->context, event);
}
