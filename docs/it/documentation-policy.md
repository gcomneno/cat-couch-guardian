# Policy linguistica della documentazione

[English](../documentation-policy.md) | [Italiano](documentation-policy.md)

## Lingua canonica

L'inglese è la lingua canonica e predefinita della documentazione pubblica
mantenuta. L'italiano è una traduzione ufficialmente mantenuta per le famiglie
di documenti elencate sotto.

Quando il testo inglese e quello italiano divergono, il documento inglese è la
fonte di verità. Una traduzione deve preservare requisiti, esempi, avvertenze,
limitazioni e significato tecnico; non deve essere un riassunto abbreviato.

Comandi, percorsi, nomi file, simboli C, variabili Yocto, identificatori, stringhe
di evidence, hash e frammenti di codice non vengono tradotti.

## Nomi e navigazione

- Il punto di ingresso nella root usa `README.md` / `README.it.md`.
- I documenti canonici mantenuti sotto `docs/` sono in inglese.
- Le traduzioni italiane mantenute vivono sotto `docs/it/` con lo stesso nome file.
- Ogni coppia mantenuta inizia con link reciproci visibili `English` e `Italiano`.
- I link interni dovrebbero restare nella lingua del lettore quando esiste un mirror
  mantenuto. Altrimenti possono puntare alla fonte canonica inglese.

## Famiglie bilingui mantenute

Le seguenti coppie fanno parte del contratto di sincronizzazione:

- `README.md` / `README.it.md`;
- `docs/learning-path.md` / `docs/it/learning-path.md`;
- `docs/architecture.md` / `docs/it/architecture.md`;
- `docs/exercises.md` / `docs/it/exercises.md`;
- `docs/documentation-policy.md` / `docs/it/documentation-policy.md`.

Il codice applicativo, le recipe, i contratti machine-readable, i valori di
evidence e le identità storiche immutabili non diventano artefatti localizzati.

## Flusso di sincronizzazione

Una modifica a un documento canonico mantenuto deve:

1. valutare il mirror italiano corrispondente;
2. aggiornare entrambi i file nella stessa pull request quando cambia il significato;
3. preservare i selettori linguistici reciproci;
4. mantenere invariati token tecnici e snippet;
5. eseguire il gate della documentazione.

Eseguire:

```sh
python3 scripts/check_documentation.py
```

Il gate verifica coppie obbligatorie, selettori reciproci, navigazione nella stessa
lingua per i documenti mantenuti e integrità dei link relativi. Non tenta la
traduzione automatica né il confronto semantico; la parità semantica resta una
responsabilità di review.

## Intento didattico

La struttura bilingue fa parte dell'esperienza di apprendimento. Un junior engineer
deve poter studiare gli stessi contratti e gli stessi esercizi in entrambe le lingue
senza ricevere una versione semplificata o tecnicamente diversa.

## Non-obiettivi

Questa policy non introduce localizzazione runtime, traduzione automatica, un
generatore di siti documentali o dipendenze di rete/runtime.
