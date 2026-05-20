# Profile format

OpenRef Monitor calibration profiles are JSON documents.

```json
{
  "schemaVersion": 1,
  "id": "audio-technica-ath-m70x-autoeq",
  "manufacturer": "Audio-Technica",
  "model": "ATH-M70x",
  "type": "headphone",
  "source": {
    "name": "AutoEq",
    "url": "https://github.com/jaakkopasanen/AutoEq",
    "license": "MIT",
    "measurementSource": "oratory1990 via AutoEq",
    "generatedAt": "2026-05-20T00:00:00Z"
  },
  "recommendedPreampDb": -3.6,
  "channels": {
    "left": {
      "frequencyResponse": [{ "hz": 20.0, "db": 0.0 }],
      "correctionCurve": [{ "hz": 20.0, "db": 0.0 }],
      "parametricEq": [
        { "type": "peaking", "freqHz": 1000.0, "gainDb": 0.0, "q": 1.0 }
      ]
    },
    "right": { "sameAs": "left" }
  }
}
```

## Filter types

- `peaking`
- `lowshelf`
- `highshelf`
- `lowpass`
- `highpass`

## User locations

Profiles:

```text
%APPDATA%\OpenRefMonitor\profiles\
```

Presets:

```text
%APPDATA%\OpenRefMonitor\presets\
```

## Import path

`tools/generate_profile.py` converts AutoEq `ParametricEQ.txt`, `GraphicEQ.txt`, and CSV exports. Future importers should support separate left/right measured CSV files.
