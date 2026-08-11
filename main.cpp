#include "Backtester/Backtester.h"
#include "listStrategies/listStrategies.h"
#include <iostream>

int main(int argc, char *argv[])
{
  if (ListStrats::run(argc, argv))
  {
    return 0;
  }

  if (argc < 4 || argc > 7)
  {
    std::cerr << "Usage: backtester <csv-file> <strategy-name> "
                 "<nameForPerformanceFile> [rows-per-batch] "
                 "[starting-balance] [periods-per-year]\n"
              << "       backtester --list-strategies"
              << std::endl;
    return 1;
  }

  int rowsPerBatch = 100;
  if (argc >= 5)
  {
    rowsPerBatch = std::stoi(argv[4]);
  }

  double startingBalance = 100000.0;
  if (argc >= 6)
  {
    startingBalance = std::stod(argv[5]);
  }

  double periodsPerYear = 365.0;
  if (argc >= 7)
  {
    periodsPerYear = std::stod(argv[6]);
  }

  Backtester backtester(argv[1], argv[2], argv[3], rowsPerBatch,
                        startingBalance, periodsPerYear);
  if (!backtester.Initialize())
  {
    return 1;
  }

  backtester.Run();

  const Portafolio &p = backtester.GetPortafolio();

  p.portafolioPerformace();

  std::cout << "Backtester ready. Bars: " << backtester.GetProcessedBars()
            << " Active positions: "
            << backtester.GetPortafolio().activeTradeCount()
            << std::endl;
  return 0;
}
