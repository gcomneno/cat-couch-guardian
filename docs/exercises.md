# Junior Engineering Exercises

[English](exercises.md) | [Italiano](it/exercises.md)

These exercises are designed to be solved by reading, predicting, testing, and only then modifying code.

## Exercise 1 — Trace One Event

Without running the program, trace a single sequence value of `1` from the simulated source to the evidence line.

Write down:

- where the value is created;
- which function receives it next;
- where the deterrent request is created;
- where the request is accepted;
- where evidence is emitted.

Then run:

```sh
cd app/cat-guardian
make check
```

Compare your prediction with the observed output.

## Exercise 2 — Find the Hardware Boundary

Search the core for references to:

- speaker;
- audio;
- GPIO;
- ALSA;
- device paths.

Expected result: none.

Explain why this absence is a feature rather than missing functionality.

## Exercise 3 — Predict a Failure

Read `cat_guardian_handle_motion()`.

Suppose the deterrent sink returns `17`.

Before reading the test, predict:

- the return value from the core;
- whether evidence is called;
- whether the request was still created.

Then verify your answer in `tests/test_core.c`.

## Exercise 4 — Evidence Failure

Suppose deterrence succeeds but evidence returns `23`.

Predict:

- whether deterrence has already happened;
- which return value reaches the caller;
- whether retrying evidence automatically would be safe.

The final question has no single implementation answer. Discuss the trade-off before changing code.

## Exercise 5 — Ports and Adapters Classification

Classify each item as domain data, port, adapter, composition, packaging, or test:

- `motion_event`
- `deterrent_request`
- `deterrent_sink`
- `simulated_motion_source.c`
- `simulated_deterrent_sink.c`
- `main.c`
- `cat-guardian_0.1.bb`
- `test_core.c`

Then compare your choices with `docs/architecture.md`.

## Exercise 6 — Change the Adapter, Not the Core

Design a hypothetical audio deterrent adapter.

Do not implement it.

Specify only:

- what input it receives;
- what success means;
- possible error conditions;
- what external library or device it might use.

Then explain why none of those details belong in `deterrent_request.h`.

## Exercise 7 — Read the M0.7 Cooldown Contract

Inspect `struct cat_guardian_state`, `motion_event.observed_at_ms`, and
`cat_guardian_handle_motion()`.

Before reading the tests, predict the outcome for:

- no previous deterrent;
- elapsed time one millisecond below the boundary;
- elapsed time exactly at the boundary;
- elapsed time one millisecond after the boundary;
- a timestamp that moves backwards.

Then verify each prediction in `tests/test_core.c`.

## Exercise 8 — State Versus Evidence Failure

A successful deterrent action updates cooldown state before success evidence is
recorded.

Explain why this matters when the evidence sink returns an error.

Then find the test proving that a following event inside cooldown is suppressed
rather than causing a duplicate deterrent action.

This exercise distinguishes domain state from observability/telemetry state.

## Exercise 9 — Separate Proof Layers

For each statement, identify whether it requires host testing, recipe build, image build, or target runtime validation:

- the core suppresses a second event during cooldown;
- the application cross-compiles for ARM64;
- the package is present in the image;
- systemd starts the service;
- the target emits the expected evidence line.

The answer should not be "BitBake proves everything."

## Exercise 10 — Review a Proposed Bad Change

Consider this proposal:

> Add ALSA calls directly inside `cat_guardian_handle_motion()`, sleep for five seconds after playing audio, and print a success line.

List the architectural problems.

Then redesign it using:

- a semantic core decision;
- a deterrent port;
- an audio adapter;
- deterministic time/cooldown handling;
- evidence after successful action.

## Definition of Done for an Exercise Patch

A learning patch is complete when the author can explain:

- the behavior being changed;
- the contract affected;
- the tests proving the behavior;
- what remains simulated;
- what host validation proves;
- whether Yocto or target validation is required.

The explanation is part of the engineering work.
