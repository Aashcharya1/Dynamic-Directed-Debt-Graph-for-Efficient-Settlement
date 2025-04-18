import pandas as pd
import matplotlib.pyplot as plt

# Read data from CSV
data = pd.read_csv('timings.csv')

# Add a small epsilon to avoid log(0) issues
epsilon = 0.1
data['AStar'] = data['AStar'] + epsilon
data['Dijkstra'] = data['Dijkstra'] + epsilon
data['BellmanFord'] = data['BellmanFord'] + epsilon
data['FloydWarshall'] = data['FloydWarshall'] + epsilon

# Plotting
plt.figure(figsize=(10, 6))

plt.plot(data['Nodes'], data['AStar'], marker='o', label='A* Algorithm')
plt.plot(data['Nodes'], data['Dijkstra'], marker='o', label='Dijkstra')
plt.plot(data['Nodes'], data['BellmanFord'], marker='o', label='Bellman-Ford')
plt.plot(data['Nodes'], data['FloydWarshall'], marker='o', label='Floyd-Warshall')

plt.yscale('log')

plt.title('Shortest Path Algorithm Timings')
plt.xlabel('Number of Nodes')
plt.ylabel('Execution Time (ms) [Log Scale]')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend()
plt.tight_layout()
plt.show()
