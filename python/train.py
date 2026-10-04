"""Train PPO on one ticker's CSV, validate on later data, export weights for the C++ engine."""
import sys

import numpy as np
from stable_baselines3 import PPO
from stable_baselines3.common.env_util import make_vec_env

import backtest_env
from backtest_gym import BacktestEnv, evaluate
from export_policy import export_policy

TICKER = sys.argv[1] if len(sys.argv) > 1 else "SPY"
prices = backtest_env.read_csv(f"data/{TICKER}.csv")

n = len(prices)
train, val, test = prices[: int(n * 0.70)], prices[int(n * 0.70) - 30 : int(n * 0.85)], prices[int(n * 0.85) - 30 :]
print(f"{TICKER}: {len(train)} train / {len(val)} val / {len(test)} test bars")

env = make_vec_env(lambda: BacktestEnv(train, episode_len=200), n_envs=4, seed=0)
model = PPO("MlpPolicy", env, n_steps=512, batch_size=256, gamma=0.99, ent_coef=0.01, seed=0, verbose=1)
model.learn(total_timesteps=300_000)

policy = lambda obs: model.predict(obs, deterministic=True)[0]
for name, split in [("train", train), ("val", val)]:
    value, bh = evaluate(policy, split)
    print(f"{name}: agent ${value:,.0f}  vs buy&hold ${bh:,.0f}")

export_policy(model, "models/PPO.txt")
print("wrote models/PPO.txt")
