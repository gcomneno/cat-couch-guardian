# Architecture Guide

[English](architecture.md) | [Italiano](it/architecture.md)

This guide explains the current M0.6 design as an engineering exercise.

## 1. Start From the Behavior

The system currently performs one deterministic transaction:

```text
motion event
-> semantic deterrent request
-> deterrent sink accepts request
-> evidence records success
```

The important word is **semantic**.

The core asks for "deterrence". It does not ask ALSA to play a WAV file, toggle a GPIO pin, or call a device-specific API.

That separation is the central architectural lesson.

## 2. Domain Data

### `motion_event`

Location:

```text
app/cat-guardian/include/motion_event.h
```

This represents something the core needs to know about an observed motion event.

### `deterrent_request`

Location:

```text
app/cat-guardian/include/deterrent_request.h
```

This is the semantic output produced by the core.

It contains enough information for downstream behavior and evidence without exposing physical implementation details.

## 3. Ports

A port describes what the core needs from the outside world.

### `deterrent_sink`

Location:

```text
app/cat-guardian/include/deterrent_sink.h
```

The core submits a deterrent request through this port.

The port says nothing about speakers, GPIO, audio formats, or drivers.

### `evidence_sink`

Location:

```text
app/cat-guardian/include/evidence_sink.h
```

This port records the successful semantic result.

Keeping evidence behind a port makes the core testable without coupling it to standard output, files, journals, or network services.

## 4. Core

Location:

```text
app/cat-guardian/src/cat_guardian_core.c
```

The core:

1. validates required inputs;
2. converts a motion event into a deterrent request;
3. submits the request to the deterrent port;
4. stops immediately if deterrence fails;
5. records evidence only after successful deterrence;
6. propagates the resulting status.

This ordering is part of the contract, not an incidental implementation detail.

## 5. Adapters

Adapters connect abstract ports to concrete mechanisms.

### Simulated motion source

Files:

```text
app/cat-guardian/include/simulated_motion_source.h
app/cat-guardian/src/simulated_motion_source.c
```

It creates a deterministic motion event and forwards it through the handler boundary.

### Simulated deterrent sink

Files:

```text
app/cat-guardian/include/simulated_deterrent_sink.h
app/cat-guardian/src/simulated_deterrent_sink.c
```

It accepts semantic requests without physical action.

This is intentionally boring. A good test adapter should reduce uncertainty.

### Standard-output evidence adapter

The current application entry point adapts the evidence port to `stdout`.

That gives host tests and target runs a simple deterministic evidence channel.

## 6. Composition Root

Location:

```text
app/cat-guardian/src/main.c
```

`main.c` wires the parts together.

It is allowed to know which adapters are selected.

The core is not.

This distinction is useful in larger systems too: configuration and concrete dependencies belong near the edge of the application.

## 7. Tests as Executable Contract

Location:

```text
app/cat-guardian/tests/test_core.c
```

Read the test file as a behavioral specification.

The important cases are:

- successful request and evidence;
- invalid inputs;
- deterrent failure suppresses evidence;
- evidence failure is propagated;
- ordering remains deterministic.

A junior engineer should be able to explain why each test exists before modifying the implementation.

## 8. Yocto Boundary

The Yocto layer is not part of the domain logic.

It is integration machinery that:

- consumes a pinned source archive;
- cross-compiles the application;
- installs the binary;
- installs the systemd unit;
- adds the package to a derived image.

This is a useful separation:

```text
application behavior != operating-system packaging
```

Both matter, but they prove different things.

## 9. Design Smells to Avoid

When extending the project, stop if you are about to:

- include ALSA, GPIO, sysfs, or device headers in the core;
- make tests depend on wall-clock sleep;
- emit success evidence before the requested action succeeds;
- hide a new behavioral rule inside an adapter;
- let a Yocto recipe become the only place where application behavior can be exercised;
- add hardware merely because a software boundary is unclear.

Prefer a semantic contract and a deterministic test first.

## 10. Architecture Test for Future Changes

For each proposed feature, ask:

1. Is this a domain decision or an adapter detail?
2. Which contract changes?
3. Can the behavior be tested without hardware?
4. What failure must prevent success evidence?
5. What exact evidence proves the behavior?
6. Does Yocto need to change, or only the application?

If those questions have crisp answers, the change is probably ready to implement.
