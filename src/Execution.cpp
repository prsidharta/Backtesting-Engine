#include "Execution.h"

bool ApplySignal(double &cash, double &shares, int signal, double price, double costRate,
                 std::vector<TradingRecord> *ledger) {
    if (signal == 1 && shares == 0.0) {
        shares = (cash * (1.0 - costRate)) / price;
        cash = 0.0;
        if (ledger) {
            ledger->emplace_back("BUY", price, shares);
        }
        return true;
    }
    if (signal == -1 && shares > 0.0) {
        double sharesSold = shares;
        cash += shares * price * (1.0 - costRate);
        shares = 0.0;
        if (ledger) {
            ledger->emplace_back("SELL", price, sharesSold);
        }
        return true;
    }
    return false;
}
