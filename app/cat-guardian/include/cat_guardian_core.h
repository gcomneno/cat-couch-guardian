#ifndef CAT_GUARDIAN_CORE_H
#define CAT_GUARDIAN_CORE_H

#include <stdint.h>

#include "deterrent_sink.h"
#include "evidence_sink.h"
#include "motion_event.h"

struct cat_guardian_state {
    uint64_t cooldown_ms;
    uint64_t last_deterrent_at_ms;
    int has_last_deterrent;
};

int cat_guardian_state_init(
    struct cat_guardian_state *state,
    uint64_t cooldown_ms
);

int cat_guardian_handle_motion(
    struct cat_guardian_state *state,
    const struct motion_event *event,
    const struct deterrent_sink *deterrent,
    const struct evidence_sink *evidence
);

#endif
