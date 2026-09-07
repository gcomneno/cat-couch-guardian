#include <inttypes.h>
#include <stdio.h>

#include "cat_guardian_core.h"
#include "simulated_motion_source.h"

static int stdout_record_motion(
    void *context,
    const struct motion_event *event
)
{
    FILE *stream = context;

    if (stream == 0 || event == 0) {
        return -1;
    }

    if (fprintf(
            stream,
            "evidence type=motion source=%s sequence=%" PRIu64 "\n",
            event->source,
            event->sequence
        ) < 0) {
        return -1;
    }

    return 0;
}

static int forward_to_core(
    const struct motion_event *event,
    void *context
)
{
    const struct evidence_sink *sink = context;

    return cat_guardian_handle_motion(event, sink);
}

int main(void)
{
    const struct evidence_sink sink = {
        .context = stdout,
        .record_motion = stdout_record_motion,
    };

    return simulated_motion_source_emit(
        1,
        forward_to_core,
        (void *)&sink
    ) == 0 ? 0 : 1;
}
