# Event-Driven C++ Backtester

A modular C++17 backtesting engine for replaying historical candle data through trading strategies, simulated execution, portfolio accounting, and performance reporting.


## Project Status

This repository is the first iteration of an event-driven backtesting engine. It is an intentionally small, research and learning-oriented foundation rather than a finished backtesting platform or live-trading system.

The goal is to keep extending the engine over time with additional strategies, execution models, portfolio capabilities, data sources, performance metrics, and tests while preserving the separation between its core components.

## Inspiration

The event-driven design is inspired in part by the Interactive Brokers Campus article [A Practical Breakdown of Vector-Based vs. Event-Based Backtesting](https://www.interactivebrokers.com/campus/ibkr-quant-news/a-practical-breakdown-of-vector-based-vs-event-based-backtesting/).

That article's discussion of sequentially processing market-data events, orders, fills, and portfolio updates helped shape this project's architecture. This repository is an independent implementation intended to provide a foundation that can be studied and extended.

## Overview

```text
Binance-style candle CSV
          |
          v
      DataEvent
          |
          v
       Strategy
          |
          v
      OrderEvent
          |
          v
   SimulatedBroker
          |
          v
       FillEvent
          |
          v
      Portafolio
          |
          v
   Performance CSV
```

For each candle, the `Backtester` reads market data, gives it to the selected strategy, processes any resulting order, updates the portfolio, calculates performance metrics, and records the current state.

At the end of the dataset, an open position is closed at the final candle close and a final performance row is written.

## Features

- C++17 implementation.
- FIFO event queue based on `std::unique_ptr<Event>`.
- Strategy selection by registered name.
- Self-registering strategy implementations.
- Configurable CSV batch size.
- Signed net-position portfolio accounting for long and short positions.
- Weighted-average entry prices.
- Partial closes and position reversals.
- Realized and unrealized PnL.
- Equity, return, drawdown, Sharpe, and Sortino metrics.
- CSV performance output after every candle.

## Included Strategies

| Name | Description | Defaults |
| --- | --- | --- |
| `buy_and_hold` | Buys once on the first candle and holds until the end. | `BTCUSDT`, `100 USDT` |
| `moving_average` | Trades when fast and slow moving averages cross. | Fast window `2`, slow window `3`, `100 USDT` per signal |
| `dollar_cost_avg` | Buys a fixed dollar amount at a fixed bar interval. | `BTCUSDT`, `$100`, every `30` bars |

List the registered strategies with:

```bash
./build/backtester --list-strategies
```

## Build

### Requirements

- CMake 3.12 or newer.
- A C++17-compatible compiler.
- Python 3 for the data-download script.

### Configure and compile

```bash
cmake -S . -B build
cmake --build build
```

## Run a Backtest

Provide a Binance-compatible CSV file first, either by using your own dataset or by running the downloader described in [Utility Scripts](#utility-scripts).

```text
backtester <csv-file> <strategy> <performance-name>
           [rows-per-batch] [starting-balance] [periods-per-year]
```

Example:

```bash
./build/backtester \
  ./Data/btcusdt_1d_5y.csv \
  buy_and_hold \
  run_01
```

Example with optional settings:

```bash
./build/backtester \
  ./Data/btcusdt_1d_5y.csv \
  moving_average \
  moving_average_run \
  100 \
  50000 \
  365
```

Default values:

| Setting | Default |
| --- | ---: |
| Rows per batch | `100` |
| Starting balance | `100000.0` |
| Periods per year | `365.0` |

### Rows Per Batch

The `rows-per-batch` argument controls how many candle records the CSV reader loads into memory at one time while the backtest is running.

It does **not** limit the total number of candles processed. The backtester continues reading batches until it reaches the end of the input file. The final batch may contain fewer records than the configured batch size.

For example:

```bash
./build/backtester data.csv buy_and_hold run_01 1
```

This processes the same complete dataset while loading one candle at a time. A larger value, such as `100` or `1000`, reduces file-read operations and allows more candles to be buffered at once.

The default is `100` rows per batch. Use a positive value for this argument. Under normal conditions, changing the batch size does not change strategy signals or portfolio results; it only changes how input data is buffered.

Performance files are written to:

```text
Performance/<performance-name>.csv
```

Existing performance files are not overwritten.

## Input Data

The CSV reader supports Binance-style candle files. It skips the header and reads these fields:

```text
open_time, open, high, low, close, volume, close_time
```

The reader requires at least 12 comma-separated fields because the remaining fields are part of the Binance export format. Timestamps are preserved as strings and numeric values are stored as `double`.

Downloaded market data is stored in `Data/`. This directory is excluded from version control, so generate a dataset locally or provide your own Binance-compatible CSV file.

## Utility Scripts

The `scripts/` folder contains command-line utilities for preparing market data. The included downloader fetches completed daily Binance spot candles and writes them in the CSV format expected by the backtester.

Example:

```bash
python scripts/download_binance_daily.py \
  --symbol BTCUSDT \
  --start 2021-06-19 \
  --end 2026-06-18 \
  --output Data/btcusdt_1d_5y.csv
```

Generated market data is stored under `Data/`, which is excluded from version control by `.gitignore`.

## Portfolio Model

`Portafolio` defines the accounting interface, while `NaivePortafolio` provides the default implementation.

The portfolio stores one signed net position:

```text
positive quantity  = long
negative quantity  = short
zero quantity      = flat
```

Fills are processed as follows:

- A fill against a flat portfolio opens a position.
- A same-direction fill increases the position and recalculates the weighted entry price.
- An opposite-direction fill reduces or closes the position.
- An oversized opposite-direction fill closes the current position and opens a position in the opposite direction.

The portfolio tracks realized PnL, unrealized PnL, commissions, equity, return, and drawdown metrics.



## Simulated Execution

`SimulatedBroker` converts an `OrderEvent` into a `FillEvent`.

- Market orders fill at the current candle close.
- Limit and stop orders use the requested order price.
- Fill quantity is copied from the order.
- Normal simulated fills use zero commission.

## Performance Output

Each output row represents the portfolio state after processing a candle:

```text
id,dateOpen,dateClose,open,high,low,close,volume,
side,currentEquity,currentDrawdown,
currentMaxDrawdown,currentClosePrice
```

The output includes the candle data, current position side, equity, current drawdown, maximum drawdown, and market close price.

The output side describes the net position after processing the candle, not whether an order was placed on that row.

## Testing

Run the C++ test suite with:

```bash
ctest --test-dir build --output-on-failure
```

The test suite intentionally focuses on end-to-end behavior rather than maintaining a separate test executable for every class:

- `backtester_final_state_test` runs a complete backtest and verifies final position closure and performance output.
- `moving_average_backtester_test` runs the registered SMA crossover strategy and verifies crossover signals, quote-amount sizing, reversal behavior, and final position closure.

## Project Structure

```text
.
├── Backtester/                    Main loop and event dispatcher
├── Data/                          Locally generated market data (gitignored)
├── DataEvent/                     Market-data event
├── DataHandler/                   CSV candle reader
├── Event/                         Base event type
├── EventQueue/                    FIFO event queue
├── FillEvent/                     Fill event
├── OrderEvent/                    Order event
├── Portafolio/                    Portfolio interface and accounting
├── SimulatedBroker/               Simulated order execution
├── Strategies/                    Strategy implementations and registrations
├── StrategyPerformaceSaver/       Performance CSV writer
├── helpers/                       Shared sizing and utility functions
├── scripts/                       Data download and preparation utilities
├── strategyBase/                  Strategy registry
├── types/                         Candle, position, and event types
├── tests/                         End-to-end integration test
├── CMakeLists.txt                 Build and test configuration
└── main.cpp                       Command-line entry point
```

## Adding a Strategy

Strategies implement the `Strategy` interface:

```cpp
class Strategy
{
public:
    virtual ~Strategy() = default;
    virtual std::optional<OrderEvent>
    onData(const DataEvent &dataEvent) = 0;
};
```

### Registering a Strategy

Each strategy uses three files under `Strategies/`:

```text
MyStrategy.h
MyStrategy.cpp
MyStrategyRegistration.cpp
```

The registration file connects a command-line name to a factory that creates the strategy:

```cpp
#include "MyStrategy.h"
#include "../strategyBase/strategyRegistration.h"
#include <memory>

namespace
{
[[maybe_unused]] const StrategyRegistration registration(
    "my_strategy",
    []
    {
        return std::make_unique<MyStrategy>(
            "BTCUSDT", 100.0); // 100 USDT quote amount
    });
}
```

The registration process works as follows:

1. `"my_strategy"` becomes the strategy name used on the command line.
2. The lambda acts as a factory and creates a new `MyStrategy` instance.
3. The registration object adds the name and factory to `StrategyRegistry` before `main()` starts.
4. `StrategyRegistry::create()` finds the name and calls the factory when the backtest starts.
5. The resulting `std::unique_ptr<Strategy>` is stored by `Backtester`.

The registration source file must be directly inside `Strategies/`. CMake discovers direct `.cpp` files in that folder automatically during configuration. After adding a strategy, reconfigure and rebuild:

```bash
cmake -S . -B build
cmake --build build
./build/backtester --list-strategies
```

### `OrderEvent` Arguments

A strategy does not call the portfolio directly. It returns an `OrderEvent` from `onData()`. The broker converts that order into a fill, and the portfolio applies the fill.

An `OrderEvent` is created with six values:

```cpp
OrderEvent(OrderType orderType,
           OrderSide side,
           std::string symbol,
           std::string timestamp,
           double price,
           double quantity);
```

Example:

```cpp
OrderEvent(OrderType::Market,   // order type
           OrderSide::Buy,      // buy or sell
           "BTCUSDT",            // asset symbol
           candle.date_open,     // order timestamp
           candle.close,         // requested price
           2.0);                 // quantity
```

The arguments mean:

| Argument | Meaning |
| --- | --- |
| `OrderType` | Execution type: `Market`, `Limit`, or `Stop`. Only `Market` is currently fully implemented; `Limit` and `Stop` are represented but do not yet validate trigger conditions. |
| `OrderSide` | Direction: `Buy` or `Sell`. |
| `symbol` | Asset being traded, such as `BTCUSDT`. |
| `timestamp` | Timestamp associated with the order. |
| `price` | Requested order price. Market orders are filled at the current candle close by the simulated broker. |
| `quantity` | Base-asset quantity to buy or sell, such as BTC. Quote amounts are converted into this quantity before the `OrderEvent` is created. |

The built-in strategies accept quote amounts such as USDT and convert them into base-asset quantities using the current candle close:

```cpp
const double quantity =
    StrategySizing::quoteAmountToQuantity(100.0, candle.close);
```

For example, `100 USDT` becomes `1 BTC` when BTC costs `100 USDT`, or `2 BTC` when BTC costs `50 USDT`. The resulting `OrderEvent` continues to use the existing quantity field, so the broker and portfolio structure do not need to change.

### Opening, Closing, and Partially Closing a Position

For a long position:

The example assumes the strategy has a member variable such as `std::size_t barsSeen{};` to schedule its signals.

```cpp
std::optional<OrderEvent>
ExampleStrategy::onData(const DataEvent &dataEvent)
{
    ++barsSeen;

    const Candle &candle = dataEvent.getCandle();

    if (barsSeen == 1)
    {
        // Open a long position of 2.0 units.
        return OrderEvent(OrderType::Market, OrderSide::Buy,
                          "BTCUSDT", candle.date_open,
                          candle.close, 2.0);
    }

    if (barsSeen == 10)
    {
        // Partially close 1.0 unit. The position becomes +1.0.
        return OrderEvent(OrderType::Market, OrderSide::Sell,
                          "BTCUSDT", candle.date_open,
                          candle.close, 1.0);
    }

    if (barsSeen == 20)
    {
        // Close the remaining 1.0 unit. The position becomes FLAT.
        return OrderEvent(OrderType::Market, OrderSide::Sell,
                          "BTCUSDT", candle.date_open,
                          candle.close, 1.0);
    }

    return std::nullopt;
}
```

The resulting position flow is:

```text
BUY  2.0  -> position +2.0  (open long)
SELL 1.0  -> position +1.0  (partial close)
SELL 1.0  -> position  0.0  (full close)
```

For a short position, the sides are reversed:

```text
SELL 2.0  -> position -2.0  (open short)
BUY  1.0  -> position -1.0  (partial close)
BUY  1.0  -> position  0.0  (full close)
```

The `quantity` in an opposite-side order determines how much of the current position is closed. An opposite-side order larger than the current position closes it and opens a position in the other direction.

## Scope and Limitations

This is a research and portfolio project rather than a live-trading system. The engine intentionally uses simplified execution and accounting:

- Market orders execute at the signal candle close.
- Cash balance and buying power are not modeled separately from portfolio equity.
- Limit and stop trigger rules are simplified.
- Slippage, latency, partial fills, and exchange connectivity are not modeled.
- The default portfolio tracks one net position rather than a complete trade ledger.
- The CSV parser expects a Binance-style schema and does not implement full CSV quoting rules.

These constraints keep the implementation small and make the core event-driven architecture easy to study and extend.
