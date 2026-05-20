# OpenRef Monitor plan

## Clean-room boundary

OpenRef Monitor is an independent monitor-correction plugin. It must not copy Sonarworks/SoundID source code, UI layout, naming, logos, proprietary headphone profiles, or proprietary target curves. Public documentation was reviewed only to understand broad user expectations for monitor-correction tools.

## Public feature references reviewed

Sonarworks public support/product pages describe these broad concepts:

- DAW plugin usage for headphone/speaker calibration.
- Loading calibration profiles/presets.
- Frequency-response curve display toggles.
- Filter modes such as zero-latency and linear-phase options.
- Safe Headroom that reduces output by the highest correction boost to avoid clipping.
- Output gain controls and metering.
- Custom target adjustment.
- Translation checks for alternate playback devices.
- Mono/output-panel utilities.

OpenRef implements these as original UX/DSP choices, with different terminology where needed and project-authored resources.

## v1 goals

- Windows x64 VST3 plugin, optional standalone target.
- Cubase Pro 14 Control Room Monitor Insert workflow.
- Audio-Technica ATH-M70x profile generated from public AutoEq data or user-imported CSV/PEQ files.
- Zero-latency PEQ correction with RBJ biquad filters.
- Safe Headroom, Dry/Wet, Amount, output gain, mono check.
- Basic response graph and meters.
- Auto-bypass during offline render ON by default.
- JSON profile/target/simulation formats.
- Unit tests and pluginval validation path.

## v1.5 goals

- Minimum-phase FIR and linear-phase FIR modes.
- Full custom target curve editor.
- Translation Check DSP routing.
- L/R individual profile import.
- Background DSP-state rebuild and click-free crossfade swaps.

## v2 goals

- Standalone app.
- WASAPI/ASIO output path for MOTU M2.
- Equalizer APO export.
- Virtual Audio Cable/systemwide workflow documentation.

## v3 goals

- Measurement import.
- miniDSP EARS/REW CSV import.
- User-measured ATH-M70x correction.
- Optional speaker/room correction path.

## AutoEq ATH-M70x finding

AutoEq includes ATH-M70x results from multiple sources:

- `results/oratory1990/over-ear/Audio-Technica ATH-M70x`
- `results/crinacle/GRAS 43AG-7 over-ear/Audio-Technica ATH-M70x`
- `results/Rtings/HMS II.3 over-ear/Audio-Technica ATH-M70x`
- `results/Innerfidelity/over-ear/Audio-Technica ATH-M70x`

The bundled profile currently uses the AutoEq recommended result, which points to the oratory1990 path in `results/README.md`. AutoEq itself is MIT licensed; measurement-source redistribution terms require review before commercial release.

## Acceptance checklist

- Build VST3 on Visual Studio 2022 x64.
- Validate with pluginval.
- Test 44.1/48/96/192 kHz.
- Test block sizes 32/64/128/512/1024.
- Confirm no file I/O/allocation in steady-state audio processing for v1 DSP path.
- Confirm Cubase Pro 14 can load the VST3 in Control Room Monitor Insert.
- Confirm offline-render auto-bypass behavior and user override.
