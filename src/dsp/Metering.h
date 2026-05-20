#pragma once

#include <cstddef>

namespace openref::dsp {

struct MeterState {
    float peak {};
    bool clipped {};
};

class PeakMeter {
public:
    void reset() noexcept;
    void process(const float* samples, std::size_t count) noexcept;
    [[nodiscard]] MeterState state() const noexcept { return current; }

private:
    MeterState current {};
};

} // namespace openref::dsp
