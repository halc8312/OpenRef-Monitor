#pragma once

namespace openref::dsp {

class SmoothedBypass {
public:
    void prepare(double sampleRate, double rampMs = 20.0) noexcept;
    void setEnabled(bool enabled) noexcept;
    [[nodiscard]] float next() noexcept;
    [[nodiscard]] bool targetEnabled() const noexcept { return target > 0.5f; }

private:
    float current { 1.0f };
    float target { 1.0f };
    float step { 1.0f };
};

} // namespace openref::dsp
