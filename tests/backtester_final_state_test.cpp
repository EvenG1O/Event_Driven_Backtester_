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
    const std::filesystem::path inputPath = "backtester_final_state_data.csv";
    const std::filesystem::path outputPath =
        std::filesystem::path("Performance") / "backtester_final_state.csv";

    std::filesystem::remove(inputPath);
    std::filesystem::remove(outputPath);
    std::filesystem::create_directories(outputPath.parent_path());

    {
        std::ofstream input(inputPath);
        input << "open_time,open,high,low,close,volume,close_time,"
                 "quote_asset_volume,number_of_trades,"
                 "taker_buy_base_asset_volume,taker_buy_quote_asset_volume,"
                 "ignore\n";
        input << "2026-01-01,50,55,45,50,10,2026-01-01T23:59:59,"
                 "0,0,0,0,0\n";
        input << "2026-01-02,50,65,48,60,10,2026-01-02T23:59:59,"
                 "0,0,0,0,0\n";
    }

    {
        Backtester backtester(inputPath.string(), "buy_and_hold",
                              "backtester_final_state", 10);
        assert(backtester.Initialize());
        backtester.Run();
        assert(backtester.GetPortafolio().activeTradeCount() == 0);
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

    assert(lines.size() == 4);

    const std::vector<std::string> finalRow = splitCsvLine(lines.back());
    assert(finalRow.size() >= 13);
    assert(finalRow[8] == "FLAT");
    assert(std::abs(std::stod(finalRow[9]) - 100020.0) < 1e-9);

    std::filesystem::remove(inputPath);
    std::filesystem::remove(outputPath);

    return 0;
}
