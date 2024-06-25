#!/usr/bin/env python3

# python3 -mvenv pyenv
# . ./pyenv/bin/activate
# pip3 install seaborn pandas matplotlib
# ./plot_compares.py

import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

sns.set_theme()

data = pd.read_csv("compares.csv")
sns.lineplot(data, x="num_items", y="num_compares")
plt.loglog(base=2)
plt.xlabel("Anzahl der Elemente")
plt.ylabel("Durchschnittliche Anzahl der Vergleiche")

plt.savefig("compares.pdf")
