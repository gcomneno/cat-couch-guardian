#ifndef CAT_GUARDIAN_DETERRENT_REQUEST_H
#define CAT_GUARDIAN_DETERRENT_REQUEST_H

#include <stdint.h>

struct deterrent_request {
    uint64_t sequence;
    const char *trigger;
    const char *source;
};

#endif
