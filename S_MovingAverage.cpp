#include "S_MovingAverage.h"

int S_MovingAverage::CreateSignal(double dayPrice, double windowPrice, double currentShares) {
    if (currentShares == 0 && dayPrice > windowPrice) {
        return 1; // buy
    } else if (currentShares != 0 && dayPrice < windowPrice) {
        return -1; // sell
    }
    return 0; // hold
}