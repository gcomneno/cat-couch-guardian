#include <inttypes.h>
#include <stdio.h>

#include "cat_guardian_core.h"
#include "simulated_deterrent_sink.h"
#include "simulated_motion_source.h"

struct application_context {
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

static int forward_to_core(
    const struct motion_event *event,
    void *context
)
{
    const struct application_context *application = context;

    if (application == 0 ||
        application->deterrent == 0 ||
        application->evidence == 0) {
        return -1;
    }

    return cat_guardian_handle_motion(
        event,
        application->deterrent,
        application->evidence
    );
}

int main(void)
{
    struct simulated_deterrent_sink simulated_deterrent = {0};

    const struct deterrent_sink deterrent = {
        .context = &simulated_deterrent,
        .submit = simulated_deterrent_submit,
    };

    const struct evidence_sink evidence = {
        .context = stdout,
        .record_deterrent_request = stdout_record_deterrent_request,
    };

    const struct application_context application = {
        .deterrent = &deterrent,
        .evidence = &evidence,
    };

    return simulated_motion_source_emit(
        1,
        forward_to_core,
        (void *)&application
    ) == 0 ? 0 : 1;
}
