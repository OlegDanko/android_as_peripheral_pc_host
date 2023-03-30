#include <communications/events/AxisInputProcessor.hpp>

AxisInputProcessor::AxisInputProcessor(float exp_base, float div) : exp_base(exp_base), div(div) {}

void AxisInputProcessor::clear() {
    val = 0;
}

int AxisInputProcessor::calc(float val_in) {
    float val_change = val_in * std::pow(exp_base, std::abs(val_in));
    val += val_change;

    float val_out = std::round(val / div);
    val -= val_out * div;

    return (int)val_out;
}

