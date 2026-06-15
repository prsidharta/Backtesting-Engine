/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Intermediate stage of backtesting engine.
 * Currently has two strategies (Moving Average and RSI + Bollinger Bands) implemented.
 * Need to connect program to online buckets for data retrieval
 */

#include "SimCLI.h"
#include <aws/core/Aws.h>
#include <exception>
#include <iostream>

int main() {

    Aws::SDKOptions options;
    Aws::InitAPI(options);
    
    SimCLI app;
    app.Run();
    
    Aws::ShutdownAPI(options);
    return 0;
}