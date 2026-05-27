#pragma once

#include "TradingStrategy.h"
#include <string>
#include <vector>

struct TradingRecord {

    std::string type;
    double price;
    double shares;

    TradingRecord(std::string givenType, double givenPrice, double givenShares) {
        type = givenType;
        price = givenPrice;
        shares = givenShares;
    }
};

class SimEngine {
  private:
    double m_cash;
    double m_shares;
    TradingStrategy *m_strategy;

  public:
    SimEngine(double startingCash, TradingStrategy *strategy);
    void RunSimulator(std::vector<double> &prices, std::vector<double> &windowAverages, int windowSize);
    double GetCash();
    double GetShares();
};

/**
 * @brief Simulates a run of the backtesting engine
 * @param prices A vector of listed prices of stock for all days
 * @param windowAverages A vector of daily average trends based on the window_size
 * @param[in,out] cash Simulated bank account of the user
 * @param[in,out] shares Number of simulated shares owned by the user
 * @param windowSize The number of days a trend average is calculated for, used to align prices day and window_average
 * day
 * @note shares set to double supports fractional share ownership, int shares does not (which is not applied here)
 **/

/**
 * @brief Calculates a window_size day average trend for each day
 * @param prices, A vector of listed prices of stock for all days
 * @param windowSize, The number of days a trend average is calculated for
 * @throw std::invalid_argument if prices vector is empty
 * @throw std::invalid_argument if window_size is greater than total number of prices
 * @return std::vector<double> containing the calculated window averages
 */
std::vector<double> CalculateWindowAverages(std::vector<double> &prices, int windowSize);