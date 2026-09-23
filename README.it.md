# Cat Couch Guardian

[English](README.md) | [Italiano](README.it.md)

Cat Couch Guardian è un progetto pubblico e didattico di embedded Linux. È un
progetto di apprendimento virtual-first che usa C, Yocto, systemd e QEMU ARM64.

Lo scenario di deterrenza del gatto è un veicolo didattico. Questo repository
non è un prodotto fisico finito, un prodotto commerciale né una dichiarazione
di validazione hardware.

L'inglese è la lingua canonica della documentazione. Vedi la
[policy linguistica della documentazione](docs/it/documentation-policy.md).

## Obiettivi didattici

Il progetto è pensato per insegnare e preservare evidence relative a:

- confini indipendenti dall'hardware;
- ports and adapters;
- comportamento C11 deterministico e test;
- layer e recipe Yocto;
- cross-compilazione AArch64;
- immagini Linux derivate;
- integrazione systemd;
- validazione target con QEMU;
- provenienza ed evidence immutabile;
- sviluppo virtual-first prima dell'acquisto di hardware.

## Percorso di apprendimento

Questo repository è anche un laboratorio guidato di engineering per junior developer.

Ordine consigliato:

1. [Percorso di apprendimento](docs/it/learning-path.md) — segui i milestone e comprendi cosa prova ciascun livello di validazione.
2. [Guida all'architettura](docs/it/architecture.md) — collega port, adapter, contratti, logica core, test e integrazione Yocto ai file concreti.
3. [Esercizi per junior engineer](docs/it/exercises.md) — prevedi il comportamento, segui i failure path, classifica i confini e progetta il milestone successivo prima di modificare il codice.

Il ciclo di apprendimento previsto è:

```text
read the contract
-> predict behavior
-> inspect tests
-> run host validation
-> inspect packaging
-> validate the target when required
-> record exactly what was proved
```

M0.7 aggiunge la prima lezione esplicitamente stateful: decisioni di cooldown deterministiche, ancora virtual-first e indipendenti dall'hardware.

## Principio virtual-first

Cat Couch Guardian valida i confini software su un target virtuale prima di
acquistare o integrare hardware fisico. Il milestone attuale usa input di
movimento simulato ed evidence deterministica, così il comportamento del core
può essere testato sull'host e in un'immagine Linux AArch64 sotto QEMU senza
GPIO, sensori, speaker o networking.

## Maturità attuale

Il comportamento host M0.7, il packaging sorgente immutabile e la build della
recipe per qemuarm64 sono implementati e verificati. M0.6 resta il milestone
precedente congelato.

Il flusso M0.7 implementato è:

```text
SimulatedMotionSource
-> motion event + deterministic timestamp
-> hardware-independent cooldown state
-> allow: semantic deterrent request -> simulated deterrent sink -> success evidence
-> suppress: no deterrent request -> suppression evidence
```

L'output esatto è:

```text
evidence type=deterrent-request trigger=motion source=simulated sequence=1
evidence type=motion-suppressed reason=cooldown source=simulated sequence=2 observed-at-ms=1001
evidence type=deterrent-request trigger=motion source=simulated sequence=3
```

Il core resta indipendente dall'hardware. Esprime una richiesta semantica di
deterrenza e non sa se un futuro adapter fisico userà audio o un altro attuatore.
L'adapter di deterrenza attuale è simulato e non esegue alcuna azione fisica.

## Architettura

L'applicazione è un piccolo programma C11 con confini espliciti:

- `SimulatedMotionSource` emette un motion event deterministico.
- Il core indipendente dall'hardware converte quell'evento in una singola
  `deterrent_request` semantica.
- La richiesta viene inviata attraverso la port `deterrent_sink`.
- L'adapter di deterrenza simulato accetta la richiesta senza attuazione fisica.
- L'evidence viene registrata solo dopo una deterrenza riuscita.

Questa separazione mantiene distinti origine hardware, integrazione del sistema
operativo e comportamento deterministico del core, abbastanza da insegnare
ports and adapters senza introdurre l'incertezza dei dispositivi reali.

## Cosa prova M0.5

M0.5 prova che il repository può compilare e testare l'applicazione C sull'host,
impacchettarla in una recipe Yocto, includerla in un'immagine derivata, avviare
quell'immagine sotto QEMU ARM64, avviarla automaticamente tramite systemd e
produrre esattamente una volta il record di evidence congelato.

Il risultato runtime verificato era:

- servizio `Type=oneshot` avviato automaticamente da `multi-user.target`;
- esattamente un avvio del servizio, un record di evidence, una conclusione e
  la terminazione QEMU;
- ordine causale: start -> evidence -> finish.

## Esclusioni attuali

Quanto segue resta intenzionalmente fuori scope per M0.7:

- GPIO;
- PIR fisico;
- audio reale;
- speaker;
- debounce;
- loop persistente;
- camera;
- computer vision;
- recognition;
- AI;
- MQTT;
- Home Assistant;
- dashboard;
- networking obbligatorio;
- validazione hardware fisica.

## Struttura del repository

```text
.
├── app/cat-guardian/                         Host-buildable C application
│   ├── include/                              Public C boundaries
│   ├── src/                                  Application and adapters
│   ├── tests/                                Deterministic host tests
│   └── packaging/systemd/                    systemd unit installed by Yocto
├── docs/                                     Guided learning material
│   ├── learning-path.md                      Milestone-based study path
│   ├── architecture.md                       Ports/adapters and proof boundaries
│   ├── exercises.md                          Hands-on junior engineering exercises
│   ├── documentation-policy.md               Bilingual documentation contract
│   └── it/                                   Maintained Italian mirrors
├── meta-cat-guardian/                        Yocto layer
│   ├── conf/layer.conf                       Wrynose-compatible layer config
│   ├── recipes-apps/cat-guardian/            Application recipe and source archive
│   └── recipes-core/images/                  Derived image recipe
├── scripts/check_documentation.py            Bilingual documentation validator
├── LICENSE                                   Repository MIT license
└── README.md                                 Canonical English project entry point
```

Prodotti di build grezzi, workspace Yocto, download, cache, esperimenti, tool
locali ed evidence runtime vengono intenzionalmente ignorati.

## Prerequisiti host

Per l'applicazione host:

- shell POSIX-like;
- `make`;
- compilatore C con supporto C11.

Dalla directory dell'applicazione:

```sh
cd app/cat-guardian
make clean
make check
```

`make check` compila applicazione e binario di test, esegue il test del core e
verifica l'output esatto dell'applicazione:

```text
evidence type=deterrent-request trigger=motion source=simulated sequence=1
```

## Controllo della documentazione

Esegui il gate della documentazione bilingue con:

```sh
python3 scripts/check_documentation.py
```

Il gate verifica coppie inglese/italiano obbligatorie, selettori linguistici
reciproci, navigazione nella stessa lingua per i documenti mantenuti e link relativi.

## Layer e immagine Yocto

Il layer Yocto è `meta-cat-guardian`.

La recipe dell'applicazione è:

```text
meta-cat-guardian/recipes-apps/cat-guardian/cat-guardian_0.1.bb
```

La recipe consuma l'archivio sorgente pinned `cat-guardian-0.1.tar.gz`,
compila e installa il binario `cat-guardian` e installa la unit systemd:

```text
app/cat-guardian/packaging/systemd/cat-guardian.service
```

L'immagine derivata è:

```text
meta-cat-guardian/recipes-core/images/cat-guardian-image.bb
```

Richiede `core-image-minimal` e aggiunge soltanto il package `cat-guardian`.

L'ambiente di build verificato per M0.5 era:

- Yocto 6.0 Wrynose LTS;
- Poky 6.0.3;
- `MACHINE=qemuarm64`;
- `DISTRO=poky-altcfg`;
- baseline `core-image-minimal`;
- immagine derivata `cat-guardian-image`;
- Linux 6.18.39;
- ARM64;
- systemd 259.5;
- virtualizzazione QEMU;
- login root bloccato;
- nessun `debug-tweaks`.

La policy locale delle risorse verificata era:

```bitbake
BB_NUMBER_THREADS = "2"
PARALLEL_MAKE = "-j 2"
INHERIT += "rm_work"
```

## Evidence e provenienza

Le identità delle evidence M0.5 sono congelate. L'evidence grezza resta locale e
ignorata; questi artefatti non vengono dichiarati scaricabili da GitHub.

Identità congelate:

| Artifact | SHA-256 | Size |
| --- | --- | --- |
| Runtime log | `75981ac4a6e98ecc64b6fcd1a399c29a635d31e8e35135e8e15d32820adb8c27` | 37441 bytes |
| ext4.zst root filesystem | `0aeeabf8fa01321401fc825db755f05f47e7fa6fc0627eeedbc1d6fa5c06b840` | |
| qemuboot configuration | `7c436e0af9fbaa4db4b04ba31affe1c525e5a7fec9d0f9534d74557a5cc42d9b` | |
| manifest | `a73f885557c5d843e231c010fa7127d6e58049ca947a0dd0c736b7416c02536f` | |
| tar rootfs | `7a3f7f31acc9cb7f4b20e45b61e3c15a257c7d11d38ee7ec63fbf398697f3fa2` | |

L'identità storica dell'archivio sorgente precedente alla relicensing era:

```text
5da43633703e3be98de949a6a0214d90065a2cd23a9517df6ad2db47daafd0f7
```

Questo valore identifica l'archivio di sviluppo privato precedente alla
preparazione della release pubblica MIT. L'archivio pubblico viene rigenerato
dall'albero distribuibile `app/cat-guardian`, include la licenza MIT e possiede
un proprio checksum di recipe.

## Archivio sorgente M0.7

L'archivio sorgente Yocto per M0.7 viene generato direttamente dall'albero
`app/cat-guardian` committed al checkpoint host `ffcb37f`.

Questo esclude per costruzione prodotti di build host e stato non correlato del
working tree.

Archivio pubblico pinned precedente:

```text
71c4bea0fedea64c6c0b833a0d2c657eb5a81651287443115211f9ec0598d138
```

Archivio M0.6 pinned precedente:

```text
b7fcd728404145305b8bb1a198441b1827a2ed83547736031e8d4381a3e964d5
```

Archivio M0.7 pinned:

```text
9d45b2bb42bb17efd8b51f0aaf398c771e5d9d50ba7b27a1dc8cc5f0c789b26f
```

La recipe Yocto pinna esattamente questa identità SHA-256.

## Futuri adapter fisici

L'attuazione fisica resta lavoro futuro. Un adapter successivo potrà usare audio
o un altro attuatore, ma quella scelta implementativa resta fuori dal contratto
semantico `deterrent_sink`.

M0.7 resta simulato, didattico e privo di hardware.

## Licenza

Cat Couch Guardian è rilasciato con licenza MIT.

Copyright (c) 2026 Giancarlo Cicellyn Comneno
