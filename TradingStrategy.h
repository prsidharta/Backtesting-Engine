#pragma once

class TradingStrategy {
  public:
    virtual ~TradingStrategy() = default;

    virtual int CreateSignal(double dayPrice, double windowPrice, double currentShares) = 0;
};