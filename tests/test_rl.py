import unittest
import numpy as np
from stable_baselines3.common.env_checker import check_env
from backtest_gym import BacktestEnv


class EnvironmentTests(unittest.TestCase):
    def test_contract(self):
        check_env(BacktestEnv(np.linspace(100, 120, 100), episode_len=20))

    def test_cost_and_lifecycle(self):
        env = BacktestEnv([100.0] * 50, episode_len=3, cost_rate=0.01)
        env.reset(seed=0, options={"start": 20})
        _, reward, _, done, info = env.step(1)
        self.assertAlmostEqual(info["value"], 9900.0)
        self.assertAlmostEqual(reward, np.log(0.99))
        self.assertFalse(done)
        self.assertTrue(env.step(0)[3])
        with self.assertRaises(Exception):
            env.step(0)

    def test_seed(self):
        env = BacktestEnv(np.linspace(100, 150, 100), episode_len=20)
        np.testing.assert_array_equal(env.reset(seed=42)[0], env.reset(seed=42)[0])


if __name__ == "__main__":
    unittest.main()
