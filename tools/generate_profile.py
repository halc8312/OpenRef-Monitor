#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from autoeq_common import parse_autoeq_csv, parse_graphic_eq, parse_parametric_eq, profile_json, write_json


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert AutoEq PEQ/GraphicEQ/CSV files to OpenRef Monitor JSON.")
    parser.add_argument("--parametric-eq", type=Path, required=True)
    parser.add_argument("--graphic-eq", type=Path)
    parser.add_argument("--csv", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--source-url", default="https://github.com/jaakkopasanen/AutoEq")
    parser.add_argument("--source-license", default="MIT (AutoEq repository); verify measurement-source terms before redistribution")
    parser.add_argument("--measurement-source", default="oratory1990 via AutoEq")
    args = parser.parse_args()

    preamp, filters = parse_parametric_eq(args.parametric_eq)
    if args.csv:
        raw, correction, _target = parse_autoeq_csv(args.csv)
    elif args.graphic_eq:
        correction = parse_graphic_eq(args.graphic_eq)
        raw = [{"hz": point["hz"], "db": 0.0} for point in correction]
    else:
        correction = [{"hz": item.freq_hz, "db": item.gain_db} for item in filters]
        raw = [{"hz": item.freq_hz, "db": 0.0} for item in filters]

    write_json(
        args.output,
        profile_json(
            preamp_db=preamp,
            filters=filters,
            raw=raw,
            correction=correction,
            source_url=args.source_url,
            source_license=args.source_license,
            measurement_source=args.measurement_source,
        ),
    )
    print(args.output)


if __name__ == "__main__":
    main()
