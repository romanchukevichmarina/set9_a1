import csv
import os
import sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load_data(csv_path: str):

    data = {}
    with open(csv_path, 'r') as f:
        reader = csv.reader(f)
        header = next(reader)
        alg_names = []
        for i in range(1, len(header), 6):
            name_part = header[i].split('-')[0]
            alg_names.append(name_part)
            data[name_part] = {
                'RandomTime': [],
                'RandomCmp': [],
                'ReversedTime': [],
                'ReversedCmp': [],
                'NearlyTime': [],
                'NearlyCmp': []
            }
        for row in reader:
            size = int(row[0])
            col = 1
            for name in alg_names:
                t_random = float(row[col]); col += 1
                c_random = float(row[col]); col += 1

                t_rev = float(row[col]); col += 1
                c_rev = float(row[col]); col += 1

                t_near = float(row[col]); col += 1
                c_near = float(row[col]); col += 1
                data[name]['RandomTime'].append((size, t_random))
                data[name]['RandomCmp'].append((size, c_random))

                data[name]['ReversedTime'].append((size, t_rev))
                data[name]['ReversedCmp'].append((size, c_rev))

                data[name]['NearlyTime'].append((size, t_near))
                data[name]['NearlyCmp'].append((size, c_near))
    return data


def plot_data(data, output_dir='.'):
    plt.rcParams.update({'font.family': 'DejaVu Sans'})

    names = {
        'Random': 'Random',
        'Reversed': 'Reversed',
        'Nearly': 'NearlySorted'
    }

    for alg_name, records in data.items():
        plt.figure()

        for key, label in names.items():
            series = records[key + 'Time']
            sizes = [x for x, y in series]
            times = [y for x, y in series]
            plt.plot(sizes, times, marker='o', label=label)

        plt.title(f"Runtime of {alg_name}")
        plt.xlabel("Number of strings")
        plt.ylabel("Time (seconds)")
        plt.legend()
        plt.grid(True)
        plt.savefig(os.path.join(output_dir, f"{alg_name}_runtime.png"))
        plt.close()

        plt.figure()

        for key, label in names.items():
            series = records[key + 'Cmp']
            sizes = [x for x, y in series]
            comps = [y for x, y in series]
            plt.plot(sizes, comps, marker='o', label=label)

        plt.title(f"Character comparisons of {alg_name}")
        plt.xlabel("Number of strings")
        plt.ylabel("Character comparisons")
        plt.legend()
        plt.grid(True)
        plt.savefig(os.path.join(output_dir, f"{alg_name}_comparisons.png"))
        plt.close()

def main():
    csv_path = 'results.csv'
    if len(sys.argv) > 1:
        csv_path = sys.argv[1]
    if not os.path.isfile(csv_path):
        print(f"CSV file '{csv_path}' not found.")
        return
    data = load_data(csv_path)
    plot_data(data, '.')


if __name__ == '__main__':
    main()