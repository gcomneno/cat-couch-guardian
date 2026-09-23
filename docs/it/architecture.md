# Guida all'architettura

[English](../architecture.md) | [Italiano](architecture.md)

Questa guida spiega il design M0.6 attuale come esercizio di engineering.

## 1. Parti dal comportamento

Il sistema esegue attualmente una transazione deterministica:

```text
motion event
-> semantic deterrent request
-> deterrent sink accepts request
-> evidence records success
```

La parola importante è **semantic**.

Il core chiede "deterrence". Non chiede ad ALSA di riprodurre un file WAV, di commutare un pin GPIO o di chiamare un'API specifica di un dispositivo.

Questa separazione è la lezione architetturale centrale.

## 2. Dati di dominio

### `motion_event`

Posizione:

```text
app/cat-guardian/include/motion_event.h
```

Rappresenta ciò che il core deve conoscere su un evento di movimento osservato.

### `deterrent_request`

Posizione:

```text
app/cat-guardian/include/deterrent_request.h
```

È l'output semantico prodotto dal core.

Contiene informazioni sufficienti per il comportamento downstream e l'evidence senza esporre dettagli dell'implementazione fisica.

## 3. Port

Una port descrive ciò di cui il core ha bisogno dal mondo esterno.

### `deterrent_sink`

Posizione:

```text
app/cat-guardian/include/deterrent_sink.h
```

Il core invia una deterrent request attraverso questa port.

La port non dice nulla su speaker, GPIO, formati audio o driver.

### `evidence_sink`

Posizione:

```text
app/cat-guardian/include/evidence_sink.h
```

Questa port registra il risultato semantico riuscito.

Tenere l'evidence dietro una port rende il core testabile senza accoppiarlo a standard output, file, journal o servizi di rete.

## 4. Core

Posizione:

```text
app/cat-guardian/src/cat_guardian_core.c
```

Il core:

1. valida gli input richiesti;
2. converte un motion event in una deterrent request;
3. invia la request alla deterrent port;
4. si ferma immediatamente se la deterrenza fallisce;
5. registra evidence solo dopo una deterrenza riuscita;
6. propaga lo status risultante.

Questo ordinamento fa parte del contratto, non è un dettaglio implementativo accidentale.

## 5. Adapter

Gli adapter collegano port astratte a meccanismi concreti.

### Simulated motion source

File:

```text
app/cat-guardian/include/simulated_motion_source.h
app/cat-guardian/src/simulated_motion_source.c
```

Crea un motion event deterministico e lo inoltra attraverso il confine handler.

### Simulated deterrent sink

File:

```text
app/cat-guardian/include/simulated_deterrent_sink.h
app/cat-guardian/src/simulated_deterrent_sink.c
```

Accetta request semantiche senza azione fisica.

È intenzionalmente noioso. Un buon test adapter dovrebbe ridurre l'incertezza.

### Adapter evidence su standard output

L'entry point corrente dell'applicazione adatta la evidence port a `stdout`.

Questo offre ai test host e alle esecuzioni target un canale di evidence semplice e deterministico.

## 6. Composition Root

Posizione:

```text
app/cat-guardian/src/main.c
```

`main.c` collega le parti tra loro.

Può sapere quali adapter sono stati selezionati.

Il core no.

Questa distinzione è utile anche in sistemi più grandi: configurazione e dipendenze concrete appartengono vicino al bordo dell'applicazione.

## 7. Test come contratto eseguibile

Posizione:

```text
app/cat-guardian/tests/test_core.c
```

Leggi il file di test come una specifica comportamentale.

I casi importanti sono:

- request ed evidence riuscite;
- input non validi;
- un errore di deterrenza sopprime l'evidence;
- un errore di evidence viene propagato;
- l'ordinamento resta deterministico.

Un junior engineer dovrebbe saper spiegare perché esiste ciascun test prima di modificare l'implementazione.

## 8. Confine Yocto

Il layer Yocto non fa parte della logica di dominio.

È meccanica di integrazione che:

- consuma un archivio sorgente pinned;
- cross-compila l'applicazione;
- installa il binario;
- installa la unit systemd;
- aggiunge il package a un'immagine derivata.

È una separazione utile:

```text
application behavior != operating-system packaging
```

Entrambi contano, ma provano cose differenti.

## 9. Design smell da evitare

Quando estendi il progetto, fermati se stai per:

- includere ALSA, GPIO, sysfs o header di device nel core;
- rendere i test dipendenti da sleep sul wall clock;
- emettere success evidence prima che l'azione richiesta abbia successo;
- nascondere una nuova regola comportamentale dentro un adapter;
- lasciare che una recipe Yocto diventi l'unico posto in cui esercitare il comportamento dell'applicazione;
- aggiungere hardware soltanto perché un confine software è poco chiaro.

Preferisci prima un contratto semantico e un test deterministico.

## 10. Test architetturale per modifiche future

Per ogni feature proposta, chiedi:

1. È una decisione di dominio o un dettaglio di adapter?
2. Quale contratto cambia?
3. Il comportamento può essere testato senza hardware?
4. Quale errore deve impedire la success evidence?
5. Quale evidence esatta prova il comportamento?
6. Deve cambiare Yocto oppure soltanto l'applicazione?

Se queste domande hanno risposte nette, la modifica è probabilmente pronta per essere implementata.
