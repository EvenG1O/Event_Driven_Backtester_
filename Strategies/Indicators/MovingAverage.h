#pragma once

#include <queue>
#include <stdexcept>

class MovingAverage
{

private:
    std::queue<float> values;
    int Window{};
    float currentSum{};
    float pop();

public:
    MovingAverage(int MovingAverageWindow) : Window(MovingAverageWindow)
    {
        if (Window <= 0)
        {
            throw std::invalid_argument(
                "Moving average window must be greater than zero");
        }
    }

    float push(const float value);
};
