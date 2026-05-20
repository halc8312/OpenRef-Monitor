#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    parser = argparse.ArgumentParser(description="Validate OpenRef Monitor calibration profile JSON.")
    parser.add_argument("profile", type=Path)
    args = parser.parse_args()

    data = json.loads(args.profile.read_text(encoding="utf-8"))
    require(data.get("schemaVersion") == 1, "schemaVersion must be 1")
    require(data.get("id"), "id is required")
    require(data.get("manufacturer"), "manufacturer is required")
    require(data.get("model"), "model is required")
    source = data.get("source", {})
    require(source.get("name") and source.get("url") and source.get("license"), "source attribution is incomplete")
    left = data.get("channels", {}).get("left", {})
    require(left.get("parametricEq"), "left.parametricEq is required")
    for index, item in enumerate(left["parametricEq"], 1):
        require(item.get("type") in {"peaking", "lowshelf", "highshelf", "lowpass", "highpass"}, f"filter {index} type invalid")
        require(float(item.get("freqHz", 0)) > 0, f"filter {index} freqHz invalid")
        require(float(item.get("q", 0)) > 0, f"filter {index} q invalid")
    print(f"valid: {args.profile}")


if __name__ == "__main__":
    main()
