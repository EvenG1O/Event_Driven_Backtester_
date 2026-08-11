#pragma once

#include "../Portafolio/Portafolio.h"
#include "../types/Candle.h"
#include <filesystem>
#include <fstream>
#include <string>

class PerformanceSaver
{
private:
    std::filesystem::path directory;
    std::string fileName;
    std::ofstream file;
    int currentIndex{};

    std::filesystem::path outputPath() const;
    static std::string positionSideToString(double quantity);
    void writeHeader();

public:
    explicit PerformanceSaver(std::string fileName,
                              std::filesystem::path directory = "Performance");

    void saveCurrentState(const Portafolio &portafolio, const Candle &candle);
    void save(const Portafolio &portafolio, const Candle &candle);
};
