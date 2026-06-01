import yfinance as yf
import os
import sys

def fetch_data(ticker="SPY", period="5y"):
    print(f"[*] Initalizing connection to financial servers...")
    print(f"[*] Fetching {period} of historical data for {ticker}...")

    data = yf.download(ticker, period=period)

    if data.empty:
        print("[!] Error: Failed to fetch data. Check your connection or ticker symbol.")
        sys.exit(1)
    
    os.makedirs("data", exist_ok=True)

    file_path = f"data/{ticker}.csv"
    data.to_csv(file_path)

    print(f"[+] Success. Wrote {len(data)} trading days to {file_path}")

if __name__ == "__main__":
    fetch_data()