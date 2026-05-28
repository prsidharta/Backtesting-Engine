#pragma once

#include "TradingStrategy.h"
#include <vector>

class S_MovingAverage : public TradingStrategy {
  private:
    int m_windowSize;
    std::vector<double> m_priceHistory;

  public:
    S_MovingAverage(int windowSize);
    int CreateSignal(double dayPrice, double currentShares) override;
};