"""Train or evaluate Python PPO policies against the compiled C++ simulator."""

import argparse
import csv
import hashlib
import json
from pathlib import Path

import numpy as np
from stable_baselines3 import PPO
from stable_baselines3.common.callbacks import BaseCallback
from stable_baselines3.common.env_util import make_vec_env
from backtest_gym import BacktestEnv


def load_prices(path):
    records = []
    with open(path, newline="") as stream:
        reader = csv.reader(stream)
        header = next(reader)
        close = header.index("Close")
        for row in reader:
            if not row or row[0] in ("Ticker", "Date"):
                continue
            from datetime import date

            day = date.fromisoformat(row[0])
            price = float(row[close])
            if (
                not np.isfinite(price)
                or price <= 0
                or (records and day <= records[-1][0])
            ):
                raise ValueError(
                    "prices must be positive and dates strictly increasing"
                )
            records.append((day, price))
    return records


class PeriodicCheckpoint(BaseCallback):
    """Intervals count aggregate environment transitions, rounded to vector steps."""

    def __init__(self, every, output):
        super().__init__()
        self.every, self.output, self.next_save = every, output, every

    def _on_step(self):
        if self.num_timesteps >= self.next_save:
            self.model.save(self.output / f"checkpoint_{self.num_timesteps}")
            self.next_save = (self.num_timesteps // self.every + 1) * self.every
        return True


def evaluate(model, prices, config, output, name):
    env = BacktestEnv(
        prices,
        episode_len=len(prices) - 20,
        starting_cash=config["starting_cash"],
        cost_rate=config["cost_rate"],
    )
    obs, _ = env.reset(seed=config["seed"], options={"start": 20})
    values = [config["starting_cash"]]
    while True:
        action, _ = model.predict(obs, deterministic=True)
        obs, _, _, done, info = env.step(action)
        values.append(info["value"])
        if done:
            break
    equity = np.asarray(values)
    returns = np.diff(equity) / equity[:-1]
    summary = dict(
        final_value=float(equity[-1]),
        return_pct=float((equity[-1] / equity[0] - 1) * 100),
        max_drawdown_pct=float(
            np.max(1 - equity / np.maximum.accumulate(equity)) * 100
        ),
        sharpe=float(returns.mean() / returns.std() * np.sqrt(252))
        if returns.std()
        else 0.0,
        buy_hold_value=float(
            equity[0] * (1 - config["cost_rate"]) * prices[-1] / prices[20]
        ),
    )
    np.savetxt(
        output / f"{name}_equity.csv",
        equity,
        delimiter=",",
        header="value",
        comments="",
    )
    return summary


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("--config", default="python/config.json")
    parser.add_argument("--evaluate", metavar="CHECKPOINT")
    args = parser.parse_args()
    config = json.loads(Path(args.config).read_text())
    if config["checkpoint_every"] <= 0 or config["n_envs"] <= 0:
        raise ValueError("checkpoint_every and n_envs must be positive")
    records = load_prices(config["data_path"])
    prices = [r[1] for r in records]
    n = len(prices)
    a = int(n * config["train_fraction"])
    b = int(n * (config["train_fraction"] + config["validation_fraction"]))
    if not 20 < a < b < n or min(b - a, n - b) < 2:
        raise ValueError("insufficient data or invalid split fractions")
    splits = {
        "train": prices[:a],
        "validation": prices[a - 20 : b],
        "test": prices[b - 20 :],
    }
    output = Path(config["output_dir"])
    output.mkdir(parents=True, exist_ok=True)
    if args.evaluate:
        model = PPO.load(args.evaluate)
    else:
        env = make_vec_env(
            lambda: BacktestEnv(
                splits["train"],
                episode_len=config["episode_len"],
                cost_rate=config["cost_rate"],
                starting_cash=config["starting_cash"],
            ),
            n_envs=config["n_envs"],
            seed=config["seed"],
        )
        model = PPO("MlpPolicy", env, seed=config["seed"], verbose=1, **config["ppo"])
        metadata = dict(
            config=config,
            feature_version=1,
            data_sha256=hashlib.sha256(
                Path(config["data_path"]).read_bytes()
            ).hexdigest(),
            validation_start=str(records[a][0]),
            test_start=str(records[b][0]),
        )
        (output / "metadata.json").write_text(json.dumps(metadata, indent=2))
        model.learn(
            total_timesteps=config["total_timesteps"],
            callback=PeriodicCheckpoint(config["checkpoint_every"], output),
        )
        model.save(output / "final")
        env.close()
    results = {
        name: evaluate(model, data, config, output, name)
        for name, data in splits.items()
    }
    (output / "evaluation.json").write_text(json.dumps(results, indent=2))
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
