# DSP design

## Processing order

```text
Input
  -> Input meter
  -> Safe Headroom auto pre-gain
  -> Headphone correction
  -> Target modifiers
  -> Optional translation simulation
  -> Dry/Wet mix with latency compensation
  -> Output gain
  -> Output meter / clip detection
  -> Output
```

## v1 PEQ mode

Zero Latency mode applies AutoEq-style parametric filters as cascaded RBJ biquads. Supported types:

- Peaking
- Low shelf
- High shelf
- Low pass
- High pass

Coefficients are recalculated on sample-rate changes. Target modifiers, bass/treble tilt, and translation-check filters are preallocated and updated from parameter state without file I/O or dynamic allocation in the audio callback. Steady-state `processBlock` must avoid file I/O, locks, and DSP-vector resizing.

## Target modifiers and translation checks

The bundled `Neutral Harman-like` target adds a gentle low-shelf lift and high-shelf trim before user tilt controls. `Flat Custom` leaves the target modifier flat. Bass Tilt and Treble Tilt add low/high shelf filters at 120 Hz and 8 kHz.

Translation checks use independent OpenRef simulation resources. Selecting a simulation inserts its filter set after headphone correction and target modifiers. Simulations marked `mono` also force mono summing while active, even if the global Mono Check toggle is off.

## Safe Headroom

The engine evaluates the active filter response over a dense 20 Hz-20 kHz log grid:

```text
autoPreampDb = -max(0, maxBoostDb) - 1 dB
```

Output gain remains separate from auto pre-gain.

## Bypass/offline render

`AudioProcessor::isNonRealtime()` is checked during processing. If `autoBypassOffline` is enabled, correction is bypassed for offline export. User bypass and auto-bypass should use a short crossfade to prevent clicks.

## FIR modes

Minimum-phase and linear-phase FIR interfaces are scaffolded. v1.5 will replace the placeholder FIR implementation with FFT/IFFT, log-magnitude smoothing, cepstral minimum-phase reconstruction, and partitioned convolution or JUCE `dsp::Convolution`.

Linear-phase latency must be reported through `setLatencySamples()` and displayed in the UI.

## Realtime safety rules

- No `new/delete`, vector resize, file loading, or mutex locking in steady-state audio processing.
- Build immutable DSP state off the audio thread.
- Atomically publish/crossfade new state.
- Use denormal protection.
- Clamp or bypass NaN/Inf output.
