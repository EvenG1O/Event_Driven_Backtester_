#pragma once

#include <cstddef>

bool isFlat(double quantity);
bool sameDirection(double currentQuantity, double fillQuantity);
bool reversedDirection(double previousQuantity, double updatedQuantity);

void updateSharpeStatistics(double periodReturn,
                            std::size_t &returnCount,
                            double &returnMean,
                            double &returnM2);

void updateSortinoStatistics(double periodReturn,
                             double targetReturn,
                             double &excessReturnSum,
                             double &downsideDifferenceSquaredSum,
                             std::size_t &downsideCount);

double calculateSharpeRatio(std::size_t returnCount,
                            double returnMean,
                            double returnM2,
                            double periodsPerYear);

double calculateSortinoRatio(std::size_t returnCount,
                             double excessReturnSum,
                             double downsideDifferenceSquaredSum,
                             std::size_t downsideCount,
                             double periodsPerYear);
