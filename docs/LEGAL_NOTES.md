# Legal notes

## Independence

OpenRef Monitor is an independent clean-room implementation. It must not use Sonarworks/SoundID code, UI, names, logos, calibration profiles, or proprietary target/correction curves.

Do not describe OpenRef Monitor as SoundID-compatible or Sonarworks-compatible.

## JUCE

JUCE is fetched from the official JUCE repository. JUCE licensing is GPLv3 or commercial. Closed-source commercial binary distribution requires a valid JUCE commercial license or another compliant licensing arrangement.

## AutoEq

AutoEq repository: https://github.com/jaakkopasanen/AutoEq

AutoEq repository license: MIT.

AutoEq also aggregates measurement data/results from multiple third-party sources. The source used for the bundled ATH-M70x profile is attributed in the generated JSON and README. Before shipping a commercial binary with bundled measurement-derived data, verify that the upstream measurement-source terms permit redistribution in your intended form.

If the redistribution status is unacceptable or uncertain, remove bundled profiles and require users to run `tools/fetch_autoeq_profile.py` and `tools/generate_profile.py` locally.

## Project-authored target/simulation data

Targets and translation simulations in `resources/targets` and `resources/simulations` are project-authored approximations for monitoring workflow checks. They are not copied from commercial products.
