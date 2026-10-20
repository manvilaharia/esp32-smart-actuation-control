#from zaber_motion.binary import Connection
#from zaber_motion import Units
import numpy as np
import matplotlib.pyplot as plt
import os
from pathlib import Path
file_path = Path(__file__).resolve().parents[1] / 'data' / 'data_exp_zaber_cont.npy'  # Add this dataset locally if available
data = np.load(file_path)
print(data)
start=00
con=data[start:,0]/4000
lvdt=data[start:,1]
force=data[start:,2]
print(len(lvdt))

#plt.plot(move)
# Create an array with row numbers
row_numbers = [(i + 1)*0.1 for i in range(len(force))]
plt.plot(row_numbers,con)  # Simple line plot, adjust as needed based on data structure
plt.plot(row_numbers,force)  # Simple line plot, adjust as needed based on data structure
plt.plot(row_numbers,lvdt)  # Simple line plot, adjust as needed based on data structure
# Create a plot
plt.plot(row_numbers, con, label='con', color='blue')  # Label for the first line
plt.plot(row_numbers, force, label='force', color='green')  # Label for the second line
plt.plot(row_numbers, lvdt, label='lvdt', color='black')  # Label for the second line
# Set x-axis label
plt.xlabel('Time(seconds)')
# Set y-axis label
plt.ylabel('Position Feedback')
plt.legend()
# Show the plot
plt.show()
