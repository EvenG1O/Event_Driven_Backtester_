#include "PortafolioHelpers.h"
#include <cmath>

static constexpr double flatEpsilon = 1e-12;

bool isFlat(double quantity)
{
    return std::abs(quantity) < flatEpsilon;
}

bool sameDirection(double currentQuantity, double fillQuantity)
{
    return (currentQuantity > 0.0 && fillQuantity > 0.0) ||
           (currentQuantity < 0.0 && fillQuantity < 0.0);
}

bool reversedDirection(double previousQuantity, double updatedQuantity)
{
    return (previousQuantity > 0.0 && updatedQuantity < 0.0) ||
           (previousQuantity < 0.0 && updatedQuantity > 0.0);
}

void updateSharpeStatistics(double periodReturn,
                            std::size_t &returnCount,
                            double &returnMean,
                            double &returnM2)
{
    ++returnCount;

    const double difference = periodReturn - returnMean;
    returnMean += difference / static_cast<double>(returnCount);

    const double updatedDifference = periodReturn - returnMean;
    returnM2 += difference * updatedDifference;
}

void updateSortinoStatistics(double periodReturn,
                             double targetReturn,
                             double &excessReturnSum,
                             double &downsideDifferenceSquaredSum,
                             std::size_t &downsideCount)
{
    excessReturnSum += periodReturn - targetReturn;

    if (periodReturn < targetReturn)
    {
        const double downsideDifference = periodReturn - targetReturn;
        downsideDifferenceSquaredSum +=
            downsideDifference * downsideDifference;
        ++downsideCount;
    }
}

double calculateSharpeRatio(std::size_t returnCount,
                            double returnMean,
                            double returnM2,
                            double periodsPerYear)
{
    if (returnCount < 2)
    {
        return 0.0;
    }

    const double variance =
        returnM2 / static_cast<double>(returnCount - 1);
    const double standardDeviation = std::sqrt(variance);

    if (isFlat(standardDeviation))
    {
        return 0.0;
    }

    return (returnMean / standardDeviation) * std::sqrt(periodsPerYear);
}

double calculateSortinoRatio(std::size_t returnCount,
                             double excessReturnSum,
                             double downsideDifferenceSquaredSum,
                             std::size_t downsideCount,
                             double periodsPerYear)
{
    if (returnCount == 0)
    {
        return 0.0;
    }

    if (downsideCount == 0)
    {
        return 0.0;
    }

    const double averageExcessReturn =
        excessReturnSum / static_cast<double>(returnCount);
   
    const double downsideDeviation =
        std::sqrt(downsideDifferenceSquaredSum /
                  static_cast<double>(returnCount));

    if (isFlat(downsideDeviation))
    {
        return 0.0;
    }

    return (averageExcessReturn / downsideDeviation) *
           std::sqrt(periodsPerYear);
}
