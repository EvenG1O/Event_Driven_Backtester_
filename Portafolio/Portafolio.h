#pragma once

#include "../FillEvent/FilleEvent.h"
#include "../types/Candle.h"

class Portafolio
{
public:
    virtual ~Portafolio() = default;

    virtual void applyFill(const FillEvent &fillEvent) = 0;
    virtual void closeOpenPosition(const Candle &closeCandle) = 0;

    virtual void updateMetrics(double marketPrice) = 0;

    virtual double unrealizedPnl(double marketPrice) const = 0;
    virtual double realizedPnl() const = 0;
    virtual double commissions() const = 0;
    virtual double currentQuantity() const = 0;
    virtual double averageEntryPrice() const = 0;
    virtual int activeTradeCount() const = 0;

    virtual double totalPnl(double marketPrice) const = 0;
    virtual double equity(double marketPrice) const = 0;
    virtual double totalReturnPercent(double marketPrice) const = 0;
    virtual double getCurrentDrawdown(double marketPrice) const = 0;
    virtual double getMaxDrawdown() const = 0;
    virtual double getSharpeRatio() const = 0;
    virtual double getSortinoRatio() const = 0;
    virtual long double totalProfit() const = 0;

    virtual void portafolioPerformace(double marketPrice) const = 0;
    virtual void portafolioPerformace() const = 0;
};
