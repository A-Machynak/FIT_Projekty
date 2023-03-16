#!/usr/bin/python3.10
# coding=utf-8

"""Modul pro projekt do předmětu IZV."""

import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np
# muzete pridat vlastni knihovny


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """Konvertovani dataframe do GeoDataFrame."""
    gdf = geopandas.GeoDataFrame(
        df,
        geometry=geopandas.points_from_xy(df["d"], df["e"]),
        crs="EPSG:5514"
    )
    return gdf[~gdf.is_empty]


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """Vykresleni grafu s nehodami s alkoholem pro roky 2018-2021."""
    title = 'Nehody v Moravskoslezském kraji dle roku'
    selected_region = 'MSK'
    selected_years = [x for x in range(2018, 2022)]

    gdf.p2a = pd.to_datetime(gdf.p2a)

    data = [
        gdf[
            (gdf.p11 >= 3) &
            (gdf.p2a.dt.year == year) &
            (gdf.region == selected_region)
        ] for year in selected_years
    ]

    tmp = np.array([data[i].total_bounds for i in range(0, 4)])
    bounds = [
        min(tmp[:, 0]), min(tmp[:, 1]),
        max(tmp[:, 2]), max(tmp[:, 3])
    ]

    fig, axs = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle(title)
    for i, ax in enumerate(axs.reshape(-1)):
        data[i].plot(ax=ax, markersize=2, color="red")
        ax.axis("off")
        ax.set_xlim(bounds[0], bounds[2])
        ax.set_ylim(bounds[1], bounds[3])
        ax.set_title("Rok " + str(selected_years[i]))
        ctx.add_basemap(
            ax, crs=gdf.crs.to_string(),
            source=ctx.providers.Stamen.Toner, alpha=0.9
        )

    fig.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """Vykresleni grafu s nehodami shlukovanych do clusteru."""
    title = 'Nehody v Moravskoslezském kraji na silnicích 1., 2. a 3. třídy'
    selected_region = 'MSK'
    data = gdf[
        (gdf.region == selected_region) &
        ((gdf.p36 == 1) | (gdf.p36 == 2) | (gdf.p36 == 3))
    ].reset_index()

    fig, ax = plt.subplots(1, 1, figsize=(12, 12))
    fig.suptitle(title)

    X = np.column_stack((data.geometry.x, data.geometry.y))

    # Použit k-means - pro potřeby "rozdělení" dat do několika shluků
    # je dostatečný. Ještě by bylo možné použít GaussianMixture.
    result = sklearn.cluster.KMeans(n_clusters=20, n_init="auto")\
        .fit_predict(X)

    cdf = pd.DataFrame(result, columns=['cluster'])
    data = data.join(cdf)
    data['cluster_count'] = data\
        .groupby('cluster')['cluster']\
        .transform('count')

    data.plot(
        column='cluster_count', ax=ax, markersize=2,
        legend=True,
        cmap="winter_r",
        legend_kwds={
            'label': "Počet nehod v úseku",
            'orientation': "horizontal",
            'shrink': 0.925,
            'pad': 0.01
        }
    )

    ctx.add_basemap(
        ax, crs=gdf.crs.to_string(),
        source=ctx.providers.Stamen.Toner, alpha=0.9
    )
    ax.axis("off")

    fig.tight_layout()
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
