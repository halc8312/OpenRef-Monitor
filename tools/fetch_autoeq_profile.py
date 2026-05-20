#!/usr/bin/env python3
from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path


AUTOEQ_URL = "https://github.com/jaakkopasanen/AutoEq.git"
DEFAULT_CACHE = Path.home() / ".cache" / "openref-monitor" / "AutoEq"


def ensure_autoeq(cache: Path) -> None:
    if (cache / ".git").exists():
        subprocess.run(["git", "-C", str(cache), "pull", "--ff-only"], check=True)
        return
    cache.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["git", "clone", "--depth", "1", AUTOEQ_URL, str(cache)], check=True)


def find_candidates(cache: Path, model: str) -> list[Path]:
    results_dir = cache / "results"
    candidates: list[Path] = []
    for readme in results_dir.rglob("README.md"):
        if model.lower() not in readme.read_text(encoding="utf-8", errors="ignore").lower():
            continue
        if list(readme.parent.glob("*ParametricEQ.txt")) or list(readme.parent.glob("*GraphicEQ.txt")):
            candidates.append(readme)
    return sorted(candidates)


def choose_candidate(candidates: list[Path], preferred: str) -> Path:
    for candidate in candidates:
        if preferred.lower() in str(candidate).lower():
            return candidate.parent
    if not candidates:
        raise SystemExit("No AutoEq candidates found")
    return candidates[0].parent


def main() -> None:
    parser = argparse.ArgumentParser(description="Fetch/list AutoEq ATH-M70x profile candidates.")
    parser.add_argument("--cache", type=Path, default=DEFAULT_CACHE)
    parser.add_argument("--model", default="Audio-Technica ATH-M70x")
    parser.add_argument("--preferred-source", default="oratory1990")
    parser.add_argument("--list", action="store_true", help="Only list detected candidates")
    parser.add_argument("--copy-to", type=Path, help="Copy selected AutoEq result files to this directory")
    args = parser.parse_args()

    ensure_autoeq(args.cache)
    candidates = find_candidates(args.cache, args.model)
    for index, candidate in enumerate(candidates, 1):
        print(f"{index}: {candidate.parent.relative_to(args.cache)}")

    if args.list:
        return

    selected = choose_candidate(candidates, args.preferred_source)
    print(f"Selected: {selected.relative_to(args.cache)}")

    if args.copy_to:
        args.copy_to.mkdir(parents=True, exist_ok=True)
        for source in selected.iterdir():
            if source.suffix.lower() in {".txt", ".csv", ".md", ".png"}:
                shutil.copy2(source, args.copy_to / source.name)


if __name__ == "__main__":
    main()
