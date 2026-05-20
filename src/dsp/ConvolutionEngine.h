#pragma once

#include <cstddef>
#include <vector>

namespace openref::dsp {

class ConvolutionEngine {
public:
    void loadImpulseResponse(std::vector<float> newIr);
    void reset();
    void process(float* samples, std::size_t count);
    [[nodiscard]] int latencySamples() const noexcept { return latency; }

private:
    std::vector<float> ir;
    std::vector<float> delay;
    std::size_t writeIndex {};
    int latency {};
};

} // namespace openref::dsp
