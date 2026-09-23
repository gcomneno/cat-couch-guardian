#!/usr/bin/env python3
from __future__ import annotations

import os
import re
from pathlib import Path
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]

DOCUMENT_PAIRS = (
    (Path("README.md"), Path("README.it.md")),
    (Path("docs/learning-path.md"), Path("docs/it/learning-path.md")),
    (Path("docs/architecture.md"), Path("docs/it/architecture.md")),
    (Path("docs/exercises.md"), Path("docs/it/exercises.md")),
    (Path("docs/documentation-policy.md"), Path("docs/it/documentation-policy.md")),
)

MARKDOWN_LINK = re.compile(r"(?<!!)\\[[^]]+]\\(([^)]+)\\)")


def read(path: Path) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def relative_link(from_path: Path, to_path: Path) -> str:
    return Path(os.path.relpath(to_path, start=from_path.parent)).as_posix()


def relative_targets(path: Path) -> list[Path]:
    targets: list[Path] = []
    for raw_target in MARKDOWN_LINK.findall(read(path)):
        target = raw_target.strip().split(maxsplit=1)[0].strip("<>")
        if not target or target.startswith(("#", "http://", "https://", "mailto:")):
            continue
        target = unquote(target.split("#", 1)[0])
        if target:
            targets.append((ROOT / path.parent / target).resolve())
    return targets


def main() -> int:
    errors: list[str] = []
    root = ROOT.resolve()

    for english, italian in DOCUMENT_PAIRS:
        for path in (english, italian):
            if not (ROOT / path).is_file():
                errors.append(f"missing bilingual document: {path}")

        if not (ROOT / english).is_file() or not (ROOT / italian).is_file():
            continue

        english_head = "\n".join(read(english).splitlines()[:8])
        italian_head = "\n".join(read(italian).splitlines()[:8])

        for label in ("English", "Italiano"):
            if label not in english_head:
                errors.append(f"{english}: missing {label} selector")
            if label not in italian_head:
                errors.append(f"{italian}: missing {label} selector")

        if relative_link(english, italian) not in english_head:
            errors.append(f"{english}: wrong Italian selector target")
        if relative_link(italian, english) not in italian_head:
            errors.append(f"{italian}: wrong English selector target")

        for path in (english, italian):
            for target in relative_targets(path):
                if target != root and root not in target.parents:
                    errors.append(f"{path}: target escapes repository: {target}")
                elif not target.exists():
                    errors.append(f"{path}: missing relative target: {target}")

    italian_readme = ROOT / "README.it.md"
    if italian_readme.is_file():
        text = read(Path("README.it.md"))
        for target in (
            "docs/it/learning-path.md",
            "docs/it/architecture.md",
            "docs/it/exercises.md",
            "docs/it/documentation-policy.md",
        ):
            if f"({target})" not in text:
                errors.append(f"README.it.md: missing same-language link to {target}")

    if errors:
        for error in errors:
            print(f"DOCUMENTATION_GATE=FAIL {error}")
        return 1

    print("DOCUMENTATION_GATE=PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
