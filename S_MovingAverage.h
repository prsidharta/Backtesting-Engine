#pragma once

#include "TradingStrategy.h"

class S_MovingAverage : public TradingStrategy {
  public:
    int CreateSignal(double dayPrice, double windowPrice, double currentShares) override;
};