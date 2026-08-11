#include "MovingAverage.h"

float MovingAverage::pop()
{
    if (values.empty())
    {
        return 0.0;
    }

    const float value = values.front();
    values.pop();
    currentSum -= value;
    return value;
}

float MovingAverage::push(const float value)
{
    values.push(value);
    currentSum += value;

    if (static_cast<int>(values.size()) > Window)
    {
        pop();
    }

    return currentSum / static_cast<float>(values.size());
}
