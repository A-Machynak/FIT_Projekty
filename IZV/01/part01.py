#!/usr/bin/env python3
"""
IZV cast1 projektu
Autor: Augustin Machyňák

Detailni zadani projektu je v samostatnem projektu e-learningu.
Nezapomente na to, ze python soubory maji dane formatovani.

Muzete pouzit libovolnou vestavenou knihovnu a knihovny predstavene na
prednasce
"""

from bs4 import BeautifulSoup
import requests
import numpy as np
import matplotlib.pyplot as plt
from typing import List

import timeit
from timeit import Timer


def integrate(x: np.array, y: np.array) -> float:
    return np.sum((x[1:] - x[:-1]) * ((y[:-1] + y[1:]) / 2.0))


def generate_graph(a: List[float], show_figure: bool = False,
                   save_path: str | None = None):
    # f_values = np.multiply(np.array([a]).T, np.linspace(-3.0, 3.0)**2)
    x_values = np.linspace(-3.0, 3.0)
    y_values = np.array([a]).T * x_values**2

    fig = plt.figure(figsize=(8, 4))
    ax = fig.add_subplot()

    for i in range(0, len(a)):
        ax.plot(x_values, y_values[i], label="$y_{" + str(a[i]) + "}(x)$")
        ax.fill_between(x_values, y_values[i], alpha=0.1)
        ax.annotate(r"$\int f_{" + str(a[i]) + "}(x)dx$",
                    xy=(x_values[-1], y_values[i][-1] - 0.75))

    ax.set_xlabel("x")
    ax.set_ylabel("$f_a(x)$")
    ax.legend(loc="upper center", bbox_to_anchor=(0.5, 1.15), ncol=len(a))

    ax.set_xbound(x_values[0], x_values[-1] + 1.0)
    ax.set_xticks(np.arange(-3.0, 4.0))

    if save_path:
        fig.savefig(save_path)
    if show_figure:
        fig.show()
    plt.close(fig)


def generate_sinus(show_figure: bool = False, save_path: str | None = None):
    n_samples = 2000
    x_values = np.linspace(0.0, 100.0, n_samples)
    y_values = [
        0.5 * np.sin((1.0/50.0) * np.pi * x_values),
        0.25 * np.sin(np.pi * x_values)
    ]
    y_values.append(y_values[0] + y_values[1])

    y_labels = [
        "$f_1(x)$",
        "$f_2(x)$",
        "$f_1(x) + f_2(x)$"
    ]

    fig = plt.figure(figsize=(9, 10))
    axs = fig.subplots(3)

    # Nastavení pro všechny grafy
    for i in range(0, len(axs)):
        axs[i].set_ylim(-0.8, 0.8)
        axs[i].set_xlim(0, 100)
        axs[i].set_xlabel('t')
        axs[i].set_ylabel(y_labels[i])

        # np.linspace() pokud by jich bylo více
        axs[i].yaxis.set_ticks([-0.8, -0.4, 0.0, 0.4, 0.8])

    # Vykreslení prvních 2
    axs[0].plot(x_values, y_values[0])
    axs[1].plot(x_values, y_values[1])

    m = np.ma.masked_greater(y_values[2], y_values[0])
    axs[2].plot(x_values, y_values[2], color="#15b01a")
    # Překreslujeme předchozí graf - vytvoření a vykreslení 2 mask
    # by vytvořilo mezery mezi těmito grafy
    axs[2].plot(x_values, m, color="red")

    if save_path:
        fig.savefig(save_path)
    if show_figure:
        fig.show()
    plt.close(fig)


def download_data(url="https://ehw.fit.vutbr.cz/izv/temp.html"):
    output = []
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'html.parser')
    table_rows = soup.find("table").find_all("tr")

    for row in table_rows:
        row_cells = row.find_all("td")
        # První 2 rok a měsíc, ostatní temp
        temps = np.array(
            [*(float(cell.get_text().replace(',', '.'))
               for cell in row_cells[2:] if cell.find("p"))]
        )
        output.append({
            "year": int(row_cells[0].get_text()),
            "month": int(row_cells[1].get_text()),
            "temp": temps
        })
    return output


def get_avg_temp(data, year=None, month=None) -> float:
    # Filtrování podle roku a měsíce
    filtered = np.concatenate(
        [*(row['temp'] for row in data
           if (not year or row['year'] == year) and
           (not month or row['month'] == month))]
    )

    return np.sum(filtered) / len(filtered)
