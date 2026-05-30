cmake_minimum_required(VERSION 3.10)

project(Backtester CXX)

add_executable(BacktestingEngine main.cpp SimEngine.cpp StockParser.cpp S_MovingAverage.cpp)