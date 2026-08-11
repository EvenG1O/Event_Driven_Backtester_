#include "PerformanceSaver.h"
#include <iomanip>
#include <stdexcept>
#include <utility>

std::filesystem::path PerformanceSaver::outputPath() const
{
    std::filesystem::path path = directory / fileName;

    if (path.extension().empty())
    {
        path.replace_extension(".csv");
    }

    return path;
}

std::string PerformanceSaver::positionSideToString(double quantity)
{
    if (quantity > 0.0)
    {
        return "BUY";
    }

    if (quantity < 0.0)
    {
        return "SELL";
    }

    return "FLAT";
}

void PerformanceSaver::writeHeader()
{
    file << "id,dateOpen,dateClose,open,high,low,close,volume,"
            "side,currentEquity,currentDrawdown,"
            "currentMaxDrawdown,currentClosePrice\n";
}

PerformanceSaver::PerformanceSaver(std::string fileName,
                                   std::filesystem::path directory)
    : directory(std::move(directory)), fileName(std::move(fileName))
{
    std::filesystem::create_directories(this->directory);

    const std::filesystem::path path = outputPath();
    if (std::filesystem::exists(path))
    {
        throw std::runtime_error("Performance file already exists: " +
                                 path.string());
    }

    file.open(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not create performance file: " +
                                 path.string());
    }

    writeHeader();
}

void PerformanceSaver::saveCurrentState(const Portafolio &portafolio,
                                        const Candle &candle)
{
    if (!file.is_open())
    {
        throw std::runtime_error("Performance file is not open");
    }

    const double marketPrice = candle.close;

    file << std::setprecision(17)
         << currentIndex++ << ','
         << candle.date_open << ','
         << candle.close_date << ','
         << candle.open << ','
         << candle.high << ','
         << candle.low << ','
          << candle.close << ','
          << candle.volume << ','
          << positionSideToString(portafolio.currentQuantity()) << ','
         << portafolio.equity(marketPrice) << ','
         << portafolio.getCurrentDrawdown(marketPrice) << ','
         << portafolio.getMaxDrawdown() << ','
         << marketPrice << '\n';

    if (!file.good())
    {
        throw std::runtime_error("Failed to write performance row");
    }
}

void PerformanceSaver::save(const Portafolio &portafolio, const Candle &candle)
{
    saveCurrentState(portafolio, candle);
}
