#pragma once

#include <cmath>

class AxisInputProcessor {
    float val{0};
    float exp_base;
    float div;
public:
    AxisInputProcessor(float exp_base = 1.001f, float div = 2.0f);
    void clear();
    int calc(float val_in);
};
