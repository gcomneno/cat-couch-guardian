# Esercizi per junior engineer

[English](../exercises.md) | [Italiano](exercises.md)

Questi esercizi sono pensati per essere risolti leggendo, prevedendo, testando e solo dopo modificando il codice.

## Esercizio 1 — Segui un evento

Senza eseguire il programma, segui un singolo valore di sequence `1` dalla sorgente simulata alla riga di evidence.

Annota:

- dove viene creato il valore;
- quale funzione lo riceve subito dopo;
- dove viene creata la deterrent request;
- dove la request viene accettata;
- dove viene emessa l'evidence.

Poi esegui:

```sh
cd app/cat-guardian
make check
```

Confronta la tua previsione con l'output osservato.

## Esercizio 2 — Trova il confine hardware

Cerca nel core riferimenti a:

- speaker;
- audio;
- GPIO;
- ALSA;
- path di device.

Risultato atteso: nessuno.

Spiega perché questa assenza è una caratteristica e non una funzionalità mancante.

## Esercizio 3 — Prevedi un errore

Leggi `cat_guardian_handle_motion()`.

Supponi che il deterrent sink restituisca `17`.

Prima di leggere il test, prevedi:

- il valore restituito dal core;
- se l'evidence viene chiamata;
- se la request è stata comunque creata.

Poi verifica la risposta in `tests/test_core.c`.

## Esercizio 4 — Errore di evidence

Supponi che la deterrenza abbia successo ma l'evidence restituisca `23`.

Prevedi:

- se la deterrenza è già avvenuta;
- quale valore di ritorno arriva al caller;
- se ritentare automaticamente l'evidence sarebbe sicuro.

L'ultima domanda non ha un'unica risposta implementativa. Discuti il trade-off prima di modificare il codice.

## Esercizio 5 — Classificazione ports and adapters

Classifica ogni elemento come domain data, port, adapter, composition, packaging o test:

- `motion_event`
- `deterrent_request`
- `deterrent_sink`
- `simulated_motion_source.c`
- `simulated_deterrent_sink.c`
- `main.c`
- `cat-guardian_0.1.bb`
- `test_core.c`

Poi confronta le tue scelte con `docs/it/architecture.md`.

## Esercizio 6 — Cambia l'adapter, non il core

Progetta un ipotetico adapter di deterrenza audio.

Non implementarlo.

Specifica soltanto:

- quale input riceve;
- cosa significa successo;
- possibili condizioni di errore;
- quale libreria o device esterno potrebbe usare.

Poi spiega perché nessuno di questi dettagli appartiene a `deterrent_request.h`.

## Esercizio 7 — Leggi il contratto di cooldown M0.7

Ispeziona `struct cat_guardian_state`, `motion_event.observed_at_ms` e
`cat_guardian_handle_motion()`.

Prima di leggere i test, prevedi l'outcome per:

- nessuna deterrenza precedente;
- elapsed time un millisecondo sotto il confine;
- elapsed time esattamente al confine;
- elapsed time un millisecondo dopo il confine;
- un timestamp che torna indietro.

Poi verifica ogni previsione in `tests/test_core.c`.

## Esercizio 8 — Stato rispetto a errore di evidence

Un'azione di deterrenza riuscita aggiorna lo stato del cooldown prima che venga
registrata la success evidence.

Spiega perché questo conta quando l'evidence sink restituisce un errore.

Poi trova il test che prova che un evento successivo dentro il cooldown viene
soppresso invece di provocare una deterrenza duplicata.

Questo esercizio distingue lo stato di dominio dallo stato di osservabilità/telemetria.

## Esercizio 9 — Separa i livelli di prova

Per ogni affermazione, identifica se richiede host testing, recipe build, image build o validazione runtime target:

- il core sopprime un secondo evento durante il cooldown;
- l'applicazione cross-compila per ARM64;
- il package è presente nell'immagine;
- systemd avvia il servizio;
- il target emette la riga di evidence attesa.

La risposta non dovrebbe essere "BitBake proves everything."

## Esercizio 10 — Revisiona una cattiva modifica proposta

Considera questa proposta:

> Add ALSA calls directly inside `cat_guardian_handle_motion()`, sleep for five seconds after playing audio, and print a success line.

Elenca i problemi architetturali.

Poi riprogettala usando:

- una decisione semantica del core;
- una deterrent port;
- un audio adapter;
- gestione deterministica di tempo/cooldown;
- evidence dopo un'azione riuscita.

## Definition of Done per una patch didattica

Una learning patch è completa quando l'autore sa spiegare:

- il comportamento che viene modificato;
- il contratto interessato;
- i test che provano il comportamento;
- cosa resta simulato;
- cosa prova la validazione host;
- se è richiesta validazione Yocto o target.

La spiegazione fa parte del lavoro di engineering.
