#pragma once
#include "TradingStrategy.h"
#include <queue>

class S_BollingerRSI : public TradingStrategy {
  private:
    int bPeriod;
    double bMultiplier;

    int rsiPeriod;
    int rsiUpper;
    int rsiLower;

    double prevDayPrice;
    std::queue<double> bWindowPrices;
    void UpdateWindow(double dayPrice);

    std::queue<double> rsiGains;
    std::queue<double> rsiLosses;

    double bStdDev();
    double bMovingAverage();
    double rsiFinal();

  public:
    S_BollingerRSI(int bPeriod, double bMultiplier, int rsiPeriod, int rsiUpper, int rsiLower);
    int CreateSignal(double dayPrice, double currentShares) override;
};