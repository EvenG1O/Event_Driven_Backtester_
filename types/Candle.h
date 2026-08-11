#pragma once
#include <string>

struct Candle {
    std::string date_open;
    double open{};
    double close{};
    double high{};
    double low{};
    double volume{};
    std::string close_date;
};

using candle = Candle;
