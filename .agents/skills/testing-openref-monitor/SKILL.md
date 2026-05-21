---
name: testing-openref-monitor
description: Test OpenRef Monitor JUCE standalone/VST3 changes, especially UI controls backed by DSP parameters.
---

# Testing OpenRef Monitor

Use this skill when validating OpenRef Monitor UI, DSP wiring, or build changes in `halc8312/OpenRef-Monitor`.

## Devin Secrets Needed

- None for local Linux standalone, CTest, CMake build, or GitHub PR checks.
- Windows/Cubase validation requires external user hardware/software access, not a Devin secret.

## Local verification commands

Run from the repo root:

```bash
cmake -S . -B build-core -DOPENREF_BUILD_PLUGIN=OFF -DOPENREF_BUILD_TESTS=ON
cmake --build build-core -j2
ctest --test-dir build-core --output-on-failure
cmake -S . -B build-plugin -DOPENREF_BUILD_PLUGIN=ON -DOPENREF_BUILD_TESTS=OFF
cmake --build build-plugin -j2
```

Expected plugin artifacts include:

- `build-plugin/OpenRefMonitor_artefacts/Standalone/OpenRef Monitor`
- `build-plugin/OpenRefMonitor_artefacts/VST3/OpenRef Monitor.vst3`

## Standalone UI smoke test

Launch the standalone app:

```bash
"$PWD/build-plugin/OpenRefMonitor_artefacts/Standalone/OpenRef Monitor"
```

If the VM has no real audio device, an ALSA `/dev/snd/seq` warning may appear. Treat this as an environment warning if the UI still launches and remains interactive.

Record GUI tests when validating user-visible UI changes. Verify:

- Profile selector shows `ATH-M70x AutoEq`.
- Target selector includes `Neutral Harman-like` and `Flat Custom`.
- Translation Check includes `Off`, `Small Phone Speaker`, `Laptop Speaker`, `Basic Car Check`, `Mono Midrange`, `Bass-limited Check`, and `Bright Earbuds Check`.
- Realtime slider labels are visible and associated with textboxes: `Amount`, `Dry/Wet`, `Output`, `Bass Tilt`, `Treble Tilt`.
- Editing a tilt textbox, e.g. Bass Tilt to `3.00`, persists without UI freeze/crash.
- Selecting `Mono Midrange` persists visibly after closing the dropdown.
- Warning/attribution text remains visible: monitoring-only warning and AutoEq attribution.

## DSP-backed assertions

For Target/Tilt/Translation changes, rely on both UI and core tests. The correction-engine tests should cover:

- Extra filters affect measured response and processing.
- Mono translation simulation collapses opposing stereo samples to mono.
- Safe headroom calculations still pass.

## Reporting caveats

Always mark these as untested unless the user or CI provides the required environment:

- Cubase Pro Control Room loading on Windows 11.
- MOTU M2 hardware monitoring.
- pluginval validation.
- Real audio audition of translation simulations in Cubase.

When testing an open PR, post a single PR comment with concise pass/fail/untested bullets, recording link, screenshots, and the Devin session URL.
