# OpenRef Monitor

OpenRef Monitor is a clean-room C++20/JUCE VST3 headphone-monitor correction plugin focused on Windows 11 x64, Cubase Pro 14, MOTU M2, and Audio-Technica ATH-M70x workflows.

This project is independent software. It is not affiliated with, endorsed by, compatible with, or based on Sonarworks or SoundID Reference. No Sonarworks/SoundID code, UI, naming, logos, calibration profiles, or proprietary correction curves are used.

## Current status

This repository contains the initial buildable skeleton and v1 DSP/profile foundations:

- JUCE 8 CMake VST3 + standalone target.
- Zero-latency PEQ correction using RBJ biquad filters.
- ATH-M70x profile generated from AutoEq public results.
- Safe Headroom analysis, Dry/Wet, Amount, output gain, mono check, bass/treble tilt, metering, and offline-render auto-bypass.
- JSON profile/target/simulation resources, with bundled target curves and independent translation checks wired into the realtime PEQ path.
- Python tools for fetching/converting/validating AutoEq profiles.
- Unit tests for DSP/profile/tool basics.

Minimum-phase FIR, linear-phase FIR, full custom curve editing, and hardened audio-thread crossfade state swaps are planned v1.5 work.

## License and legal notes

- Project source is MIT licensed unless otherwise noted.
- JUCE is used via CMake FetchContent. JUCE is available under GPLv3 or commercial licensing. If distributing closed-source binaries, obtain and comply with a JUCE commercial license. If using JUCE under GPLv3, your distribution must comply with GPLv3.
- The bundled ATH-M70x profile is generated from AutoEq repository data. AutoEq is MIT licensed, but it aggregates measurements/results from sources such as oratory1990, crinacle, Rtings, Innerfidelity, and others. Review upstream measurement-source terms before commercial redistribution.
- oratory1990-derived data is attributed in this README, `docs/LEGAL_NOTES.md`, and the plugin UI/About text. If redistribution terms are not acceptable for your product, remove `resources/profiles/audio-technica-ath-m70x.autoeq.json` and use the import tools so users generate profiles locally.

## Attribution

Bundled ATH-M70x profile:

- Source: AutoEq, https://github.com/jaakkopasanen/AutoEq
- AutoEq license: MIT
- Measurement/result source: oratory1990 over-ear ATH-M70x result via AutoEq recommended results
- Generated file: `resources/profiles/audio-technica-ath-m70x.autoeq.json`

## Windows build

Install Visual Studio 2022 with the Desktop development with C++ workload and CMake.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
cmake --install build --config Release
```

Recommended VST3 install location:

```text
C:\Program Files\Common Files\VST3\OpenRefMonitor\OpenRef Monitor.vst3
```

For validation, run pluginval against the generated VST3:

```powershell
pluginval --strictness-level 10 "build\OpenRefMonitor_artefacts\Release\VST3\OpenRef Monitor.vst3"
```

## Linux/macOS developer checks

Plugin builds may need platform GUI/audio dependencies, but core tests can be run without building JUCE:

```bash
cmake -S . -B build -DOPENREF_BUILD_PLUGIN=OFF -DOPENREF_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/validate_profile.py resources/profiles/audio-technica-ath-m70x.autoeq.json
```

## Cubase Pro 14 usage

Use OpenRef Monitor for monitoring, not printing correction into exported audio:

1. Enable Control Room in Cubase Pro 14.
2. Assign MOTU M2 outputs to the monitor bus.
3. Insert `OpenRef Monitor` on the Control Room Monitor Insert.
4. Select `Audio-Technica ATH-M70x (AutoEq)`.
5. Keep `Safe Headroom` and `Auto-bypass offline render` ON by default.
6. Do not place monitor correction on mix tracks. If inserted on Stereo Out, verify auto-bypass behavior before exporting.

See `docs/USAGE_CUBASE.md` for details.

## Profile generation

List AutoEq candidates:

```bash
python3 tools/fetch_autoeq_profile.py --list
```

Generate OpenRef JSON from AutoEq files:

```bash
python3 tools/generate_profile.py \
  --parametric-eq "AutoEq/results/oratory1990/over-ear/Audio-Technica ATH-M70x/Audio-Technica ATH-M70x ParametricEQ.txt" \
  --graphic-eq "AutoEq/results/oratory1990/over-ear/Audio-Technica ATH-M70x/Audio-Technica ATH-M70x GraphicEQ.txt" \
  --csv "AutoEq/results/oratory1990/over-ear/Audio-Technica ATH-M70x/Audio-Technica ATH-M70x.csv" \
  --output resources/profiles/audio-technica-ath-m70x.autoeq.json
```

User profiles are loaded from:

```text
%APPDATA%\OpenRefMonitor\profiles\
```

User presets are stored under:

```text
%APPDATA%\OpenRefMonitor\presets\
```

## MOTU M2 recommendations

- Use the native MOTU ASIO driver.
- Set Cubase and the MOTU driver to the same sample rate.
- Recommended sample rates: 44.1, 48, 88.2, 96, or 192 kHz.
- Keep interface/headphone gain conservative when testing correction curves.

## Known limitations

- Average headphone correction is not individual headphone calibration.
- ATH-M70x high-frequency response depends strongly on fit, pad condition, ear shape, and placement.
- OpenRef Monitor does not promise the same sonic result as any commercial correction product.
- FIR modes are scaffolded for v1.5; v1 focuses on zero-latency PEQ.