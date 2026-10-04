cmake_minimum_required(VERSION 3.10)
project(BacktestingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(CURL REQUIRED)


add_library(engine_core STATIC
    src/SimEngine.cpp
    src/Execution.cpp
    src/StockParser.cpp
    src/S_MovingAverage.cpp
    src/S_BollingerRSI.cpp
    src/FeatureBuilder.cpp
    src/SimEnv.cpp
    src/S_PPO.cpp
)
target_include_directories(engine_core PUBLIC src)

add_executable(BacktestingEngine
    src/main.cpp
    src/SimCLI.cpp
    src/AWSConnection.cpp
)
target_link_libraries(BacktestingEngine PRIVATE engine_core CURL::libcurl)


option(BUILD_PYTHON_MODULE "Build the pybind11 backtest_env module" OFF)
if(BUILD_PYTHON_MODULE)
    find_package(pybind11 CONFIG REQUIRED)
    pybind11_add_module(backtest_env src/bindings.cpp)
    target_link_libraries(backtest_env PRIVATE engine_core)
endif()



