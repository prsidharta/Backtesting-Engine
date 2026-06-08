#pragma once

/**
 * @class TradingStrategy
 * @brief Base class that all specific trading strategies inherit from
 */
class TradingStrategy {
  public:
    virtual ~TradingStrategy() = default;

    /**
     * @brief
     * @param dayPrice The value of the stock on the current day
     * @param currentShares The total number of shares currently held by the user
     * @return int 1 Commands the engine to buy shares
     * @return int 0 Commands the engine to hold
     * @return int -1 Commands the engine to sell shares
     */
    virtual int CreateSignal(double dayPrice, double currentShares) = 0;
};