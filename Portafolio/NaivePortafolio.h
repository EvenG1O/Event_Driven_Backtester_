#pragma once

#include "Portafolio.h"
#include "../types/Position.h"

#include <cstddef>
#include <string>

class NaivePortafolio : public Portafolio
{
private:
    Position position_{};
    std::string symbol_{"BTCUSDT"};
    double startingBalance_;
    double peakEquity_;
    double maxDrawdown_{};
    double sortinoRatio_{};
    double sharpeRatio_{};
    double targetReturn_{};
    double previousEquity_{};
    bool hasPreviousEquity_ = false;
    double periodsPerYear_;
    std::size_t returnCount_{};
    double returnMean_{};
    double returnM2_{};
    double excessReturnSum_{};
    double downsideDifferenceSquaredSum_{};
    std::size_t downsideCount_{};

public:
    explicit NaivePortafolio(double startingBalance = 100000.0,
                             double periodsPerYear = 365.0);

    void applyFill(const FillEvent &fillEvent) override;
    void closeOpenPosition(const Candle &closeCandle) override;
    void updateMetrics(double marketPrice) override;

    double unrealizedPnl(double marketPrice) const override;
    double realizedPnl() const override;
    double commissions() const override;
    double currentQuantity() const override;
    double averageEntryPrice() const override;
    int activeTradeCount() const override;

    double totalPnl(double marketPrice) const override;
    double equity(double marketPrice) const override;
    double totalReturnPercent(double marketPrice) const override;
    double getCurrentDrawdown(double marketPrice) const override;
    double getMaxDrawdown() const override;
    double getSharpeRatio() const override;
    double getSortinoRatio() const override;
    long double totalProfit() const override;

    void portafolioPerformace(double marketPrice) const override;
    void portafolioPerformace() const override;
};
