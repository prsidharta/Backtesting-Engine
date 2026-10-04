"""Writes an SB3 PPO policy (the actor: policy MLP + action head) in the text format PPOStrategy loads."""
import torch.nn as nn


def export_policy(model, path):
    p = model.policy
    layers = [m for m in p.mlp_extractor.policy_net if isinstance(m, nn.Linear)] + [p.action_net]
    with open(path, "w") as f:
        f.write(f"{len(layers)}\n")
        for layer in layers:
            w = layer.weight.detach().cpu().numpy()
            b = layer.bias.detach().cpu().numpy()
            f.write(f"{w.shape[1]} {w.shape[0]}\n")
            f.write(" ".join(f"{x:.9g}" for x in w.flatten()) + "\n")
            f.write(" ".join(f"{x:.9g}" for x in b) + "\n")
