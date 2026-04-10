#!/usr/bin/env python3

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


INPUT_FILE = "hash_collisions.csv"
OUTPUT_DIR = "hash_plots"


def load_data(csv_path: Path) -> pd.DataFrame:
    data = pd.read_csv(csv_path)

    required_columns = {
        "hash_name",
        "string_count",
        "series_index",
        "collision_count",
    }

    missing_columns = required_columns - set(data.columns)

    if missing_columns:
        missing = ", ".join(sorted(missing_columns))
        raise ValueError(f"Missing columns: {missing}")

    return data


def build_summary(data: pd.DataFrame) -> pd.DataFrame:
    summary = (
        data.groupby("hash_name")["collision_count"]
        .agg(["mean", "median", "max", "sum"])
        .reset_index()
        .rename(
            columns={
                "mean": "mean_collisions",
                "median": "median_collisions",
                "max": "max_collisions",
                "sum": "total_collisions",
            }
        )
        .sort_values(
            by=[
                "mean_collisions",
                "max_collisions",
                "total_collisions",
                "hash_name",
            ]
        )
        .reset_index(drop=True)
    )

    return summary


def save_summary_csv(summary: pd.DataFrame, output_dir: Path) -> None:
    summary.to_csv(output_dir / "summary.csv", index=False)


def plot_mean_curves(data: pd.DataFrame, output_dir: Path) -> None:
    mean_curves = (
        data.groupby(["hash_name", "string_count"])["collision_count"]
        .mean()
        .reset_index()
    )

    plt.figure(figsize=(12, 7))

    for hash_name in sorted(mean_curves["hash_name"].unique()):
        part = mean_curves[mean_curves["hash_name"] == hash_name]
        plt.plot(
            part["string_count"],
            part["collision_count"],
            label=hash_name,
        )

    plt.xlabel("Number of strings")
    plt.ylabel("Average collisions")
    plt.title("Average collisions vs number of strings")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_dir / "mean_collisions.png", dpi=200)
    plt.close()


def plot_max_curves(data: pd.DataFrame, output_dir: Path) -> None:
    max_curves = (
        data.groupby(["hash_name", "string_count"])["collision_count"]
        .max()
        .reset_index()
    )

    plt.figure(figsize=(12, 7))

    for hash_name in sorted(max_curves["hash_name"].unique()):
        part = max_curves[max_curves["hash_name"] == hash_name]
        plt.plot(
            part["string_count"],
            part["collision_count"],
            label=hash_name,
        )

    plt.xlabel("Number of strings")
    plt.ylabel("Maximum collisions")
    plt.title("Maximum collisions vs number of strings")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_dir / "max_collisions.png", dpi=200)
    plt.close()


def plot_total_bars(summary: pd.DataFrame, output_dir: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.bar(summary["hash_name"], summary["total_collisions"])
    plt.xlabel("Hash function")
    plt.ylabel("Total collisions")
    plt.title("Total collisions by hash function")
    plt.grid(True, axis="y")
    plt.tight_layout()
    plt.savefig(output_dir / "total_collisions.png", dpi=200)
    plt.close()


def main() -> None:
    script_dir = Path(__file__).resolve().parent
    csv_path = script_dir / INPUT_FILE
    output_dir = script_dir / OUTPUT_DIR
    output_dir.mkdir(exist_ok=True)

    data = load_data(csv_path)
    summary = build_summary(data)

    save_summary_csv(summary, output_dir)
    plot_mean_curves(data, output_dir)
    plot_max_curves(data, output_dir)
    plot_total_bars(summary, output_dir)

    print("Plots saved to:", output_dir)
    print("Summary saved to:", output_dir / "summary.csv")


if __name__ == "__main__":
    main()