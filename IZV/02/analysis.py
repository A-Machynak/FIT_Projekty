#!/usr/bin/env python3.9
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import zipfile

# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na
# prednaskach dalsi knihovny pak na dotaz

# Ukol 1: nacteni dat ze ZIP souboru


def load_data(filename: str) -> pd.DataFrame:
    # tyto konstanty nemente, pomuzou vam pri nacitani
    headers = [
        "p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8",
        "p9", "p10", "p11", "p12", "p13a", "p13b", "p13c", "p14", "p15",
        "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27",
        "p28", "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49",
        "p50a", "p50b", "p51", "p52", "p53", "p55a", "p57", "p58", "a", "b",
        "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q",
        "r", "s", "t", "p5a"
    ]
    regions = {
        "PHA": "00",
        "STC": "01",
        "JHC": "02",
        "PLK": "03",
        "ULK": "04",
        "HKK": "05",
        "JHM": "06",
        "MSK": "07",
        "OLK": "14",
        "ZLK": "15",
        "VYS": "16",
        "PAK": "17",
        "LBK": "18",
        "KVK": "19",
    }
    csv_data = []
    with zipfile.ZipFile(filename, 'r') as root_zf:
        for file_name in root_zf.filelist:
            with zipfile.ZipFile(root_zf.open(file_name), 'r') as file_zf:
                for k, v in regions.items():
                    read_data = pd.read_csv(file_zf.open(v + ".csv"),
                                            names=headers,
                                            encoding='cp1250',
                                            sep=';',
                                            low_memory=False)
                    read_data.insert(len(headers), "region", k)

                    csv_data.append(read_data)
    return pd.concat(csv_data, ignore_index=True)


# Ukol 2: zpracovani dat


def parse_data(df: pd.DataFrame, verbose: bool = False) -> pd.DataFrame:
    newDf = pd.DataFrame(df, copy=True)

    newDf = newDf.drop_duplicates(subset="p1")
    newDf = newDf.astype('category')
    newDf.insert(len(newDf.columns), "date", '1970-01-01')
    newDf["date"] = pd.to_datetime(newDf["p2a"])
    # Do 5. úlohy
    newDf['p13a'] = newDf['p13a'].astype('int')
    newDf['p13b'] = newDf['p13b'].astype('int')
    newDf['p13c'] = newDf['p13c'].astype('int')

    if verbose:
        print("orig_size={:.1f} MB".format(
            float(df.memory_usage(deep=True).sum() / 10**6)
        ))
        print("new_size={:.1f} MB".format(
            float(newDf.memory_usage(deep=True).sum() / 10**6)
        ))
    return newDf

# Ukol 3: počty nehod v jednotlivých regionech podle viditelnosti


def plot_visibility(df: pd.DataFrame, fig_location: str = None,
                    show_figure: bool = False):
    selected_regions = ['PHA', 'JHM', 'MSK', 'ZLK']

    fig, axs = plt.subplots(2, 2, figsize=(12, 9))
    fig.suptitle("Počet nehod dle viditelnosti")
    grp = df.groupby(['region', 'p19']).size()
    region_data = [
        [
            grp[s_r][1],
            grp[s_r][2] + grp[s_r][3],
            grp[s_r][4] + grp[s_r][6],
            grp[s_r][5] + grp[s_r][7]
        ] for s_r in selected_regions
    ]
    region_titles = [
        'Hl. m. Praha', 'Jihomoravský', 'Moravskoslezský', 'Zlínský'
    ]
    labels = [
        "Viditelnost ve dne - nezhoršená", "Viditelnost ve dne - zhoršená",
        "Viditelnost v noci - nezhoršená", "Viditelnost v noci - zhoršená",
    ]
    colors = ["#096AF3", "#DBB40C", "#A9561E", "#F97306"]

    for i, ax in enumerate(axs.reshape(-1)):
        counts = [
            region_data[0][i], region_data[1][i],
            region_data[2][i], region_data[3][i],
        ]
        bars = ax.bar(region_titles, counts, color=colors)
        ax.bar_label(bars, fontweight='bold')
        ax.set_title(labels[i], fontweight='bold')
        ax.set_ylabel('Počet nehod')
        ax.set_xlabel('Kraj')
    fig.tight_layout()
    if fig_location:
        fig.savefig(fig_location)
    if show_figure:
        fig.show()

# Ukol4: druh srážky jedoucích vozidel


