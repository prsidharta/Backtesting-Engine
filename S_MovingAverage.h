#pragma once

#include "TradingStrategy.h"
#include <vector>

/**
 * @class S_MovingAverage
 * @brief The sliding window average trading strategy where you buy and sell based on the daily price compared to a
 * window average
 */
class S_MovingAverage : public TradingStrategy {
  private:
    int m_windowSize;
    std::vector<double> m_priceHistory;

  public:
    /**
     * @brief Constructs a moving average strategy with a hisory of prices
     * @param windowSize The number of days to track for when calculating the moving average
     */
    S_MovingAverage(int windowSize);

    /**
     * @brief Creates a buy || hold || sell signal and relays that information to the simulation based on the strategy
     * @param dayPrice The current daily stock price
     * @param currentShares The amount of shares of stock held by the user at the present time
     * @return 1 indicating to buy || 0 indicating to hold|| -1 indicating to sell
     */
    int CreateSignal(double dayPrice, double currentShares) override;
};