#pragma once

#include "TradingStrategy.h"
#include <string>
#include <vector>

/**
 * @struct TradingRecord
 * @brief Data container that holds all necessary information for one singular trade, used in vector of structs to
 * compile whole trading record of simulation
 */
struct TradingRecord {

    std::string type; ///< The transaction classification ("BUY", "HOLD", "SELL")
    double price;     ///< The price per share at time of trade
    double shares;    ///< The amount of shares bought or sold at the time of the trade

    /**
     * @brief Constructs a new TradingRecord instance
     * @param givenType The buy, hold, or sell signal
     * @param givenPrice The price of the shares at the time of the trade
     * @param givenShares The amount of shares bought or sold at the time of the trade
     */
    TradingRecord(const std::string givenType, double givenPrice, double givenShares) {
        type = givenType;
        price = givenPrice;
        shares = givenShares;
    }
};

/**
 * @class SimEngine
 * @brief Execution engine that maintains portfolio state and executes trades based on the provided trading strategy
 */
class SimEngine {
  private:
    double m_cash;               ///< The current amount of cash ($USD) held by the portfolio
    double m_shares;             ///< Present held volume of equity
    TradingStrategy *m_strategy; ///< Pointer to the trading logic that will be used in the simulation

  public:
    /**
     * @brief Constructs a new simulation engine with initial capital and a strategy to be tested
     * @param startingCash The initial balance in USD
     * @param strategy A pointer to the trading strategy to be utilized
     */
    SimEngine(double startingCash, TradingStrategy *strategy);

    /**
     * @brief Executes the trading strategy across the given price dates
     * @param prices A vector of historical stock prices
     */
    void RunSimulator(const std::vector<double> &prices);

    /**
     * @brief Calculates total portfolio value at the given market price
     * @param currentPrice The current market price of the stock
     * @return The total value of the portfolio
     */
    double GetPortfolioValue(double currentPrice);
};