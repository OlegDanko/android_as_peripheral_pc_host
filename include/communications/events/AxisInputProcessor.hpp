#pragma once

#include <cmath>

class AxisInputProcessor {
    float val{0};
    float exp_base;
    float div;
public:
    AxisInputProcessor(float exp_base = 1.00002f, float div = 200.0f);
    void clear();
    int calc(float val_in);
};
