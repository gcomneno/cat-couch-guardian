# Learning Path

[English](learning-path.md) | [Italiano](it/learning-path.md)

Cat Couch Guardian is a small embedded Linux project designed to be read in layers.

The recommended way to study it is not to start from Yocto or from hardware. Start from the behavior, identify the contracts, run the host checks, and only then follow the same behavior into packaging and the target image.

## How to Use This Repository

For every milestone:

1. read the intended behavior;
2. predict what the program should do;
3. inspect the public contracts in `app/cat-guardian/include/`;
4. inspect the implementation in `app/cat-guardian/src/`;
5. read the tests before changing code;
6. run `make check`;
7. only after host behavior is deterministic, inspect the Yocto recipe and target integration;
8. record what was proved and what was deliberately not proved.

This sequence is intentional. It teaches engineers to separate behavior, integration, and evidence.

## M0.5 — Event to Evidence

Flow:

```text
simulated motion -> core -> evidence
```

Learn:

- how a small C program exposes a hardware-independent boundary;
- why simulated input is useful before buying hardware;
- how deterministic output can act as evidence;
- how the same application can be built on the host and packaged by Yocto.

Questions to answer:

- Which file defines a motion event?
- Which part of the application knows that the source is simulated?
- Which part does not?
- What exact output proves the happy path?

## M0.6 — Semantic Deterrent Boundary

Flow:

```text
simulated motion
-> core
-> deterrent request
-> simulated deterrent sink
-> evidence
```

Learn:

- ports and adapters;
- semantic contracts versus device APIs;
- ordering constraints;
- failure propagation;
- why evidence must describe a successful semantic action rather than merely an input event.

Trace these types and functions:

- `struct motion_event`
- `struct deterrent_request`
- `struct deterrent_sink`
- `struct evidence_sink`
- `cat_guardian_handle_motion()`

Then read the tests and identify where they prove:

- exactly one deterrent request;
- deterrent-before-evidence ordering;
- no success evidence after deterrent failure;
- propagation of evidence failure.

## Host, Packaging, Target

These are different proof layers.

### Host proof

`make check` proves deterministic application behavior with the native compiler.

It does not prove Yocto packaging or target boot.

### Packaging proof

The recipe proves that the committed application can be fetched from a pinned source archive, cross-compiled, installed, and represented as a package.

It does not by itself prove service startup in a booted image.

### Target proof

The derived image and QEMU validation prove that the package can exist in an ARM64 Linux image and participate in the target runtime.

Keep these claims separate. Good engineering evidence says exactly what was tested.

## M0.7 — Reactive Behavior

The next recommended functional lesson is a small stateful policy such as cooldown.

Example intent:

```text
motion
-> decision: allow or suppress
-> deterrent request only when allowed
-> evidence describing the decision
```

The educational goal is not merely to add a timer. It is to introduce controlled state while preserving deterministic tests and keeping time/hardware behind explicit boundaries.

Before implementing M0.7, define:

- the state owned by the core;
- the input needed to make a decision;
- the exact suppression rule;
- deterministic time representation for tests;
- evidence for both allowed and suppressed events;
- failure behavior.

## Later Milestones

Only after the behavior is well specified should the project add physical adapters.

Possible sequence:

```text
simulated source
-> deterministic stateful core
-> simulated deterrent
-> QEMU validation
-> physical motion adapter
-> physical deterrent adapter
```

A real PIR sensor or speaker should replace an adapter, not force a redesign of the core.

## Completion Rule

A milestone is complete when:

- its contract is explicit;
- its happy path and failure paths are tested;
- host checks pass;
- packaging identity is pinned when packaging changes;
- target validation is performed when target behavior changes;
- documentation states both what is proved and what remains outside scope.

Do not treat "it compiled" as equivalent to "the behavior is proved."
