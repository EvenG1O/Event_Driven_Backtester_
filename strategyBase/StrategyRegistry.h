#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Strategy;

using StrategyFactory = std::function<std::unique_ptr<Strategy>()>;

class StrategyRegistry
{
private:
    std::map<std::string, StrategyFactory> Strats;

public:
    static StrategyRegistry &instance();

    bool registerStrategy(std::string name, StrategyFactory strat);
    std::unique_ptr<Strategy> create(const std::string &name) const;

    std::vector<std::string> listStrats() const;
};
