"""Gymnasium wrapper around the C++ SimEnv (build/backtest_env*.so must be importable)."""
import gymnasium as gym
import numpy as np
from gymnasium import spaces

import backtest_env


class BacktestEnv(gym.Env):
    """Actions: 0 = be flat, 1 = be long. Reward: log change in portfolio value after trading costs."""

    metadata = {"render_modes": []}

    def __init__(self, prices, episode_len=252, cost_rate=0.0005, starting_cash=10_000.0):
        super().__init__()
        self.prices = list(prices)
        self.warmup = backtest_env.SimEnv.warmup()
        if len(self.prices) < self.warmup + episode_len + 1:
            raise ValueError("price series too short for this episode_len")
        self.episode_len = episode_len
        self.sim = backtest_env.SimEnv(self.prices, starting_cash, cost_rate)
        self.action_space = spaces.Discrete(2)
        self.observation_space = spaces.Box(
            low=-5.0, high=5.0, shape=(backtest_env.SimEnv.num_features(),), dtype=np.float32
        )

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        hi = len(self.prices) - self.episode_len
        start = int(self.np_random.integers(self.warmup, hi + 1))
        obs = self.sim.reset(start, start + self.episode_len)
        return np.asarray(obs, dtype=np.float32), {}

    def step(self, action):
        obs, reward, done, value = self.sim.step(int(action))
        return np.asarray(obs, dtype=np.float32), float(reward), False, bool(done), {"value": value}


def evaluate(policy_fn, prices, cost_rate=0.0005, starting_cash=10_000.0):
    """Runs one full pass over `prices` and returns (final value, buy-and-hold value)."""
    sim = backtest_env.SimEnv(list(prices), starting_cash, cost_rate)
    start = backtest_env.SimEnv.warmup()
    obs = np.asarray(sim.reset(start, len(prices)), dtype=np.float32)
    value = starting_cash
    while True:
        obs, _, done, value = sim.step(int(policy_fn(obs)))
        obs = np.asarray(obs, dtype=np.float32)
        if done:
            break
    buy_hold = starting_cash * prices[-1] / prices[start]
    return value, buy_hold
