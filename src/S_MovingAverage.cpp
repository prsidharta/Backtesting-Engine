#include "S_MovingAverage.h"

S_MovingAverage::S_MovingAverage(int windowSize) {
    m_windowSize = windowSize;
}

int S_MovingAverage::CreateSignal(double dayPrice, double currentShares) {

    /* OLD O(N^2)
    for (std::size_t pastDay = startIndex; pastDay < m_priceHistory.size(); pastDay++) {
        sum += m_priceHistory[pastDay];
    }
    */

    m_priceHistory.push_back(dayPrice);
    m_currentSum += dayPrice;

    if (m_priceHistory.size() < m_windowSize) {
        return 0;
    }

    if (m_priceHistory.size() > m_windowSize) {
        std::size_t oldestIndex = m_priceHistory.size() - m_windowSize - 1;
        m_currentSum -= m_priceHistory[oldestIndex];
    }

    double windowPrice = m_currentSum / m_windowSize;

    if (currentShares == 0 && dayPrice > windowPrice) {
        return 1; // buy
    } else if (currentShares != 0 && dayPrice < windowPrice) {
        return -1; // sell
    }

    return 0; // hold
}