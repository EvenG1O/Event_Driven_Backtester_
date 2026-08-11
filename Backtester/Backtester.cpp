#include "Backtester.h"
#include "../DataEvent/DataEvent.h"
#include "../FillEvent/FilleEvent.h"
#include "../OrderEvent/OrderEvent.h"
#include "../Portafolio/NaivePortafolio.h"
#include "../strategyBase/StrategyRegistry.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

Backtester::Backtester(const std::string &fileName,
                       const std::string &strategyLocation,
                       const std::string &nameForPerfSaver, int rowsToRead,
                       double startingBalance, double periodsPerYear,
                       std::unique_ptr<Portafolio> portfolio)
    : Backtester(fileName, nameForPerfSaver,
                 StrategyRegistry::instance().create(strategyLocation),
                 rowsToRead, startingBalance, periodsPerYear,
                 std::move(portfolio)) {}

Backtester::Backtester(const std::string &fileName,
                       const std::string &nameForPerfSaver,
                       std::unique_ptr<Strategy> strategy,
                       int rowsToRead, double startingBalance,
                       double periodsPerYear,
                       std::unique_ptr<Portafolio> portfolio)
    : portafolio(std::move(portfolio)),
      strategy(std::move(strategy)), performanceSaver(nameForPerfSaver),
      dataFileName(fileName), rowsToRead(rowsToRead)
{
  if (!this->strategy)
  {
    throw std::invalid_argument("Backtester strategy cannot be null");
  }

  if (!this->portafolio)
  {
    this->portafolio =
        std::make_unique<NaivePortafolio>(startingBalance, periodsPerYear);
  }
}

bool Backtester::Initialize()
{
  readCsv.setAmountofRowsToRead(rowsToRead);
  return readCsv.openFile(dataFileName);
}

void Backtester::Run()
{
  while (std::optional<DataEvent> currentBar = readCsv.newBar())
  {
    queue.pushEvent(std::make_unique<DataEvent>(*currentBar));

    while (!queue.empty())
    {
      handleEvent(queue.pop());
    }

    // Update all  the performance  metrics  based after every  candle and events
    portafolio->updateMetrics(latestCandle.close);
    performanceSaver.saveCurrentState(*portafolio, latestCandle);
  }

  // close any remaining position at the final available candle and
  if (LastCandle)
  {
    portafolio->closeOpenPosition(latestCandle);
    portafolio->updateMetrics(latestCandle.close);
    performanceSaver.saveCurrentState(*portafolio, latestCandle);
  }

  std::cout << "Backtester done" << std::endl;
}

void Backtester::handleEvent(std::unique_ptr<Event> event)
{
  if (!event)
  {
    return;
  }

  if (event->type == EventType::Data)
  {
    const auto &dataEvent = static_cast<const DataEvent &>(*event);
    handleDataEvent(dataEvent);
  }
  else if (event->type == EventType::Order)
  {
    const auto &orderEvent = static_cast<const OrderEvent &>(*event);
    handleOrderEvent(orderEvent);
  }
  else if (event->type == EventType::Fill)
  {
    const auto &fillEvent = static_cast<const FillEvent &>(*event);
    handleFillEvent(fillEvent);
  }
}

void Backtester::handleDataEvent(const DataEvent &dataEvent)
{
  ++processedBars;
  latestCandle = dataEvent.getCandle();
  LastCandle = true;

  std::optional<OrderEvent> orderEvent = strategy->onData(dataEvent);
  if (orderEvent)
  {
    queue.pushEvent(std::make_unique<OrderEvent>(*orderEvent));
  }
}

void Backtester::handleOrderEvent(const OrderEvent &orderEvent)
{
  if (!LastCandle)
  {
    return;
  }

  queue.pushEvent(broker.fillOrder(orderEvent, latestCandle));
}

void Backtester::handleFillEvent(const FillEvent &fillEvent)
{
  portafolio->applyFill(fillEvent);
}

int Backtester::GetProcessedBars() const { return processedBars; }

double Backtester::GetLatestClose() const { return latestCandle.close; }

const Portafolio &Backtester::GetPortafolio() const { return *portafolio; }
