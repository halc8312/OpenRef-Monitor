#include "ConvolutionEngine.h"

#include <algorithm>

namespace openref::dsp {

void ConvolutionEngine::loadImpulseResponse(std::vector<float> newIr)
{
    ir = std::move(newIr);
    delay.assign(ir.size(), 0.0f);
    writeIndex = 0;
    latency = static_cast<int>(ir.size() / 2);
}

void ConvolutionEngine::reset()
{
    std::fill(delay.begin(), delay.end(), 0.0f);
    writeIndex = 0;
}

void ConvolutionEngine::process(float* samples, std::size_t count)
{
    if (ir.empty() || delay.empty()) {
        return;
    }

    for (std::size_t i = 0; i < count; ++i) {
        auto& sample = samples[i];
        delay[writeIndex] = sample;
        auto read = writeIndex;
        auto acc = 0.0f;
        for (const auto tap : ir) {
            acc += tap * delay[read];
            read = read == 0 ? delay.size() - 1 : read - 1;
        }
        sample = acc;
        writeIndex = (writeIndex + 1) % delay.size();
    }
}

} // namespace openref::dsp
