cmake_minimum_required(VERSION 3.12)
project(backtester VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB STRATEGY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Strategies/*.cpp")

set(BACKTESTER_SOURCES
    Backtester/Backtester.cpp
    strategyBase/StrategyRegistry.cpp
    strategyBase/strategyRegistration.cpp
    listStrategies/listStrategies.cpp
    ${STRATEGY_SOURCES}
    Portafolio/Portafolio.cpp
    Portafolio/NaivePortafolio.cpp
    DataHandler/ReadDataCsv.cpp
    EventQueue/EventQueue.cpp
    Portafolio/PortafolioHelpers.cpp
    helpers/QuoteSizing.cpp
    SimulatedBroker/Broker.cpp
    Strategies/Indicators/MovingAverage.cpp
    StrategyPerformaceSaver/PerformanceSaver.cpp
)

add_executable(backtester
    main.cpp
    ${BACKTESTER_SOURCES}
)

enable_testing()

add_executable(backtester_final_state_test
    tests/backtester_final_state_test.cpp
    ${BACKTESTER_SOURCES}
)

add_test(NAME backtester_final_state_test COMMAND backtester_final_state_test)

add_executable(moving_average_backtester_test
    tests/moving_average_backtester_test.cpp
    ${BACKTESTER_SOURCES}
)

add_test(NAME moving_average_backtester_test
         COMMAND moving_average_backtester_test)
