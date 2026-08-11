#pragma once

#include "../DataHandler/ReadDataCsv.h"
#include "../EventQueue/EventQueue.h"
#include "../Portafolio/Portafolio.h"
#include "../SimulatedBroker/Broker.h"
#include "../StrategyPerformaceSaver/PerformanceSaver.h"
#include "../Strategies/Strategy.h"
#include "../types/Candle.h"
#include <memory>
#include <string>

class Backtester
{
private:
  readDataCsv readCsv;
  EventQueue queue;
  std::unique_ptr<Portafolio> portafolio;
  std::unique_ptr<Strategy> strategy;
  PerformanceSaver performanceSaver;
  SimulatedBroker broker;
  Candle latestCandle;
  bool LastCandle = false;
  std::string dataFileName;
  int rowsToRead;
  int processedBars{};

  void
  handleEvent(std::unique_ptr<Event> event);
  void handleDataEvent(const DataEvent &dataEvent);
  void handleOrderEvent(const OrderEvent &orderEvent);
  void handleFillEvent(const FillEvent &fillEvent);

public:
  Backtester(const std::string &fileName, const std::string &strategyLocation,
             const std::string &nameForPerfSaver, int rowsToRead = 100,
             double startingBalance = 100000.0,
             double periodsPerYear = 365.0,
             std::unique_ptr<Portafolio> portfolio = nullptr);
  Backtester(const std::string &fileName, const std::string &nameForPerfSaver,
             std::unique_ptr<Strategy> strategy, int rowsToRead = 100,
             double startingBalance = 100000.0,
             double periodsPerYear = 365.0,
             std::unique_ptr<Portafolio> portfolio = nullptr);

  bool Initialize();
  void Run();

  int GetProcessedBars() const;
  double GetLatestClose() const;

  const Portafolio &GetPortafolio() const;
};
