#!/usr/bin/env python3
from __future__ import annotations

import csv
import json
import re
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Iterable


FILTER_RE = re.compile(
    r"Filter\s+(?P<index>\d+):\s+ON\s+(?P<type>\w+)\s+Fc\s+(?P<freq>[-+]?\d+(?:\.\d+)?)\s+Hz\s+Gain\s+(?P<gain>[-+]?\d+(?:\.\d+)?)\s+dB\s+Q\s+(?P<q>[-+]?\d+(?:\.\d+)?)",
    re.IGNORECASE,
)
PREAMP_RE = re.compile(r"Preamp:\s*(?P<preamp>[-+]?\d+(?:\.\d+)?)\s*dB", re.IGNORECASE)


@dataclass(frozen=True)
class Filter:
    type: str
    freq_hz: float
    gain_db: float
    q: float


def normalise_filter_type(value: str) -> str:
    value = value.lower()
    if value in {"pk", "peak", "peaking"}:
        return "peaking"
    if value in {"ls", "lsc", "lowshelf", "low_shelf"}:
        return "lowshelf"
    if value in {"hs", "hsc", "highshelf", "high_shelf"}:
        return "highshelf"
    if value in {"lp", "lowpass", "low_pass"}:
        return "lowpass"
    if value in {"hp", "highpass", "high_pass"}:
        return "highpass"
    raise ValueError(f"unsupported filter type: {value}")


def parse_parametric_eq(path: Path) -> tuple[float, list[Filter]]:
    text = path.read_text(encoding="utf-8")
    preamp_match = PREAMP_RE.search(text)
    preamp = float(preamp_match.group("preamp")) if preamp_match else 0.0
    filters = [
        Filter(
            type=normalise_filter_type(match.group("type")),
            freq_hz=float(match.group("freq")),
            gain_db=float(match.group("gain")),
            q=float(match.group("q")),
        )
        for match in FILTER_RE.finditer(text)
    ]
    if not filters:
        raise ValueError(f"no AutoEq filters found in {path}")
    return preamp, filters


def parse_graphic_eq(path: Path) -> list[dict[str, float]]:
    text = path.read_text(encoding="utf-8").strip()
    if not text.startswith("GraphicEQ:"):
        raise ValueError(f"not an AutoEq GraphicEQ file: {path}")
    points: list[dict[str, float]] = []
    for pair in text.removeprefix("GraphicEQ:").split(";"):
        pair = pair.strip()
        if not pair:
            continue
        freq, gain = pair.split()
        points.append({"hz": float(freq), "db": float(gain)})
    return points


def parse_autoeq_csv(path: Path) -> tuple[list[dict[str, float]], list[dict[str, float]], list[dict[str, float]]]:
    raw: list[dict[str, float]] = []
    correction: list[dict[str, float]] = []
    target: list[dict[str, float]] = []
    with path.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            hz = float(row["frequency"])
            raw.append({"hz": hz, "db": float(row.get("raw") or 0.0)})
            correction.append({"hz": hz, "db": float(row.get("equalization") or 0.0)})
            target.append({"hz": hz, "db": float(row.get("target") or 0.0)})
    return raw, correction, target


def profile_json(
    *,
    preamp_db: float,
    filters: Iterable[Filter],
    raw: list[dict[str, float]],
    correction: list[dict[str, float]],
    source_url: str,
    source_license: str,
    measurement_source: str,
) -> dict:
    return {
        "schemaVersion": 1,
        "id": "audio-technica-ath-m70x-autoeq",
        "manufacturer": "Audio-Technica",
        "model": "ATH-M70x",
        "type": "headphone",
        "source": {
            "name": "AutoEq",
            "url": source_url,
            "license": source_license,
            "measurementSource": measurement_source,
            "generatedAt": datetime.now(timezone.utc).isoformat(),
        },
        "recommendedPreampDb": preamp_db,
        "channels": {
            "left": {
                "frequencyResponse": raw,
                "correctionCurve": correction,
                "parametricEq": [
                    {
                        "type": item.type,
                        "freqHz": item.freq_hz,
                        "gainDb": item.gain_db,
                        "q": item.q,
                    }
                    for item in filters
                ],
            },
            "right": {"sameAs": "left"},
        },
    }


def write_json(path: Path, data: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
