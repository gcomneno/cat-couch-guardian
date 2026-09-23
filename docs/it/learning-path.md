# Percorso di apprendimento

[English](../learning-path.md) | [Italiano](learning-path.md)

Cat Couch Guardian è un piccolo progetto embedded Linux progettato per essere letto a strati.

Il modo consigliato per studiarlo non è partire da Yocto o dall'hardware. Parti dal comportamento, identifica i contratti, esegui i controlli host e solo dopo segui lo stesso comportamento nel packaging e nell'immagine target.

## Come usare questo repository

Per ogni milestone:

1. leggi il comportamento previsto;
2. prevedi cosa dovrebbe fare il programma;
3. ispeziona i contratti pubblici in `app/cat-guardian/include/`;
4. ispeziona l'implementazione in `app/cat-guardian/src/`;
5. leggi i test prima di modificare il codice;
6. esegui `make check`;
7. solo dopo che il comportamento host è deterministico, ispeziona la recipe Yocto e l'integrazione target;
8. registra cosa è stato provato e cosa deliberatamente non è stato provato.

Questa sequenza è intenzionale. Insegna agli engineer a separare comportamento, integrazione ed evidence.

## M0.5 — Dall'evento all'evidence

Flusso:

```text
simulated motion -> core -> evidence
```

Impara:

- come un piccolo programma C espone un confine indipendente dall'hardware;
- perché un input simulato è utile prima di acquistare hardware;
- come un output deterministico può fungere da evidence;
- come la stessa applicazione può essere compilata sull'host e impacchettata da Yocto.

Domande a cui rispondere:

- Quale file definisce un motion event?
- Quale parte dell'applicazione sa che la sorgente è simulata?
- Quale parte non lo sa?
- Quale output esatto prova l'happy path?

## M0.6 — Confine semantico di deterrenza

Flusso:

```text
simulated motion
-> core
-> deterrent request
-> simulated deterrent sink
-> evidence
```

Impara:

- ports and adapters;
- contratti semantici rispetto alle API dei dispositivi;
- vincoli di ordinamento;
- propagazione degli errori;
- perché l'evidence deve descrivere un'azione semantica riuscita e non soltanto un evento di input.

Segui questi tipi e funzioni:

- `struct motion_event`
- `struct deterrent_request`
- `struct deterrent_sink`
- `struct evidence_sink`
- `cat_guardian_handle_motion()`

Poi leggi i test e individua dove provano:

- esattamente una deterrent request;
- ordinamento deterrent-before-evidence;
- nessuna success evidence dopo un errore di deterrenza;
- propagazione dell'errore di evidence.

## Host, Packaging, Target

Sono livelli di prova differenti.

### Prova host

`make check` prova il comportamento deterministico dell'applicazione con il compilatore nativo.

Non prova il packaging Yocto né il boot del target.

### Prova di packaging

La recipe prova che l'applicazione committed può essere recuperata da un archivio sorgente pinned, cross-compilata, installata e rappresentata come package.

Da sola non prova l'avvio del servizio in un'immagine avviata.

### Prova target

L'immagine derivata e la validazione QEMU provano che il package può esistere in un'immagine Linux ARM64 e partecipare al runtime target.

Mantieni separate queste affermazioni. Una buona engineering evidence dice esattamente cosa è stato testato.

## M0.7 — Stato di cooldown deterministico

M0.7 introduce la prima state machine esplicita nel core.

Contratto:

```text
motion event + observed_at_ms
-> core cooldown policy
-> ALLOW: deterrent request -> success evidence
-> SUPPRESS: no deterrent request -> suppression evidence
```

Lo stato è esplicito in `struct cat_guardian_state` e contiene la durata del
cooldown insieme al timestamp dell'ultima azione di deterrenza riuscita.

La regola è precisa:

- il primo evento valido è consentito;
- un elapsed time strettamente minore di `cooldown_ms` viene soppresso;
- un elapsed time esattamente uguale a `cooldown_ms` viene consentito;
- un elapsed time maggiore di `cooldown_ms` viene consentito;
- un timestamp precedente all'ultima deterrenza riuscita è input non valido.

Il timestamp entra come dato dell'evento. Il core non legge un wall clock e i
test non usano mai sleep.

Un errore di deterrenza non avvia il cooldown. Quando la deterrenza ha successo,
lo stato del cooldown avanza prima di registrare la success evidence. Quindi un
errore di evidence non porta il core a ripetere un'azione di deterrenza già
riuscita.

Questa è la lezione chiave di M0.7: lo stato applicativo deve descrivere ciò che
è realmente accaduto, non soltanto se telemetria/evidence ha avuto successo.

## Milestone successivi

Solo dopo aver specificato bene il comportamento il progetto dovrebbe aggiungere adapter fisici.

Sequenza possibile:

```text
simulated source
-> deterministic stateful core
-> simulated deterrent
-> QEMU validation
-> physical motion adapter
-> physical deterrent adapter
```

Un vero sensore PIR o speaker dovrebbe sostituire un adapter, non forzare una riprogettazione del core.

## Regola di completamento

Un milestone è completo quando:

- il suo contratto è esplicito;
- happy path e failure path sono testati;
- i controlli host passano;
- l'identità del packaging è pinned quando il packaging cambia;
- la validazione target viene eseguita quando cambia il comportamento target;
- la documentazione dichiara sia ciò che è provato sia ciò che resta fuori scope.

Non trattare "it compiled" come equivalente a "the behavior is proved."
