#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "cat_guardian_core.h"

struct test_state {
    int deterrent_calls;
    int evidence_calls;
    int order;
    int deterrent_order;
    int evidence_order;
    int deterrent_result;
    int evidence_result;
    uint64_t deterrent_sequence;
    uint64_t evidence_sequence;
    const char *deterrent_trigger;
    const char *evidence_trigger;
    const char *deterrent_source;
    const char *evidence_source;
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

static void test_success(void)
{
    struct test_state state = {0};

    const struct deterrent_sink deterrent = {
        .context = &state,
        .submit = capture_deterrent,
    };

    const struct evidence_sink evidence = {
        .context = &state,
        .record_deterrent_request = capture_evidence,
    };

    const struct motion_event event = {
        .sequence = 42,
        .source = "unit-test",
    };

    assert(cat_guardian_handle_motion(
        &event,
        &deterrent,
        &evidence
    ) == 0);

    assert(state.deterrent_calls == 1);
    assert(state.evidence_calls == 1);

    assert(state.deterrent_order == 1);
    assert(state.evidence_order == 2);

    assert(state.deterrent_sequence == 42);
    assert(state.evidence_sequence == 42);

    assert(strcmp(state.deterrent_trigger, "motion") == 0);
    assert(strcmp(state.evidence_trigger, "motion") == 0);

    assert(strcmp(state.deterrent_source, "unit-test") == 0);
    assert(strcmp(state.evidence_source, "unit-test") == 0);
}

static void test_invalid_inputs(void)
{
    struct test_state state = {0};

    const struct deterrent_sink deterrent = {
        .context = &state,
        .submit = capture_deterrent,
    };

    const struct deterrent_sink invalid_deterrent = {
        .context = &state,
        .submit = 0,
    };

    const struct evidence_sink evidence = {
        .context = &state,
        .record_deterrent_request = capture_evidence,
    };

    const struct evidence_sink invalid_evidence = {
        .context = &state,
        .record_deterrent_request = 0,
    };

    const struct motion_event event = {
        .sequence = 42,
        .source = "unit-test",
    };

    const struct motion_event invalid_event = {
        .sequence = 42,
        .source = 0,
    };

    assert(cat_guardian_handle_motion(
        0,
        &deterrent,
        &evidence
    ) != 0);

    assert(cat_guardian_handle_motion(
        &invalid_event,
        &deterrent,
        &evidence
    ) != 0);

    assert(cat_guardian_handle_motion(
        &event,
        0,
        &evidence
    ) != 0);

    assert(cat_guardian_handle_motion(
        &event,
        &invalid_deterrent,
        &evidence
    ) != 0);

    assert(cat_guardian_handle_motion(
        &event,
        &deterrent,
        0
    ) != 0);

    assert(cat_guardian_handle_motion(
        &event,
        &deterrent,
        &invalid_evidence
    ) != 0);

    assert(state.deterrent_calls == 0);
    assert(state.evidence_calls == 0);
}

static void test_deterrent_failure_stops_evidence(void)
{
    struct test_state state = {
        .deterrent_result = 17,
    };

    const struct deterrent_sink deterrent = {
        .context = &state,
        .submit = capture_deterrent,
    };

    const struct evidence_sink evidence = {
        .context = &state,
        .record_deterrent_request = capture_evidence,
    };

    const struct motion_event event = {
        .sequence = 7,
        .source = "unit-test",
    };

    assert(cat_guardian_handle_motion(
        &event,
        &deterrent,
        &evidence
    ) == 17);

    assert(state.deterrent_calls == 1);
    assert(state.evidence_calls == 0);
}

static void test_evidence_failure_is_propagated(void)
{
    struct test_state state = {
        .evidence_result = 23,
    };

    const struct deterrent_sink deterrent = {
        .context = &state,
        .submit = capture_deterrent,
    };

    const struct evidence_sink evidence = {
        .context = &state,
        .record_deterrent_request = capture_evidence,
    };

    const struct motion_event event = {
        .sequence = 9,
        .source = "unit-test",
    };

    assert(cat_guardian_handle_motion(
        &event,
        &deterrent,
        &evidence
    ) == 23);

    assert(state.deterrent_calls == 1);
    assert(state.evidence_calls == 1);
    assert(state.deterrent_order == 1);
    assert(state.evidence_order == 2);
}

int main(void)
{
    test_success();
    test_invalid_inputs();
    test_deterrent_failure_stops_evidence();
    test_evidence_failure_is_propagated();

    return 0;
}
