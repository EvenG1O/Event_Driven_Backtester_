#include "NaivePortafolio.h"
#include "PortafolioHelpers.h"
#include <algorithm>
#include <cmath>
#include <iostream>

NaivePortafolio::NaivePortafolio(double startingBalance,
                                 double periodsPerYear)
    : startingBalance_(startingBalance), peakEquity_(startingBalance),
      periodsPerYear_(periodsPerYear)
{
}

void NaivePortafolio::closeOpenPosition(const Candle &closeCandle)
{
    if (isFlat(position_.quantity))
    {
        return;
    }

    const OrderSide closeSide =
        position_.quantity > 0.0 ? OrderSide::Sell : OrderSide::Buy;
    FillEvent closeFill(symbol_, closeSide, closeCandle.close,
                        std::abs(position_.quantity), 0.0, closeCandle);

    applyFill(closeFill);
}

void NaivePortafolio::applyFill(const FillEvent &fillEvent)
{
    if (fillEvent.getFillQuantity() <= 0.0)
    {
        return;
    }

    if (!fillEvent.getSymbol().empty())
    {
        symbol_ = fillEvent.getSymbol();
    }

    const double signedFillQuantity =
        fillEvent.getSide() == OrderSide::Buy ? fillEvent.getFillQuantity()
                                              : -fillEvent.getFillQuantity();
    const double fillPrice = fillEvent.getFillPrice();
    const double oldQuantity = position_.quantity;

    position_.commissions += fillEvent.getCommission();

    if (isFlat(oldQuantity))
    {
        position_.quantity = signedFillQuantity;
        position_.averageEntryPrice = fillPrice;
        return;
    }

    if (sameDirection(oldQuantity, signedFillQuantity))
    {
        const double newQuantity = oldQuantity + signedFillQuantity;
        position_.averageEntryPrice =
            ((std::abs(oldQuantity) * position_.averageEntryPrice) +
             (std::abs(signedFillQuantity) * fillPrice)) /
            std::abs(newQuantity);
        position_.quantity = newQuantity;
        return;
    }

    const double closingQuantity =
        std::min(std::abs(oldQuantity), std::abs(signedFillQuantity));

    if (oldQuantity > 0.0)
    {
        position_.realizedPnl +=
            (fillPrice - position_.averageEntryPrice) * closingQuantity;
    }
    else
    {
        position_.realizedPnl +=
            (position_.averageEntryPrice - fillPrice) * closingQuantity;
    }

    const double newQuantity = oldQuantity + signedFillQuantity;

    if (isFlat(newQuantity))
    {
        position_.quantity = 0.0;
        position_.averageEntryPrice = 0.0;
    }
    else
    {
        position_.quantity = newQuantity;

        if (reversedDirection(oldQuantity, newQuantity))
        {
            position_.averageEntryPrice = fillPrice;
        }
    }
}

double NaivePortafolio::unrealizedPnl(double marketPrice) const
{
    if (isFlat(position_.quantity))
    {
        return 0.0;
    }

    return (marketPrice - position_.averageEntryPrice) * position_.quantity;
}

double NaivePortafolio::realizedPnl() const
{
    return position_.realizedPnl;
}

double NaivePortafolio::commissions() const
{
    return position_.commissions;
}

double NaivePortafolio::currentQuantity() const
{
    return position_.quantity;
}

double NaivePortafolio::averageEntryPrice() const
{
    return position_.averageEntryPrice;
}

int NaivePortafolio::activeTradeCount() const
{
    return isFlat(position_.quantity) ? 0 : 1;
}

void NaivePortafolio::updateMetrics(double marketPrice)
{
    const double currentEquity = equity(marketPrice);

    if (hasPreviousEquity_ && !isFlat(previousEquity_))
    {
        const double periodReturn =
            (currentEquity - previousEquity_) / previousEquity_;

        updateSharpeStatistics(
            periodReturn, returnCount_, returnMean_, returnM2_);
        updateSortinoStatistics(
            periodReturn, targetReturn_, excessReturnSum_,
            downsideDifferenceSquaredSum_, downsideCount_);

        sharpeRatio_ = calculateSharpeRatio(
            returnCount_, returnMean_, returnM2_, periodsPerYear_);
        sortinoRatio_ =
            calculateSortinoRatio(
                returnCount_, excessReturnSum_,
                downsideDifferenceSquaredSum_, downsideCount_,
                periodsPerYear_);
    }

    previousEquity_ = currentEquity;
    hasPreviousEquity_ = true;

    if (currentEquity > peakEquity_)
    {
        peakEquity_ = currentEquity;
    }

    const double drawdown = peakEquity_ - currentEquity;
    if (drawdown > maxDrawdown_)
    {
        maxDrawdown_ = drawdown;
    }
}

double NaivePortafolio::totalPnl(double marketPrice) const
{
    return position_.realizedPnl + unrealizedPnl(marketPrice) -
           position_.commissions;
}

double NaivePortafolio::equity(double marketPrice) const
{
    return startingBalance_ + totalPnl(marketPrice);
}

double NaivePortafolio::totalReturnPercent(double marketPrice) const
{
    if (isFlat(startingBalance_))
    {
        return 0.0;
    }

    return ((equity(marketPrice) - startingBalance_) / startingBalance_) *
           100.0;
}

double NaivePortafolio::getCurrentDrawdown(double marketPrice) const
{
    const double currentDrawdown = peakEquity_ - equity(marketPrice);
    return currentDrawdown > 0.0 ? currentDrawdown : 0.0;
}

double NaivePortafolio::getMaxDrawdown() const
{
    return maxDrawdown_;
}

double NaivePortafolio::getSharpeRatio() const
{
    return sharpeRatio_;
}

double NaivePortafolio::getSortinoRatio() const
{
    return sortinoRatio_;
}

long double NaivePortafolio::totalProfit() const
{
    return position_.realizedPnl - position_.commissions;
}

void NaivePortafolio::portafolioPerformace(double marketPrice) const
{
    std::cout << "Portafolio Performace overview" << std::endl;
    std::cout << "Realized PnL: " << totalProfit() << std::endl;
    std::cout << "Total PnL: " << totalPnl(marketPrice) << std::endl;
    std::cout << "Equity: " << equity(marketPrice) << std::endl;
    std::cout << "Total return %: " << totalReturnPercent(marketPrice)
              << std::endl;
    std::cout << "Commissions: " << commissions() << std::endl;
    std::cout << "Max drawdown: " << maxDrawdown_ << std::endl;
    std::cout << "Sharpe ratio: " << sharpeRatio_ << std::endl;
    std::cout << "Sortino ratio: " << sortinoRatio_ << std::endl;
}

void NaivePortafolio::portafolioPerformace() const
{
    portafolioPerformace(averageEntryPrice());
}
