# Junior Engineering Exercises

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

## Exercise 7 — Design M0.7 Cooldown

Current behavior can request deterrence for every accepted motion event.

Design a deterministic cooldown rule.

Example questions:

- What state must be remembered?
- Should time enter the core as a timestamp value or through a clock port?
- What happens at exactly the cooldown boundary?
- What evidence should represent a suppressed event?
- Does suppression count as success, failure, or a separate outcome?

Do not implement until these questions have explicit answers.

## Exercise 8 — Write Tests First

Before implementing cooldown, write a table of cases such as:

| Previous accepted event | New event | Expected decision |
| --- | --- | --- |
| none | t=100 | allow |
| t=100 | t=101 | suppress |
| t=100 | exact boundary | define explicitly |
| t=100 | after boundary | allow |

The precise numbers are less important than defining the boundary.

This exercise introduces test-driven design without requiring a framework.

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
