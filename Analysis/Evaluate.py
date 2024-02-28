import os
import pandas as pd
import matplotlib.pyplot as plt
import re


def main():
    path = "Benchmarks"
    seq_df, para_df = import_csv(path)

    print(seq_df.keys())
    ds1_seq = seq_df.loc[seq_df['Upper'] == 15000]
    ds2_seq = seq_df.loc[seq_df["Upper"] == 30000]
    ds3_seq = seq_df.loc[seq_df["Upper"] == 100000]

    ds1_para = para_df.loc[para_df["Upper"] == 15000]
    ds2_para = para_df.loc[para_df["Upper"] == 30000]
    ds3_para = para_df.query('`Upper` == 100000')

    ds3 = ds3_para.query('`Scheduling Strategy` == "static" & `Core Count` == 32')
    ds3.sort_values(by="Chunk Size", ascending=True)

    plt.figure()
    plt.xlabel("Chunk Size")
    plt.ylabel("Execution Time (s)")
    plt.plot(ds3["Chunk Size"], ds3["Execution Time"], label="Core Count vs Execution Time - Static, CS=1")
    plt.show()


def sort_cores(df):
    return df.sort_values(by="Core Count", ascending=True)


def sort_runtime(df):
    return df.sort_values(by="Execution Time", ascending=True)


def get_sched(df, type):
    match type:
        case "static":
            return df.loc[df['Scheduling Strategy'] == "static"]
        case "dynamic":
            return df.loc[df['Scheduling Strategy'] == "dynamic"]
        case "guided":
            return df.loc[df["Scheduling Strategy"] == "guided"]


def get_chunk(df, size):
    match size:
        case 1:
            return df.loc[df['Chunk Size'] == 1]


def import_csv(path: str):
    def clean_column_names(df):
        # Strip whitespace and remove quotation marks from column names
        df.columns = df.columns.str.strip().str.replace('"', '').str.replace('\'', '')
        return df

    def clean_strings(df):
        df = df.map(lambda x: x.strip().replace('"', '').replace('\'', '') if isinstance(x, str) else x)
        return df

    files = os.listdir(path)
    seq_df = []
    para_df = []

    # regex
    seq_pattern = r"seq_benchmark_ds(\d+)_gcd(\d+).csv"
    para_pattern = r"para_benchmark_ds(\d+).csv"

    for file in files:
        if file.endswith(".csv"):
            if re.match(seq_pattern, file):
                seq_df.append(pd.read_csv(os.path.join(path, file), dtype={'Scheduling Strategy': str}))
                print("Added Seq")
            elif re.match(para_pattern, file):
                para_df.append(pd.read_csv(os.path.join(path, file)))
                print("Added Para")
            else:
                print(f"Unknown file format: {file}")

    return clean_strings(clean_column_names(pd.concat(seq_df, ignore_index=True))), clean_strings(clean_column_names(
        pd.concat(para_df, ignore_index=True)))


main()
