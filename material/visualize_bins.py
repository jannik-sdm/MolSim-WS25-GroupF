#! /usr/bin/env python

import pandas as pd
import numpy as np
import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.interpolate import griddata


df: pd.DataFrame = pd.DataFrame()

# Concatenate files
for i, file in enumerate(sys.argv[1:]):
	data = pd.read_csv(file, names=["count", "average"], header=None)
	data["time"] = i
	data["bin"] = data.index
	df = pd.concat([df, data], ignore_index=True)


# Plot
def plot(column: str, label: str, filename: str):
	fig = plt.figure(figsize=(15, 10))
	ax = fig.add_subplot(111, projection='3d')

	# Plot the data
	for _, group in df.groupby("bin"):
		ax.plot(group["time"], group["bin"], group[column])

	# Add labels
	ax.set_xlabel("Iteration (10'000)")
	ax.set_ylabel("Bin")
	ax.set_zlabel(label)

	plt.subplots_adjust(left=0, right=2, bottom=0, top=2)

	# Save as SVG
	fig.savefig(filename, format="svg", dpi=300, bbox_inches="tight")

plot("count", "Particle count", "Particle_Count.svg")
plot("average", "Velocity Average (Y)", "Velocity_Profile.svg")
