CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -O2
LDFLAGS ?=

BUILD_DIR ?= build

APP := $(BUILD_DIR)/cat-guardian
TEST := $(BUILD_DIR)/test-core

COMMON_SOURCES := \
	src/cat_guardian_core.c

APP_SOURCES := \
	$(COMMON_SOURCES) \
	src/simulated_motion_source.c \
	src/simulated_deterrent_sink.c \
	src/main.c

TEST_SOURCES := \
	$(COMMON_SOURCES) \
	tests/test_core.c

.PHONY: all test check clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SOURCES) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) $(APP_SOURCES) $(LDFLAGS) -o $@

$(TEST): $(TEST_SOURCES) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) $(TEST_SOURCES) $(LDFLAGS) -o $@

test: $(TEST)
	$(TEST)

check: all test
	EXPECTED="$$(printf '%s\n' \
		'evidence type=deterrent-request trigger=motion source=simulated sequence=1' \
		'evidence type=motion-suppressed reason=cooldown source=simulated sequence=2 observed-at-ms=1001' \
		'evidence type=deterrent-request trigger=motion source=simulated sequence=3')"; \
	test "$$($(APP))" = "$$EXPECTED"

clean:
	rm -rf $(BUILD_DIR)