def plot_direction(df: pd.DataFrame, fig_location: str = None,
                   show_figure: bool = False):
    selected_regions = ['PHA', 'JHM', 'MSK', 'ZLK']

    fig, axs = plt.subplots(2, 2, figsize=(12, 8))
    fig.suptitle("Počet nehod dle kraje")
    grp = df.groupby(['region', 'p7', df.date.dt.month]).size()

    region_data = [grp[s_r] for s_r in selected_regions]
    region_titles = [
        'Hl. m. Praha', 'Jihomoravský',
        'Moravskoslezský', 'Zlínský'
    ]

    x = np.arange(1, 13)
    width = 0.25
    for i, ax in enumerate(axs.reshape(-1)):
        ax.grid(True, linestyle='-', color='white', axis='y')
        ax.set_axisbelow(True)
        ax.set_facecolor('#F5F5F5')

        ax.bar(x - width, region_data[i][1], width, label='čelní',
               color='#069AF3')
        ax.bar(x, region_data[i][2], width, label='boční', color='#D2691E')
        ax.bar(x + width, region_data[i][3], width, label='zezadu',
               color='#94CC94')

        ax.set_xticks(x)
        ax.set_title(region_titles[i], fontweight='bold')
        ax.set_ylabel('Počet nehod')
        ax.set_xlabel('Měsíc')
        ax.set_xbound(1-width*2, 12 + width*2)

    fig.legend(['čelní', 'boční', 'zezadu'], bbox_to_anchor=(1.1, 0.575),
               title='Druh srážky')

    fig.tight_layout()
    if fig_location:
        fig.savefig(fig_location, pad_inches=0.1, bbox_inches='tight')
    if show_figure:
        fig.show()

# Ukol 5: Následky v čase


def plot_consequences(df: pd.DataFrame, fig_location: str = None,
                      show_figure: bool = False):
    selected_regions = ['PHA', 'JHM', 'MSK', 'ZLK']
    region_titles = [
        'Hl. m. Praha', 'Jihomoravský',
        'Moravskoslezský', 'Zlínský'
    ]

    fig, axs = plt.subplots(2, 2, figsize=(12, 8))
    fig.suptitle("Počet nehod dle kraje")

    # Vybereme pouze to, co potřebujeme
    filtered = df[df['region'].isin(selected_regions)]
    [['region', 'date', 'p13a', 'p13b', 'p13c']]

    filtered = filtered.rename(columns={
        "p13a": "Usmrceno",
        "p13b": "Těžce zraněno",
        "p13c": "Lehce zraněno"
    })

    # Bohužel nemám nejmenší tušení, jak to udělat lépe.
    # Už jsem na tom strávil příliš dlouho...
    table = pd.pivot_table(
        filtered,
        index=[filtered.date.dt.year, filtered.date.dt.month],
        values=['Usmrceno', 'Těžce zraněno', 'Lehce zraněno'],
        columns='region',
        aggfunc='sum',
        observed=True
    )
    # table_stacked = table.stack()

    x_values = np.arange(
        np.datetime64("2016-01"),
        np.datetime64("2022-01"),
        np.timedelta64(1, "M")
    )

    for i, ax in enumerate(axs.reshape(-1)):
        y_values = [
            table['Lehce zraněno'][selected_regions[i]].values,
            table['Těžce zraněno'][selected_regions[i]].values,
            table['Usmrceno'][selected_regions[i]].values
        ]
        ax.grid(True, linestyle='-', color='lightgray')
        ax.set_facecolor('#F5F5F5')

        import matplotlib.dates as mdates
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%m/%y'))
        ax.set_ylabel('Počet nehod')
        ax.set_title("Kraj: " + region_titles[i], fontweight='bold')
        ax.set_xlim(np.datetime64("2016-01"), np.datetime64("2022-01"))

        ax.plot(x_values, y_values[0], label='Lehce zraněno')
        ax.plot(x_values, y_values[1], label='Těžce zraněno')
        ax.plot(x_values, y_values[2], label='Usmrceno')

    fig.legend(
        ['Lehké zranění', 'Těžké zranění', 'Usmrcení'],
        bbox_to_anchor=(1.125, 0.5),
        loc='center right',
        title='Následky'
    )
    fig.tight_layout()

    if fig_location:
        fig.savefig(fig_location, pad_inches=0.1, bbox_inches='tight')
    if show_figure:
        fig.show()


if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni
    # funkce.
    df = load_data("data/data.zip")
    df2 = parse_data(df, True)

    plot_visibility(df2, "01_visibility.png")
    plot_direction(df2, "02_direction.png", True)
    plot_consequences(df2, "03_consequences.png")


# Poznamka:
# pro to, abyste se vyhnuli castemu nacitani muzete vyuzit napr
# VS Code a oznaceni jako bunky (radek #%%% )
# Pak muzete data jednou nacist a dale ladit jednotlive funkce
# Pripadne si muzete vysledny dataframe ulozit nekam na disk (pro ladici
# ucely) a nacitat jej naparsovany z disku
