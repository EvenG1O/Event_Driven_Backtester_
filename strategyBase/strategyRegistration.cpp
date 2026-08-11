#include "strategyRegistration.h"
#include <stdexcept>
#include <utility>

StrategyRegistration::StrategyRegistration(std::string name,
                                           StrategyFactory strat)
{
    if (!StrategyRegistry::instance().registerStrategy(std::move(name),
                                                       std::move(strat)))
    {
        throw std::invalid_argument("Duplicate strategy name");
    }
}
