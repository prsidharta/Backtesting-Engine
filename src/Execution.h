#pragma once

#include <string>
#include <vector>

/**
 * @struct TradingRecord
 * @brief One executed trade (moved here from SimEngine.h so every code path shares it)
 */
struct TradingRecord {
    std::string type; ///< "BUY" or "SELL"
    double price;     ///< Price per share at time of trade
    double shares;    ///< Shares bought or sold

    TradingRecord(const std::string givenType, double givenPrice, double givenShares) {
        type = givenType;
        price = givenPrice;
        shares = givenShares;
    }
};

/**
 * @brief The single definition of how a signal changes the portfolio
 * @details Used by SimEngine::Simulate (batch) and SimEnv::Step (RL) so both paths always agree.
 * Long-only, all-in / all-out. A proportional cost is charged on every trade.
 * @param cash Cash balance (modified in place)
 * @param shares Shares held (modified in place)
 * @param signal 1 = buy, -1 = sell, 0 = hold
 * @param price Execution price
 * @param costRate Fraction of traded value lost per trade (0.0005 = 5 bps)
 * @param ledger Optional trade log to append to
 * @return true if a trade was executed
 */
bool ApplySignal(double &cash, double &shares, int signal, double price, double costRate,
                 std::vector<TradingRecord> *ledger = nullptr);
