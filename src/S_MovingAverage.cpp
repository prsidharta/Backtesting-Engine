#include "S_MovingAverage.h"

S_MovingAverage::S_MovingAverage(int windowSize) {
    m_windowSize = windowSize;
}

int S_MovingAverage::CreateSignal(double dayPrice, double currentShares) {

    m_priceHistory.push_back(dayPrice);

    if (m_priceHistory.size() < m_windowSize) {
        return 0;
    }

    double sum = 0;
    std::size_t startIndex = m_priceHistory.size() - m_windowSize;

    for (std::size_t pastDay = startIndex; pastDay < m_priceHistory.size(); pastDay++) {
        sum += m_priceHistory[pastDay];
    }

    double windowPrice = sum / m_windowSize;

    if (currentShares == 0 && dayPrice > windowPrice) {
        return 1; // buy
    } else if (currentShares != 0 && dayPrice < windowPrice) {
        return -1; // sell
    }
    return 0; // hold
}