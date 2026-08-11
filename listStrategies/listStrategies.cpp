#include "listStrategies.h"
#include "../strategyBase/StrategyRegistry.h"

#include <iostream>
#include <string>

bool ListStrats::run(int argc, char *argv[])
{
    if (argc != 2 || std::string(argv[1]) != "--list-strategies")
    {
        return false;
    }

    printStrats();
    return true;
}

void ListStrats::printStrats()
{
    const auto strategies =
        StrategyRegistry::instance().listStrats();

    std::cout << "Available strategies:\n";

    for (const auto &strategy : strategies)
    {
        std::cout << "  " << strategy << '\n';
    }
}
