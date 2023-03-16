#!/usr/bin/python3.10
# coding=utf-8

"""Modul pro projekt do předmětu IZV."""

import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import numpy as np
import seaborn as sns


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """Konvertovani dataframe do geopandas.GeoDataFrame se spravnym kodovani"""
    gpd = geopandas.GeoDataFrame(
        df,
        geometry=geopandas.points_from_xy(df["d"], df["e"]),
        crs="EPSG:5514"
    )
    return gpd[~gpd.is_empty]


def plot_bars(df: pd.DataFrame, fig_location: str = None,
              show_figure: bool = False):
    """ Vykresleni baru s nehodami zaviněnými technickou závadou """
    df.p2a = pd.to_datetime(df.p2a)

    total_accidents = df.groupby([df.p2a.dt.year]).size()
    print("rok;nehod")
    for i in range(2016, 2022):
        print(f"{i};{total_accidents.get(i)}")

    # p10 = 7 -> nehoda zaviněna technickou závadou
    print("\nrok;nehod_zavadou")
    df_filtered = df[(df.p10 == 7)]
    accidents = df_filtered.groupby([df_filtered.p2a.dt.year]).size()
    for i in range(2016, 2022):
        print(f"{i};{accidents.get(i)}")

    ax = sns.barplot(x=accidents.index, y=accidents.values)
    ax.set(xlabel="Rok", ylabel="Počet nehod")
    ax.bar_label(ax.containers[0])
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False, year: int = 2016):
    """ Vykresleni grafu s nehodami zaviněnými technickou závadou """
    gdf.p2a = pd.to_datetime(gdf.p2a)

    # p10 = 7 -> nehoda zaviněna technickou závadou
    data = gdf[
        (gdf.p10 == 7) &
        (gdf.p2a.dt.year == year)
    ]

    fig, ax = plt.subplots(1, 1, figsize=(12, 7))
    data.plot(ax=ax, markersize=2, color="red")
    ax.axis("off")
    ctx.add_basemap(ax, crs=gdf.crs.to_string(),
                    source=ctx.providers.CartoDB.Voyager, alpha=0.9)

    fig.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    df = pd.read_pickle("accidents.pkl.gz")
    gdf = make_geo(df)

    # Moc jsem toho tady nevymyslel...
    # Bar ploty
    plot_bars(df, "barplot.png")

    # Geo ploty
    for year in range(2016, 2022):
        plot_geo(gdf, "geo_" + str(year) + ".png", False, year)
