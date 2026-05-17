import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('../output/data.csv')

plt.figure(figsize=(8, 5))
serial_df = df[df['mode'] == 'serial']

for alg in serial_df['algorithm'].unique():
    alg_data = serial_df[serial_df['algorithm'] == alg].sort_values(by='n')
    plt.plot(alg_data['n'], alg_data['time_seconds'], marker='o', label=alg.capitalize())

plt.xscale('log')
plt.yscale('log')
plt.title('Serial Execution: Size vs Time')
plt.xlabel('Size (n)')
plt.ylabel('Time (Seconds)')
plt.legend()
plt.grid(True, which="both", ls="--", alpha=0.5)
plt.tight_layout()
plt.savefig('../output/execution.png')
plt.close()

plt.figure(figsize=(8, 5))
parallel_df = df[df['mode'] == 'parallel']

for alg in parallel_df['algorithm'].unique():
    p_data = parallel_df[parallel_df['algorithm'] == alg].sort_values(by='processes')
    
    n_val = p_data['n'].iloc[0] 
    
    s_data = serial_df[(serial_df['algorithm'] == alg) & (serial_df['n'] == n_val)]
    
    if not s_data.empty:
        serial_time = s_data['time_seconds'].values[0]
        speedup = serial_time / p_data['time_seconds']
        
        plt.plot(p_data['processes'], speedup, marker='s', label=f"{alg.capitalize()}")

max_proc = parallel_df['processes'].max()
plt.plot([1, max_proc], [1, max_proc], 'k--', alpha=0.7, label='Ideal Speedup')

plt.title('Speedup of Parallel Algorithms')
plt.xlabel('Number of Processes')
plt.ylabel('Speedup')
plt.legend()
plt.grid(True, ls="--", alpha=0.5)
plt.tight_layout()
plt.savefig('../output/speedup.png')
plt.close()