#include "cat_guardian_core.h"

int cat_guardian_state_init(
    struct cat_guardian_state *state,
    uint64_t cooldown_ms
)
{
    if (state == 0) {
        return -1;
    }

    state->cooldown_ms = cooldown_ms;
    state->last_deterrent_at_ms = 0;
    state->has_last_deterrent = 0;

    return 0;
}

int cat_guardian_handle_motion(
    struct cat_guardian_state *state,
    const struct motion_event *event,
    const struct deterrent_sink *deterrent,
    const struct evidence_sink *evidence
)
{
    struct deterrent_request request;
    struct motion_suppression suppression;
    int result;

    if (state == 0 ||
        event == 0 ||
        event->source == 0 ||
        deterrent == 0 ||
        deterrent->submit == 0 ||
        evidence == 0 ||
        evidence->record_deterrent_request == 0 ||
        evidence->record_motion_suppression == 0) {
        return -1;
    }

    if (state->has_last_deterrent != 0) {
        uint64_t elapsed_ms;

        if (event->observed_at_ms < state->last_deterrent_at_ms) {
            return -1;
        }

        elapsed_ms =
            event->observed_at_ms - state->last_deterrent_at_ms;

        if (elapsed_ms < state->cooldown_ms) {
            suppression.sequence = event->sequence;
            suppression.observed_at_ms = event->observed_at_ms;
            suppression.reason = "cooldown";
            suppression.source = event->source;

            return evidence->record_motion_suppression(
                evidence->context,
                &suppression
            );
        }
    }

    request.sequence = event->sequence;
    request.trigger = "motion";
    request.source = event->source;

    result = deterrent->submit(deterrent->context, &request);
    if (result != 0) {
        return result;
    }

    state->last_deterrent_at_ms = event->observed_at_ms;
    state->has_last_deterrent = 1;

    return evidence->record_deterrent_request(
        evidence->context,
        &request
    );
}
