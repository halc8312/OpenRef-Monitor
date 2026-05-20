#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description="Plot OpenRef Monitor profile curves to PNG.")
    parser.add_argument("profile", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    import matplotlib.pyplot as plt

    data = json.loads(args.profile.read_text(encoding="utf-8"))
    left = data["channels"]["left"]
    for key, label in [("frequencyResponse", "Raw"), ("correctionCurve", "Correction")]:
        points = left.get(key, [])
        if points:
            plt.semilogx([p["hz"] for p in points], [p["db"] for p in points], label=label)
    plt.xlim(20, 20000)
    plt.ylim(-18, 18)
    plt.grid(True, which="both")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("dB")
    plt.title(data.get("model", "OpenRef profile"))
    plt.legend()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(args.output, dpi=160)
    print(args.output)


if __name__ == "__main__":
    main()
