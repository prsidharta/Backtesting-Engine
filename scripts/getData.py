import yfinance as yf

spy = yf.Ticker("SPY")

data = spy.history(period="5y")

data.to_csv("SPY.csv")