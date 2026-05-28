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
    void RunSimulator(std::vector<double> &prices);
    double GetPortfolioValue(double currentPrice);
};