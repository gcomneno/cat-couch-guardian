#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "cat_guardian_core.h"

struct capture {
    int calls;
    uint64_t sequence;
    const char *source;
};

static int capture_motion(
    void *context,
    const struct motion_event *event
)
{
    struct capture *capture = context;

    assert(capture != 0);
    assert(event != 0);

    capture->calls += 1;
    capture->sequence = event->sequence;
    capture->source = event->source;

    return 0;
}

int main(void)
{
    struct capture capture = {0};

    const struct evidence_sink sink = {
        .context = &capture,
        .record_motion = capture_motion,
    };

    const struct motion_event event = {
        .sequence = 42,
        .source = "unit-test",
    };

    assert(cat_guardian_handle_motion(&event, &sink) == 0);

    assert(capture.calls == 1);
    assert(capture.sequence == 42);
    assert(strcmp(capture.source, "unit-test") == 0);

    assert(cat_guardian_handle_motion(0, &sink) != 0);
    assert(cat_guardian_handle_motion(&event, 0) != 0);

    return 0;
}
