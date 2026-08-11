#include "../Backtester/Backtester.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
std::vector<std::string> splitCsvLine(const std::string &line)
{
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ','))
    {
        fields.push_back(field);
    }

    return fields;
}
}

int main()
{
    const std::filesystem::path inputPath =
        "moving_average_backtest_data.csv";
    const std::filesystem::path outputPath =
        std::filesystem::path("Performance") /
        "moving_average_backtest.csv";

    std::filesystem::remove(inputPath);
    std::filesystem::remove(outputPath);
    std::filesystem::create_directories(outputPath.parent_path());

    {
        std::ofstream input(inputPath);
        input << "open_time,open,high,low,close,volume,close_time,"
                 "quote_asset_volume,number_of_trades,"
                 "taker_buy_base_asset_volume,taker_buy_quote_asset_volume,"
                 "ignore\n";
        input << "2026-01-01,3,4,2,3,10,2026-01-01T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-02,2,3,1,2,10,2026-01-02T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-03,1,2,0.5,1,10,2026-01-03T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-04,3,4,2,3,10,2026-01-04T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-05,4,5,3,4,10,2026-01-05T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-06,1,2,0.5,1,10,2026-01-06T23:59:59,"
                 "0,0,0,0,0\n";
    }

    double sortinoRatio = 0.0;
    {
        Backtester backtester(inputPath.string(), "moving_average",
                              "moving_average_backtest", 10);
        assert(backtester.Initialize());
        backtester.Run();

        assert(backtester.GetProcessedBars() == 6);
        assert(backtester.GetPortafolio().activeTradeCount() == 0);
        assert(std::isfinite(backtester.GetPortafolio().getSharpeRatio()));
        assert(std::isfinite(backtester.GetPortafolio().getSortinoRatio()));
        sortinoRatio = backtester.GetPortafolio().getSortinoRatio();
    }

    std::vector<std::string> lines;
    {
        std::ifstream output(outputPath);
        assert(output.is_open());

        for (std::string line; std::getline(output, line);)
        {
            if (!line.empty())
            {
                lines.push_back(line);
            }
        }
    }

    // Header, six candle rows, and the final position-closing row.
    assert(lines.size() == 8);

    const std::vector<std::string> firstCandle = splitCsvLine(lines[1]);
    const std::vector<std::string> bullishCrossover =
        splitCsvLine(lines[5]);
    const std::vector<std::string> bearishCrossover =
        splitCsvLine(lines[6]);
    const std::vector<std::string> finalRow = splitCsvLine(lines.back());

    assert(firstCandle[8] == "FLAT");
    assert(bullishCrossover[8] == "BUY");
    assert(bearishCrossover[8] == "SELL");
    assert(finalRow[8] == "FLAT");

    assert(finalRow.size() >= 13);
    assert(std::abs(std::stod(finalRow[9]) - 99925.0) < 1e-9);

    const double expectedSortino =
        ((-0.00075 / 6.0) /
         std::sqrt((0.00075 * 0.00075) / 6.0)) *
        std::sqrt(365.0);
    assert(std::abs(sortinoRatio - expectedSortino) < 1e-9);

    std::filesystem::remove(inputPath);
    std::filesystem::remove(outputPath);

    return 0;
}
