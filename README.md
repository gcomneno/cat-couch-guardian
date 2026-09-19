# Cat Couch Guardian

Cat Couch Guardian is a public educational embedded Linux project. It is a
virtual-first learning project using C, Yocto, systemd, and QEMU ARM64.

The cat-deterrent scenario is a learning vehicle. This repository is not a
finished physical product, a commercial product, or a hardware validation
claim.

## Educational Objectives

The project is designed to teach and preserve evidence for:

- hardware-independent boundaries;
- ports and adapters;
- deterministic C11 behavior and tests;
- Yocto layers and recipes;
- AArch64 cross-compilation;
- derived Linux images;
- systemd integration;
- QEMU target validation;
- provenance and immutable evidence;
- virtual-first development before hardware purchases.

## Virtual-First Principle

Cat Couch Guardian validates software boundaries in a virtual target before
buying or integrating physical hardware. The current milestone uses simulated
motion input and deterministic evidence output so that the core behavior can be
tested on the host and in a QEMU AArch64 Linux image without GPIO, sensors,
speakers, or networking.

## Current Maturity

M0.6 host behavior is implemented and verified. M0.5 remains the frozen
previous milestone.

The implemented M0.6 flow is:

```text
SimulatedMotionSource
-> motion event
-> hardware-independent core
-> semantic deterrent request
-> simulated deterrent sink
-> deterministic evidence
```

The exact output is:

```text
evidence type=deterrent-request trigger=motion source=simulated sequence=1
```

The core remains hardware-independent. It expresses a semantic deterrent
request and does not know whether a future physical adapter uses audio or
another actuator. The current deterrent adapter is simulated and performs no
physical action.

## Architecture

The application is a small C11 program with explicit boundaries:

- `SimulatedMotionSource` emits a deterministic motion event.
- The hardware-independent core converts that event into one semantic
  `deterrent_request`.
- The request is submitted through the hardware-independent `deterrent_sink`
  port.
- The simulated deterrent adapter accepts the request without physical
  actuation.
- Evidence is recorded only after successful deterrent submission.

This keeps hardware origin, operating-system integration, and deterministic
core behavior separate enough to teach ports and adapters without introducing
real-device uncertainty.

## What M0.5 Proves

M0.5 proves that the repository can build and test the C application on the
host, package it in a Yocto recipe, include it in a derived image, boot that
image under QEMU ARM64, autostart it through systemd, and produce the frozen
evidence record exactly once.

The verified runtime result was:

- `Type=oneshot` service autostarted from `multi-user.target`;
- exactly one service start, one evidence record, one finish, and QEMU
  termination;
- causal ordering: start -> evidence -> finish.

## Current Exclusions

The following remain intentionally out of scope for M0.6:

- GPIO;
- physical PIR;
- real audio;
- speaker;
- debounce;
- cooldown;
- persistent loop;
- camera;
- computer vision;
- recognition;
- AI;
- MQTT;
- Home Assistant;
- dashboard;
- required networking;
- physical hardware validation.

## Repository Layout

```text
.
├── app/cat-guardian/                         Host-buildable C application
│   ├── include/                              Public C boundaries
│   ├── src/                                  Application and adapters
│   ├── tests/                                Deterministic host tests
│   └── packaging/systemd/                    systemd unit installed by Yocto
├── meta-cat-guardian/                        Yocto layer
│   ├── conf/layer.conf                       Wrynose-compatible layer config
│   ├── recipes-apps/cat-guardian/            Application recipe and source archive
│   └── recipes-core/images/                  Derived image recipe
├── LICENSE                                   Repository MIT license
└── README.md                                 Public project documentation
```

Raw build products, Yocto workspaces, downloads, caches, experiments, local
tools, and runtime evidence are intentionally ignored.

## Host Prerequisites

For the host application:

- POSIX-like shell;
- `make`;
- C compiler with C11 support.

From the application directory:

```sh
cd app/cat-guardian
make clean
make check
```

`make check` builds the application and test binary, runs the core test, and
verifies the exact application output:

```text
evidence type=deterrent-request trigger=motion source=simulated sequence=1
```

## Yocto Layer and Image

The Yocto layer is `meta-cat-guardian`.

The application recipe is:

```text
meta-cat-guardian/recipes-apps/cat-guardian/cat-guardian_0.1.bb
```

The recipe consumes the pinned `cat-guardian-0.1.tar.gz` source archive,
builds and installs the `cat-guardian` binary, and installs the systemd unit:

```text
app/cat-guardian/packaging/systemd/cat-guardian.service
```

The derived image is:

```text
meta-cat-guardian/recipes-core/images/cat-guardian-image.bb
```

It requires `core-image-minimal` and appends only the `cat-guardian` package.

The verified build environment for M0.5 was:

- Yocto 6.0 Wrynose LTS;
- Poky 6.0.3;
- `MACHINE=qemuarm64`;
- `DISTRO=poky-altcfg`;
- baseline `core-image-minimal`;
- derived image `cat-guardian-image`;
- Linux 6.18.39;
- ARM64;
- systemd 259.5;
- QEMU virtualization;
- locked root login;
- no `debug-tweaks`.

The verified local resource policy was:

```bitbake
BB_NUMBER_THREADS = "2"
PARALLEL_MAKE = "-j 2"
INHERIT += "rm_work"
```

## Evidence and Provenance

M0.5 evidence identities are frozen. Raw evidence remains local and ignored;
these artifacts are not claimed to be downloadable from GitHub.

Frozen identities:

| Artifact | SHA-256 | Size |
| --- | --- | --- |
| Runtime log | `75981ac4a6e98ecc64b6fcd1a399c29a635d31e8e35135e8e15d32820adb8c27` | 37441 bytes |
| ext4.zst root filesystem | `0aeeabf8fa01321401fc825db755f05f47e7fa6fc0627eeedbc1d6fa5c06b840` | |
| qemuboot configuration | `7c436e0af9fbaa4db4b04ba31affe1c525e5a7fec9d0f9534d74557a5cc42d9b` | |
| manifest | `a73f885557c5d843e231c010fa7127d6e58049ca947a0dd0c736b7416c02536f` | |
| tar rootfs | `7a3f7f31acc9cb7f4b20e45b61e3c15a257c7d11d38ee7ec63fbf398697f3fa2` | |

The historical pre-relicensing source archive identity was:

```text
5da43633703e3be98de949a6a0214d90065a2cd23a9517df6ad2db47daafd0f7
```

That value identifies the private-development archive before the public MIT
release preparation. The public archive is regenerated from the distributable
`app/cat-guardian` tree, includes the MIT license, and has its own recipe
checksum.

## M0.6 Source Archive

The Yocto source archive for M0.6 is generated directly from the committed
`app/cat-guardian` tree at host checkpoint `2050fea`.

This excludes host build products and unrelated working-tree state by
construction.

Previous pinned public archive:

```text
71c4bea0fedea64c6c0b833a0d2c657eb5a81651287443115211f9ec0598d138
```

M0.6 pinned archive:

```text
b7fcd728404145305b8bb1a198441b1827a2ed83547736031e8d4381a3e964d5
```

The Yocto recipe pins this exact SHA-256 identity.

## Future Physical Adapters

Physical actuation remains future work. A later adapter may use audio or
another actuator, but that implementation choice remains outside the semantic
`deterrent_sink` contract.

M0.6 itself remains simulated, educational, and hardware-free.

## License

Cat Couch Guardian is released under the MIT License.

Copyright (c) 2026 Giancarlo Cicellyn Comneno
