#include "StrategyRegistry.h"
#include "../Strategies/Strategy.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace
{
    std::string normalize(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });
        return value;
    }
}

StrategyRegistry &StrategyRegistry::instance()
{
    static StrategyRegistry registry;
    return registry;
}

bool StrategyRegistry::registerStrategy(std::string name,
                                        StrategyFactory factory)
{
    return Strats.emplace(normalize(std::move(name)),
                          std::move(factory))
        .second;
}

std::unique_ptr<Strategy>
StrategyRegistry::create(const std::string &name) const
{
    const auto iterator = Strats.find(normalize(name));
    if (iterator == Strats.end())
    {
        throw std::invalid_argument("Unknown strategy: " + name);
    }

    return iterator->second();
}

std::vector<std::string> StrategyRegistry::listStrats() const
{
    std::vector<std::string> strats;

    for (const auto &s : Strats)
    {
        strats.push_back(s.first);
    }

    return strats;
}
