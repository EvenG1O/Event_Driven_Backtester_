#include "QuoteSizing.h"
#include <cmath>
#include <stdexcept>

namespace StrategySizing
{
double quoteAmountToQuantity(double quoteAmount, double marketPrice)
{
    if (!std::isfinite(quoteAmount) || quoteAmount <= 0.0 ||
        !std::isfinite(marketPrice) || marketPrice <= 0.0)
    {
        throw std::invalid_argument(
            "Quote amount and market price must be positive and finite");
    }

    return quoteAmount / marketPrice;
}
}
