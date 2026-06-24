# Backtester

A high-performance C++ trading simulation engine designed to test various trading strategies against historical market data. This program is containerized with Docker and utilizes an AWS S3 data pipeline. 

## Why This Project

I built this trading simulation engine to connect learned academic data structures with real verifiable data.
I wanted to create an environment where I was able to test different data structures and observe their impact
on performance. I designed the AWS S3 data pipeline and containerized the system with Docker to gain practical experience with the distributed infrastructure and cloud architectures.

## Tech Stack

* **Language & Build System:** C++17, CMake
* **Networking & HTTP:** libcurl
* **Containerization:** Docker
* **Cloud Infrastructure (AWS):** Amazon S3, API Gateway, AWS Lambda, Amazon SNS, AWS Budgets
* **Data Sourcing:** Python (yfinance)

## System Architecture & Data Flow

1. **The Python Script:** A python script is utilized to download the past 5 years of OHLCV data from a specific stock.
2. **Cloud Storage:** The downloaded data is staged in S3. The data is read from a private S3 bucket into the API Gateway public proxy. To ensure proper cloud protection, the API Gateway is rate-limited to 2 requests/second before retrieved by the C++ using libcurl. By using this data pipeline, it ensures that other users do not need to manually setup their own AWS bucket and requires zero AWS credentials, zero IAM setup, and zero .env files.
3. **The C++ Engine:** The simulation engine retrieves the data from the S3 bucket using libcurl to make HTTP GET requests to the API gateway. 

## Cloud Security
AWS Budget, SNS Topic, and an AWS lambda function were used in tandem to automatically lock the bucket if costs exceed an unacceptable amount. In the case the data pricing exceeds the AWS Budget, an SNS notification is received which executes the AWS lambda function applying a deny-all S3 bucket policy.

## Features

**The Trading Strategies**
I designed the program to test two different trading strategies (with more coming in the future!).
* **Simple Moving Average (SMA):** The engine calculates the average stock price over a set window of days. If the current price crosses above that average, it buys. If the standing price is lower, the program sells.
* **Bollinger Bands + RSI:** A combination strategy that looks for cases when a stock is statistically overbought or oversold. Momentum is used in tandem to confirm if it is a good time to buy or sell.

**The Simulation Output**
After the simulation engine finishes its calculations it outputs:
* **Trading Stats:** Measures how well the algorithm performed in the market. ROI, win/loss ratio, maximum drawdown, and buy-and-hold benchmark are output.
* **Computer Performance:** Measures how efficiently the C++ code ran. Exact execution time in nanoseconds, peak memory usage, and how many millions of rows the engine processed per second are output.

## Initial Design & Early Decisions

- C++ was used instead of Python for faster performance and manual memory management. I wanted to interact with lower-level programming and practice its intricacies. 
- Performance was put at the forefront of this project. One example of this was my initial design to fully separate the CSV reading from the program. CSV reading was done all at once to ensure spatial locality could be leveraged in the future.

## How the Project Evolved

This project started as a single file, running off of a mock data CSV with a hardcoded Simple Moving Average strategy. This was not scalable and created an environment where change was hard to implement. At this current stage, this project features a fully containerized program that utilizes an AWS data pipeline ensuring accessibility regardless of user, two independent trading strategies are present with the ability to add more with ease, built-in performance tracking times to test program speeds down to the nanosecond, and automated cloud cost protection that locks the S3 data bucket in the case of excess costs.

- *Algorithm Refactoring:* Initially, the engine was much slower due to the average calculation's nested loop. This was refactored into an O(N) sliding window later on.
- *OOP:* Initially the program was inside a single file. This was changed incorporating OOP principles by distributing the program into multiple classes and files. This allowed for a much more pleasant experience when debugging, and it also allowed for easier implementation of new features.
- *Cloud Integration:* Originally, the program only worked with hardcoded local files. To make the system portable and scalable, I built a Python script to stage the data into an AWS S3 bucket. The program was also containerized in Docker for easier deployment.
- *Cloud Optimization:* The AWS C++ SDK was initially as a stepping stone into the AWS ecosystem. This was then replaced with libcurl to eliminate the need for AWS keys, which simplified the project and reduced the Docker build time.

## Future Work

- **Offline Testing:** Implement local file reading to ensure a seamless offline experience if AWS is down.
- **Multi-Asset Portfolio:** Expand the engine to simulate strategies across multiple assets and an entire portfolio to simulate actual personal situations.

## Project Structure
```text
.
├── .github/workflows/
│   └── ci.yml
├── data/
├── scripts/
│   └── getData.py
├── src/
│   ├── AWSConnection.cpp/.h
│   ├── main.cpp
│   ├── S_BollingerRSI.cpp/.h
│   ├── S_MovingAverage.cpp/.h
│   ├── SimCLI.cpp/.h
│   ├── SimEngine.cpp/.h
│   ├── StockParser.cpp/.h
│   └── TradingStrategy.h
├── .clang-format
├── .dockerignore
├── .gitignore
├── CMakeLists.txt
├── Dockerfile
├── README.md
└── requirements.txt
```
## Performance & Algorithmic Efficiency


I benchmarked the engine on my local machine using 5 years of daily OHLCV data for SPY:

| Implementation | Execution Time | Processing Speed |
| :--- | :--- | :--- |
| **Previous Iteration O(N²)** | 177,066 ns | 7.08 million rows/sec |
| **Optimized Loop O(N)** | 134,943 ns | 9.30 million rows/sec |

**Result:** The algorithmic optimization made the core simulation **1.31x faster**, allowing it to process over 9 million rows per second. Also ensures linear growth in performance as the data set grows instead of quadratic from using the previous method.

## Getting Started

The Backtester engine is containerized with Docker to ensure a seamless setup and identical execution across all machines.

### Prerequisites
* Docker must be installed and running on your machine.
* Zero AWS IAM credentials or `.env` files are required. 

### 1. Repository Setup
Clone the repository to your local machine and navigate into the directory:
```bash
git clone https://github.com/prsidharta/Backtesting-Engine.git
cd Backtesting-Engine
```

### Step 2 — Build the image
```bash
docker build -t backtesting-engine .
```

### Step 3 — Run it
```bash
docker run -it --rm backtesting-engine
```

### Sample Execution
```text

START CASH: 10000
Buy and Hold End Cash: 18957.52

--- Trade Simulation Stats ---
Win rate: 42.79%
Wins: 95
Losses: 127
Total Trades: 222
Profit Factor: 1.13
Total ROI: 18.99%
Max Drawdown: 20.02%
Sharpe Ratio: 0.37

--- Computer Performance Metrics ---
END CASH: $11898.82
Core Compute Time: 134943 ns
Engine Throughput: 9300223.06 rows/sec
Peak Memory Usage: 22.38 MB
```

### How to Add a New Strategy

The engine is designed around OOP principles to ensure adding new trading logic feels easy. To build a new strategy, inherit from the `TradingStrategy` interface and implement the signal generation method:

```cpp
//TradingStrategy.h
class TradingStrategy {
  public:
    virtual ~TradingStrategy() = default;

    //Buy = 1, Sell = -1, Hold = 0
    virtual int CreateSignal(double dayPrice, double currentShares) = 0;
};
```

## License
MIT