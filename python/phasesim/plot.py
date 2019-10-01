import numpy as np
import matplotlib.pyplot as plt

from .metrics import compute


def plot_roc(dfs, label="CPIPhaseDetector"):
    dfs = dfs.dropna()
    dfs = dfs.replace([np.inf], 1)

    results = {}
    for column in dfs:
        if column == label:
            continue
        results[column] = {
            "sensitivity": [],
            "false_positive": []
        }

    dfs[label] = dfs[label] > 0.02

    for threshold in np.linspace(0, 1, num=101):
        for column in dfs:
            if column == label:
                continue
            df = dfs.copy()
            df[column] = df[column] > threshold
            # results[str(threshold)] = compute(df, label)
            result = compute(df, label).replace([np.inf], 1)

            results[column]["sensitivity"].append(result["sensitivity"].loc[column])
            results[column]["false_positive"].append(result["false_positive"].loc[column])

    for column in dfs:
        if column == label:
            continue
        plt.plot(results[column]["false_positive"], results[column]["sensitivity"])

    return results
