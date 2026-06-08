#pragma once
#include "TradingStrategy.h"
#include <queue>

/**
 * @class S_BollingerRSI
 * @brief Utilizes Bollinger bands and Relative Strength Index (RSI) to influence trading decisions
 * @details Executes trades based on price extremes in combination with momentum exhaustion
 */
class S_BollingerRSI : public TradingStrategy {
  private:
    int bPeriod;
    double bMultiplier;
    double bWindowSum;
    double bWindowSumSquares;

    int rsiPeriod;
    int rsiUpper;
    int rsiLower;
    double rsiGainsSum;
    double rsiLossesSum;

    double prevDayPrice;
    std::queue<double> bWindowPrices;
    /**
     * @brief Takes in new price tick and ensures all internals match new date
     * @param dayPrice The incoming raw market price
     */
    void UpdateWindow(double dayPrice);

    std::queue<double> rsiGains;
    std::queue<double> rsiLosses;

    /**
     * @brief Calculates the standard deviation for the bollinger window period in O(1) time
     * @details Variables needed for this function's end calculation are found during the UpdateWindow loop preventing further data traversal and saving time
     * @return The standard deviation of the bollinger window period (0.0 on division by zero edge cases)
     */
    double bStdDev();

    /**
     * @brief Calculates the simple moving average for the bollinger band final calculation in O(1) time
     * @return The mean price of the current sliding window. (0.0 if window empty)
     */
    double bMovingAverage();

    /**
     * @brief Computes the Relative Strength Index (RSI) using both rsi gains and losses queues
     * @details Catches division by zero edge cases 
     * @return RSI between 0.0 and 100.0
     */
    double rsiFinal();

  public:
    /**
     * @brief Initializes the window, limits, and thresholds
     * @param bPeriod The window of days being tracked for the bollinger band average calculation
     * @param bMultiplier The stddev multiplier outlining the width of the bollinger bands
     * @param rsiPeriod The period of days being tracked for the rsi momentum calculation
     * @param rsiUpper The overbought momentum threshold
     * @param rsiLower The oversold momentum threshold
     */
    S_BollingerRSI(int bPeriod, double bMultiplier, int rsiPeriod, int rsiUpper, int rsiLower);

    /**
     * @brief Evaluates current market state to generate a trading signal
     * @param dayPrice The incoming raw market price
     * @param currentShares The amount of shares of stock held by the user at the present time
     * @details returns BUY if price dips under lower band AND momentum is oversold
     * @details returns SELL if price overshoots upper band AND momentum is overbought
     * @return 1 (BUY), 0 (HOLD), -1 (SELL)
     */
    int CreateSignal(double dayPrice, double currentShares) override;
};