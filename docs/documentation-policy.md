# Documentation Language Policy

[English](documentation-policy.md) | [Italiano](it/documentation-policy.md)

## Canonical language

English is the canonical and default language for maintained public documentation.
Italian is an officially maintained translation for the document families listed
below.

When English and Italian wording diverge, the English document is the source of
truth. A translation must preserve requirements, examples, warnings, limitations,
and technical meaning; it must not be a shortened summary.

Commands, paths, filenames, C symbols, Yocto variables, identifiers, evidence
strings, hashes, and code snippets are never translated.

## Naming and navigation

- The root entry point uses `README.md` / `README.it.md`.
- Canonical maintained documents under `docs/` are English.
- Maintained Italian translations live under `docs/it/` with the same filename.
- Every maintained pair starts with visible reciprocal `English` and `Italiano`
  links.
- Internal links should remain in the reader's language when a maintained mirror
  exists. Otherwise they may point to the English canonical source.

## Maintained bilingual families

The following pairs are part of the synchronization contract:

- `README.md` / `README.it.md`;
- `docs/learning-path.md` / `docs/it/learning-path.md`;
- `docs/architecture.md` / `docs/it/architecture.md`;
- `docs/exercises.md` / `docs/it/exercises.md`;
- `docs/documentation-policy.md` / `docs/it/documentation-policy.md`.

Application source, recipes, machine-readable contracts, evidence values, and
historical immutable identities do not become localized artifacts.

## Synchronization workflow

A change to a maintained canonical document must:

1. evaluate the matching Italian mirror;
2. update both files in the same pull request when meaning changes;
3. preserve reciprocal language selectors;
4. keep technical tokens and snippets unchanged;
5. run the documentation gate.

Run:

```sh
python3 scripts/check_documentation.py
```

The gate verifies required pairs, reciprocal selectors, same-language navigation
for maintained documents, and relative-link integrity. It does not attempt
machine translation or semantic equivalence; semantic parity remains a review
responsibility.

## Educational intent

The bilingual structure is part of the learning experience. A junior engineer
should be able to study the same contracts and exercises in either language
without receiving a simplified or technically different version.

## Non-goals

This policy does not introduce runtime localization, automatic translation, a
documentation-site generator, or any network/runtime dependency.
