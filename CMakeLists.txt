cmake_minimum_required(VERSION 3.10)

project(Backtester CXX)

add_executable(BacktestingEngine 
    src/main.cpp 
    src/SimEngine.cpp 
    src/StockParser.cpp 
    src/S_MovingAverage.cpp
    src/S_BollingerRSI.cpp
)