#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "cat_guardian_core.h"

struct test_state {
    int deterrent_calls;
    int evidence_calls;
    int suppression_calls;
    int order;
    int deterrent_order;
    int evidence_order;
    int suppression_order;
    int deterrent_result;
    int evidence_result;
    int suppression_result;
    uint64_t deterrent_sequence;
    uint64_t evidence_sequence;
    uint64_t suppression_sequence;
    uint64_t suppression_observed_at_ms;
    const char *deterrent_trigger;
    const char *evidence_trigger;
    const char *deterrent_source;
    const char *evidence_source;
    const char *suppression_reason;
    const char *suppression_source;
};

static int capture_deterrent(
    void *context,
    const struct deterrent_request *request
)
{
    struct test_state *state = context;

    assert(state != 0);
    assert(request != 0);

    state->deterrent_calls += 1;
    state->order += 1;
    state->deterrent_order = state->order;
    state->deterrent_sequence = request->sequence;
    state->deterrent_trigger = request->trigger;
    state->deterrent_source = request->source;

    return state->deterrent_result;
}

static int capture_evidence(
    void *context,
    const struct deterrent_request *request
)
{
    struct test_state *state = context;

    assert(state != 0);
    assert(request != 0);

    state->evidence_calls += 1;
    state->order += 1;
    state->evidence_order = state->order;
    state->evidence_sequence = request->sequence;
    state->evidence_trigger = request->trigger;
    state->evidence_source = request->source;

    return state->evidence_result;
}

static int capture_suppression(
    void *context,
    const struct motion_suppression *suppression
)
{
    struct test_state *state = context;

    assert(state != 0);
    assert(suppression != 0);

    state->suppression_calls += 1;
    state->order += 1;
    state->suppression_order = state->order;
    state->suppression_sequence = suppression->sequence;
    state->suppression_observed_at_ms = suppression->observed_at_ms;
    state->suppression_reason = suppression->reason;
    state->suppression_source = suppression->source;

    return state->suppression_result;
}

static struct deterrent_sink deterrent_sink_for(
    struct test_state *state
)
{
    const struct deterrent_sink sink = {
        .context = state,
        .submit = capture_deterrent,
    };

    return sink;
}

static struct evidence_sink evidence_sink_for(
    struct test_state *state
)
{
    const struct evidence_sink sink = {
        .context = state,
        .record_deterrent_request = capture_evidence,
        .record_motion_suppression = capture_suppression,
    };

    return sink;
}

static struct motion_event event_at(
    uint64_t sequence,
    uint64_t observed_at_ms
)
{
    const struct motion_event event = {
        .sequence = sequence,
        .observed_at_ms = observed_at_ms,
        .source = "unit-test",
    };

    return event;
}

static void test_state_initialization(void)
{
    struct cat_guardian_state state = {
        .cooldown_ms = 99,
        .last_deterrent_at_ms = 99,
        .has_last_deterrent = 1,
    };

    assert(cat_guardian_state_init(&state, 5000) == 0);
    assert(state.cooldown_ms == 5000);
    assert(state.last_deterrent_at_ms == 0);
    assert(state.has_last_deterrent == 0);
    assert(cat_guardian_state_init(0, 5000) != 0);
}

static void test_first_event_is_allowed(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event event = event_at(42, 1000);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &deterrent,
        &evidence
    ) == 0);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 1);
    assert(capture.suppression_calls == 0);
    assert(capture.deterrent_order == 1);
    assert(capture.evidence_order == 2);
    assert(capture.deterrent_sequence == 42);
    assert(capture.evidence_sequence == 42);
    assert(strcmp(capture.deterrent_trigger, "motion") == 0);
    assert(strcmp(capture.evidence_trigger, "motion") == 0);
    assert(strcmp(capture.deterrent_source, "unit-test") == 0);
    assert(strcmp(capture.evidence_source, "unit-test") == 0);
    assert(core_state.has_last_deterrent == 1);
    assert(core_state.last_deterrent_at_ms == 1000);
}

static void test_event_inside_cooldown_is_suppressed(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(1, 1000);
    const struct motion_event second = event_at(2, 5999);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 0);

    capture.order = 0;
    capture.deterrent_order = 0;
    capture.evidence_order = 0;
    capture.suppression_order = 0;

    assert(cat_guardian_handle_motion(
        &core_state,
        &second,
        &deterrent,
        &evidence
    ) == 0);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 1);
    assert(capture.suppression_calls == 1);
    assert(capture.suppression_order == 1);
    assert(capture.suppression_sequence == 2);
    assert(capture.suppression_observed_at_ms == 5999);
    assert(strcmp(capture.suppression_reason, "cooldown") == 0);
    assert(strcmp(capture.suppression_source, "unit-test") == 0);
    assert(core_state.last_deterrent_at_ms == 1000);
}

