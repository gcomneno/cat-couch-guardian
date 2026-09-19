#include "cat_guardian_core.h"

int cat_guardian_handle_motion(
    const struct motion_event *event,
    const struct deterrent_sink *deterrent,
    const struct evidence_sink *evidence
)
{
    struct deterrent_request request;
    int result;

    if (event == 0 ||
        event->source == 0 ||
        deterrent == 0 ||
        deterrent->submit == 0 ||
        evidence == 0 ||
        evidence->record_deterrent_request == 0) {
        return -1;
    }

    request.sequence = event->sequence;
    request.trigger = "motion";
    request.source = event->source;

    result = deterrent->submit(deterrent->context, &request);
    if (result != 0) {
        return result;
    }

    return evidence->record_deterrent_request(
        evidence->context,
        &request
    );
}
