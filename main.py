import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("input.csv", sep=r", ", engine="python")

types = sorted(df["type"].unique())
for tp in types:
    d = df[df["type"] == tp].copy()

    # Первый
    plt.figure(figsize=(8, 5))
    d_merge = d[(d["algo"] == "merge") | (d["threshold"] < 0)].sort_values("size")
    plt.plot(d_merge["size"], d_merge["avg_ms"], label="merge", lw=2)

    ths = sorted(d.loc[d["algo"].str.contains("insertion"), "threshold"].unique())
    for th in ths:
        dh = d[(d["algo"].str.contains("insertion")) & (d["threshold"] == th)].sort_values("size")
        plt.plot(dh["size"], dh["avg_ms"], label=f"hybrid th={th}")
    plt.xlabel("Размер (n)")
    plt.ylabel("Время (ms)")
    plt.title(f"{tp}: время сортировки")
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"time for {tp}.png")
    plt.close()

    # Второй
    best_rows = []
    m_by_n = dict(zip(d_merge["size"], d_merge["avg_ms"]))
    for n in sorted(d["size"].unique()):
        dh_n = d[(d["algo"].str.contains("insertion")) & (d["size"] == n)]
        if not dh_n.empty and n in m_by_n:
            best_h = dh_n["avg_ms"].min()
            best_th = int(dh_n.loc[dh_n["avg_ms"].idxmin(), "threshold"])
            ratio = best_h / m_by_n[n] if m_by_n[n] > 0 else np.nan
            best_rows.append((n, best_th, best_h, m_by_n[n], ratio))
    best_df = pd.DataFrame(best_rows,
                           columns=["size", "best_threshold", "best_h_ms", "merge_ms", "ratio"])

    plt.figure(figsize=(8, 5))
    plt.plot(best_df["size"], best_df["ratio"], label="hybrid / merge")
    plt.axhline(1.0, color="k", ls="--")
    plt.xlabel("size (n)")
    plt.ylabel("Отношение a/b (меньше 1 - лучше hybrid)")
    plt.title(f"{tp}: выигрыш hybrid")
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"who best {tp}.png")
    plt.close()

    results = []
    for th in ths:
        dh = d[(d["algo"].str.contains("insertion")) & (d["threshold"] == th)][["size", "avg_ms"]]
        res = pd.merge(dh, d_merge[["size", "avg_ms"]], on="size", suffixes=("_hyb", "_mer"))
        if not res.empty:
            res["ratio"] = res["avg_ms_hyb"] / res["avg_ms_mer"]
            results.append((th, float(res["ratio"].mean())))

    th_sorted = sorted(results, key=lambda x: x[1])
    best_thr, best_med = th_sorted[0]
    print(f"{tp}: Лучший threshold = {best_thr} (среднее отношение={best_med:.3f})")
