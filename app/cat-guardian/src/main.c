#include <inttypes.h>
#include <stdio.h>

#include "cat_guardian_core.h"
#include "simulated_deterrent_sink.h"
#include "simulated_motion_source.h"

struct application_context {
    struct cat_guardian_state *state;
    const struct deterrent_sink *deterrent;
    const struct evidence_sink *evidence;
};

static int stdout_record_deterrent_request(
    void *context,
    const struct deterrent_request *request
)
{
    FILE *stream = context;

    if (stream == 0 ||
        request == 0 ||
        request->trigger == 0 ||
        request->source == 0) {
        return -1;
    }

    if (fprintf(
            stream,
            "evidence type=deterrent-request trigger=%s source=%s sequence=%" PRIu64 "\n",
            request->trigger,
            request->source,
            request->sequence
        ) < 0) {
        return -1;
    }

    return 0;
}

static int stdout_record_motion_suppression(
    void *context,
    const struct motion_suppression *suppression
)
{
    FILE *stream = context;

    if (stream == 0 ||
        suppression == 0 ||
        suppression->reason == 0 ||
        suppression->source == 0) {
        return -1;
    }

    if (fprintf(
            stream,
            "evidence type=motion-suppressed reason=%s source=%s sequence=%" PRIu64 " observed-at-ms=%" PRIu64 "\n",
            suppression->reason,
            suppression->source,
            suppression->sequence,
            suppression->observed_at_ms
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
    const struct application_context *application = context;

    if (application == 0 ||
        application->state == 0 ||
        application->deterrent == 0 ||
        application->evidence == 0) {
        return -1;
    }

    return cat_guardian_handle_motion(
        application->state,
        event,
        application->deterrent,
        application->evidence
    );
}

int main(void)
{
    struct cat_guardian_state state;
    struct simulated_deterrent_sink simulated_deterrent = {0};

    const struct deterrent_sink deterrent = {
        .context = &simulated_deterrent,
        .submit = simulated_deterrent_submit,
    };

    const struct evidence_sink evidence = {
        .context = stdout,
        .record_deterrent_request = stdout_record_deterrent_request,
        .record_motion_suppression = stdout_record_motion_suppression,
    };

    const struct application_context application = {
        .state = &state,
        .deterrent = &deterrent,
        .evidence = &evidence,
    };

    if (cat_guardian_state_init(&state, 5000) != 0) {
        return 1;
    }

    if (simulated_motion_source_emit(
            1,
            1000,
            forward_to_core,
            (void *)&application
        ) != 0) {
        return 1;
    }

    if (simulated_motion_source_emit(
            2,
            1001,
            forward_to_core,
            (void *)&application
        ) != 0) {
        return 1;
    }

    return simulated_motion_source_emit(
        3,
        6000,
        forward_to_core,
        (void *)&application
    ) == 0 ? 0 : 1;
}