static void test_exact_boundary_is_allowed(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(1, 1000);
    const struct motion_event boundary = event_at(2, 6000);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &boundary,
        &deterrent,
        &evidence
    ) == 0);

    assert(capture.deterrent_calls == 2);
    assert(capture.evidence_calls == 2);
    assert(capture.suppression_calls == 0);
    assert(core_state.last_deterrent_at_ms == 6000);
}

static void test_event_after_boundary_is_allowed(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(1, 1000);
    const struct motion_event later = event_at(2, 6001);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &later,
        &deterrent,
        &evidence
    ) == 0);

    assert(capture.deterrent_calls == 2);
    assert(capture.evidence_calls == 2);
    assert(capture.suppression_calls == 0);
    assert(core_state.last_deterrent_at_ms == 6001);
}

static void test_time_regression_is_rejected(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(1, 1000);
    const struct motion_event regressed = event_at(2, 999);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 0);

    assert(cat_guardian_handle_motion(
        &core_state,
        &regressed,
        &deterrent,
        &evidence
    ) != 0);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 1);
    assert(capture.suppression_calls == 0);
    assert(core_state.last_deterrent_at_ms == 1000);
}

static void test_invalid_inputs(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct deterrent_sink invalid_deterrent = {
        .context = &capture,
        .submit = 0,
    };
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct evidence_sink invalid_success_evidence = {
        .context = &capture,
        .record_deterrent_request = 0,
        .record_motion_suppression = capture_suppression,
    };
    const struct evidence_sink invalid_suppression_evidence = {
        .context = &capture,
        .record_deterrent_request = capture_evidence,
        .record_motion_suppression = 0,
    };
    const struct motion_event event = event_at(42, 1000);
    const struct motion_event invalid_event = {
        .sequence = 42,
        .observed_at_ms = 1000,
        .source = 0,
    };

    assert(cat_guardian_state_init(&core_state, 5000) == 0);

    assert(cat_guardian_handle_motion(
        0,
        &event,
        &deterrent,
        &evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        0,
        &deterrent,
        &evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &invalid_event,
        &deterrent,
        &evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        0,
        &evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &invalid_deterrent,
        &evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &deterrent,
        0
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &deterrent,
        &invalid_success_evidence
    ) != 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &deterrent,
        &invalid_suppression_evidence
    ) != 0);

    assert(capture.deterrent_calls == 0);
    assert(capture.evidence_calls == 0);
    assert(capture.suppression_calls == 0);
}

static void test_deterrent_failure_does_not_start_cooldown(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {
        .deterrent_result = 17,
    };
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event event = event_at(7, 1000);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &event,
        &deterrent,
        &evidence
    ) == 17);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 0);
    assert(capture.suppression_calls == 0);
    assert(core_state.has_last_deterrent == 0);
}

static void test_success_evidence_failure_keeps_cooldown_state(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {
        .evidence_result = 23,
    };
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(9, 1000);
    const struct motion_event second = event_at(10, 1001);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 23);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 1);
    assert(core_state.has_last_deterrent == 1);
    assert(core_state.last_deterrent_at_ms == 1000);

    capture.evidence_result = 0;

    assert(cat_guardian_handle_motion(
        &core_state,
        &second,
        &deterrent,
        &evidence
    ) == 0);

    assert(capture.deterrent_calls == 1);
    assert(capture.suppression_calls == 1);
}

static void test_suppression_evidence_failure_is_propagated(void)
{
    struct cat_guardian_state core_state;
    struct test_state capture = {0};
    const struct deterrent_sink deterrent = deterrent_sink_for(&capture);
    const struct evidence_sink evidence = evidence_sink_for(&capture);
    const struct motion_event first = event_at(1, 1000);
    const struct motion_event second = event_at(2, 1001);

    assert(cat_guardian_state_init(&core_state, 5000) == 0);
    assert(cat_guardian_handle_motion(
        &core_state,
        &first,
        &deterrent,
        &evidence
    ) == 0);

    capture.suppression_result = 31;

    assert(cat_guardian_handle_motion(
        &core_state,
        &second,
        &deterrent,
        &evidence
    ) == 31);

    assert(capture.deterrent_calls == 1);
    assert(capture.evidence_calls == 1);
    assert(capture.suppression_calls == 1);
    assert(core_state.last_deterrent_at_ms == 1000);
}

int main(void)
{
    test_state_initialization();
    test_first_event_is_allowed();
    test_event_inside_cooldown_is_suppressed();
    test_exact_boundary_is_allowed();
    test_event_after_boundary_is_allowed();
    test_time_regression_is_rejected();
    test_invalid_inputs();
    test_deterrent_failure_does_not_start_cooldown();
    test_success_evidence_failure_keeps_cooldown_state();
    test_suppression_evidence_failure_is_propagated();

    return 0;
}
