import pandas as pd


def correlation(dfs):
    return pd.DataFrame.from_records(dfs).corr()


def compute(dfs, label="CPIPhaseDetector"):
    results = {}
    for detector in dfs:
        num_stable = 0
        num_phases = 0

        # Number of actual phase changes (using label)
        num_changed = 0
        num_unchanged = 0

        # True/false positive/negative
        num_tp = 0
        num_tn = 0
        num_fp = 0
        num_fn = 0

        prev_value = 0
        for actual, predicted in zip(dfs[label].items(),
                                           dfs[detector].items()):

            pindex, pvalue = predicted
            aindex, avalue = actual

            # New phase
            if avalue == 1:
                num_changed += 1

                if pvalue == 1:
                    num_tp += 1
                else:
                    num_fn += 1

            # Not new phase
            else:
                num_unchanged += 1

                if pvalue == 1:
                    num_fp += 1
                else:
                    num_tn += 1

            # Stable if not a new phase
            if pvalue == 0:
                num_stable += 1

                # Previous also stable if these are first two stable in a phase
                if prev_value == 1:
                    num_stable += 1
                    num_phases += 1

            prev_value = pvalue
        results[detector] = {
            "num_stable": num_stable,
            "num_phases": num_phases,
            "avg_length": num_stable / num_phases if num_phases > 0 else 0,
            "num_changed": num_changed,
            "num_unchanged": num_unchanged,
            "num_tp": num_tp,
            "num_tn": num_tn,
            "num_fp": num_fp,
            "num_fn": num_fn,
            "sensitivity": num_tp / num_changed,
            "false_positive": num_fp / num_unchanged
        }

    return pd.DataFrame.from_dict(results, orient="index")
