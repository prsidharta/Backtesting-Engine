#include "SimEnv.h"
#include "Execution.h"
#include <cmath>
#include <stdexcept>

SimEnv::SimEnv(std::vector<double> prices, double startingCash, double costRate)
    : m_prices(std::move(prices)), m_startingCash(startingCash), m_costRate(costRate) {
    if (m_prices.size() < FeatureBuilder::kWarmup + 3) {
        throw std::invalid_argument("SimEnv: not enough prices");
    }
    for (double p : m_prices) {
        if (p <= 0.0) {
            throw std::invalid_argument("SimEnv: prices must all be positive");
        }
    }
}

std::vector<double> SimEnv::Reset(size_t start, size_t end) {
    if (start < FeatureBuilder::kWarmup || end > m_prices.size() || end < start + 2) {
        throw std::invalid_argument("SimEnv::Reset: invalid [start, end) window");
    }
    m_cash = m_startingCash;
    m_shares = 0.0;
    m_entryPrice = 0.0;
    m_end = end;
    m_t = start;

    // Replay the warm-up bars so indicators are valid on the very first decision
    m_features.Reset();
    for (size_t i = start - FeatureBuilder::kWarmup; i <= start; i++) {
        m_features.Update(m_prices[i]);
    }
    return Observe();
}

std::vector<double> SimEnv::Observe() const {
    const double price = m_prices[m_t];
    const double unrealized = (m_shares > 0.0 && m_entryPrice > 0.0) ? price / m_entryPrice - 1.0 : 0.0;
    return m_features.Features(m_shares > 0.0 ? 1.0 : 0.0, unrealized);
}

StepResult SimEnv::Step(int action) {
    if (m_t + 1 >= m_end) {
        throw std::logic_error("SimEnv::Step: episode is over, call Reset()");
    }
    const double price = m_prices[m_t];
    const double before = Value(price);

    int signal = 0;
    if (action == 1 && m_shares == 0.0) {
        signal = 1;
    } else if (action == 0 && m_shares > 0.0) {
        signal = -1;
    }
    if (ApplySignal(m_cash, m_shares, signal, price, m_costRate)) {
        m_entryPrice = (signal == 1) ? price : 0.0;
    }

    m_t++;
    m_features.Update(m_prices[m_t]);
    const double after = Value(m_prices[m_t]);

    StepResult result;
    result.reward = std::log(after / before);
    result.done = (m_t + 1 >= m_end);
    result.portfolioValue = after;
    return result;
}
