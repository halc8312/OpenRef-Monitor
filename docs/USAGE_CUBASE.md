# Cubase Pro 14 usage

## Goal

Use OpenRef Monitor as monitor correction for Audio-Technica ATH-M70x through MOTU M2. The correction should be heard while mixing/mastering, but should not be printed into exported audio.

## Enable Control Room

1. Open **Studio > Audio Connections**.
2. Open the **Control Room** tab.
3. Enable Control Room.
4. Add a stereo Monitor bus if one does not exist.
5. Assign the monitor bus outputs to the MOTU M2 main output pair.

## Insert OpenRef Monitor

1. Open the Control Room panel.
2. Locate the Monitor Inserts section.
3. Insert **OpenRef Monitor**.
4. Select the `Audio-Technica ATH-M70x (AutoEq)` profile.
5. Select `Neutral Harman-like` or `Flat Custom` target.
6. Start with:
   - Correction: ON
   - Mode: Zero Latency
   - Amount: 100%
   - Dry/Wet: 100%
   - Safe Headroom: ON
   - Auto-bypass offline render: ON

## MOTU M2 settings

- Use the MOTU ASIO driver.
- Match Cubase project sample rate and MOTU driver sample rate.
- Keep the M2 headphone gain low at first, then increase gradually.
- If Safe Headroom lowers plugin level, prefer raising the monitor/interface gain rather than disabling headroom.

## Translation checks

Translation Check profiles are original broad simulations for mix sanity checks. They are not copies of any commercial product presets.

Use them briefly to evaluate vocal balance, bass dependence, mono compatibility, and harshness. Disable them for final tonal decisions.

## Export safety

Recommended: insert OpenRef Monitor only on Control Room Monitor Inserts.

If you put the plugin on Stereo Out:

- Keep **Auto-bypass offline render** ON unless you intentionally want correction printed.
- Test one short export and compare with realtime playback before relying on it.
- If Auto-bypass is OFF, correction will be included in exported audio.

## Troubleshooting

- If the plugin is missing, confirm it is installed under `C:\Program Files\Common Files\VST3\OpenRefMonitor\OpenRef Monitor.vst3` and rescan VST3 plugins.
- If the level drops, Safe Headroom is reducing gain to avoid clipping from EQ boosts.
- If linear-phase mode is used in future versions, Cubase PDC should compensate reported latency; watch the plugin latency display.
