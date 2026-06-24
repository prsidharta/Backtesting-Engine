cmake_minimum_required(VERSION 3.10)
project(BacktestingEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)

add_executable(BacktestingEngine 
    src/main.cpp 
    src/SimEngine.cpp 
    src/StockParser.cpp 
    src/S_MovingAverage.cpp
    src/S_BollingerRSI.cpp
    src/SimCLI.cpp
    src/AWSConnection.cpp
)

target_link_libraries(BacktestingEngine PRIVATE CURL::libcurl)