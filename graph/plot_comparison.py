#!/usr/bin/env python3
"""
Bitonic Sort Performance Comparison
Generates graphs comparing MPI, OpenMP, and CUDA implementations
"""

import matplotlib.pyplot as plt
import numpy as np

# ============================================================================
# Data from execution results
# ============================================================================

# OpenMP data (threads vs execution time in seconds)
openmp_threads = [1, 2, 4, 8, 16]
openmp_times = [0.000649, 0.000764, 0.000804, 0.001623, 0.002517]

# MPI data (processes vs execution time in seconds)
mpi_processes = [1, 2, 4, 8, 16]
mpi_times = [0.000340, 0.000112, 0.000066, 0.000065, 0.000095]

# CUDA data (threads per block vs execution time in seconds)
cuda_threads_per_block = [128, 256, 512, 1024]
cuda_times = [0.000747, 0.000774, 0.000901, 0.000856]

dataset_size = 2048

# ============================================================================
# Figure 1: Execution Time Comparison (Line Plot)
# ============================================================================

plt.figure(figsize=(14, 10))

# Subplot 1: All three on same plot
plt.subplot(2, 2, 1)
plt.plot(openmp_threads, openmp_times, 'o-', linewidth=2, markersize=8, 
         label='OpenMP', color='#2E86AB')
plt.plot(mpi_processes, mpi_times, 's-', linewidth=2, markersize=8, 
         label='MPI', color='#A23B72')
plt.xlabel('Number of Threads/Processes', fontsize=11, fontweight='bold')
plt.ylabel('Execution Time (seconds)', fontsize=11, fontweight='bold')
plt.title('OpenMP vs MPI Performance Comparison\n(Dataset: 2048 elements)', 
          fontsize=12, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.yscale('log')

# Subplot 2: CUDA performance
plt.subplot(2, 2, 2)
plt.plot(cuda_threads_per_block, cuda_times, 'd-', linewidth=2, markersize=8, 
         label='CUDA', color='#F18F01')
plt.xlabel('Threads per Block', fontsize=11, fontweight='bold')
plt.ylabel('Execution Time (seconds)', fontsize=11, fontweight='bold')
plt.title('CUDA Performance vs Thread Configuration\n(Dataset: 2048 elements)', 
          fontsize=12, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)

# Subplot 3: Speedup comparison (vs baseline)
plt.subplot(2, 2, 3)
openmp_speedup = [openmp_times[0] / t for t in openmp_times]
mpi_speedup = [mpi_times[0] / t for t in mpi_times]
plt.plot(openmp_threads, openmp_speedup, 'o-', linewidth=2, markersize=8, 
         label='OpenMP', color='#2E86AB')
plt.plot(mpi_processes, mpi_speedup, 's-', linewidth=2, markersize=8, 
         label='MPI', color='#A23B72')
plt.plot([1, 16], [1, 1], 'k--', alpha=0.5, label='No Speedup')
plt.xlabel('Number of Threads/Processes', fontsize=11, fontweight='bold')
plt.ylabel('Speedup (relative to single thread/process)', fontsize=11, fontweight='bold')
plt.title('Speedup Analysis', fontsize=12, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)

# Subplot 4: Best performance comparison (Bar chart)
plt.subplot(2, 2, 4)
best_openmp = min(openmp_times)
best_mpi = min(mpi_times)
best_cuda = min(cuda_times)

methods = ['OpenMP\n(1 thread)', 'MPI\n(8 processes)', 'CUDA\n(128 threads/block)']
best_times = [best_openmp, best_mpi, best_cuda]
colors = ['#2E86AB', '#A23B72', '#F18F01']

bars = plt.bar(methods, best_times, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
plt.ylabel('Execution Time (seconds)', fontsize=11, fontweight='bold')
plt.title('Best Performance Comparison', fontsize=12, fontweight='bold')
plt.grid(True, axis='y', alpha=0.3)

# Add value labels on bars
for bar, time in zip(bars, best_times):
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height,
             f'{time:.6f}s',
             ha='center', va='bottom', fontsize=10, fontweight='bold')

plt.tight_layout()
plt.savefig('OutputFiles/performance_comparison.png', dpi=300, bbox_inches='tight')
print("✓ Saved: OutputFiles/performance_comparison.png")

# ============================================================================
# Figure 2: Detailed Analysis
# ============================================================================

plt.figure(figsize=(15, 5))

# Subplot 1: Execution time with values
plt.subplot(1, 3, 1)
x_pos = np.arange(len(openmp_threads))
width = 0.35

bars1 = plt.bar(x_pos - width/2, openmp_times, width, label='OpenMP', 
                color='#2E86AB', alpha=0.7, edgecolor='black')
bars2 = plt.bar(x_pos + width/2, mpi_times, width, label='MPI', 
                color='#A23B72', alpha=0.7, edgecolor='black')

plt.xlabel('Configuration', fontsize=11, fontweight='bold')
plt.ylabel('Execution Time (seconds)', fontsize=11, fontweight='bold')
plt.title('OpenMP vs MPI: Detailed Time Comparison', fontsize=12, fontweight='bold')
plt.xticks(x_pos, [f'{t}' for t in openmp_threads])
plt.legend(fontsize=10)
plt.grid(True, axis='y', alpha=0.3)

# Add value labels
for bars in [bars1, bars2]:
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                 f'{height:.4f}',
                 ha='center', va='bottom', fontsize=8)

# Subplot 2: Efficiency Analysis
plt.subplot(1, 3, 2)
openmp_efficiency = [(openmp_times[0] / (t * openmp_times[i])) * 100 
                     for i, t in enumerate(openmp_threads)]
mpi_efficiency = [(mpi_times[0] / (t * mpi_times[i])) * 100 
                  for i, t in enumerate(mpi_processes)]

plt.plot(openmp_threads, openmp_efficiency, 'o-', linewidth=2, markersize=8, 
         label='OpenMP', color='#2E86AB')
plt.plot(mpi_processes, mpi_efficiency, 's-', linewidth=2, markersize=8, 
         label='MPI', color='#A23B72')
plt.axhline(y=100, color='k', linestyle='--', alpha=0.5, label='Ideal (100%)')
plt.xlabel('Number of Threads/Processes', fontsize=11, fontweight='bold')
plt.ylabel('Parallel Efficiency (%)', fontsize=11, fontweight='bold')
plt.title('Parallel Efficiency Analysis', fontsize=12, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)

# Subplot 3: CUDA detailed analysis
plt.subplot(1, 3, 3)
cuda_blocks = [16, 8, 4, 2]
x_labels = [f'{t}t\n{b}b' for t, b in zip(cuda_threads_per_block, cuda_blocks)]
bars = plt.bar(range(len(cuda_times)), cuda_times, color='#F18F01', 
               alpha=0.7, edgecolor='black', linewidth=1.5)
plt.xlabel('Threads per Block (t) / Blocks (b)', fontsize=11, fontweight='bold')
plt.ylabel('Execution Time (seconds)', fontsize=11, fontweight='bold')
plt.title('CUDA: Thread Configuration Impact', fontsize=12, fontweight='bold')
plt.xticks(range(len(cuda_times)), x_labels)
plt.grid(True, axis='y', alpha=0.3)

# Add value labels
for bar, time in zip(bars, cuda_times):
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height,
             f'{time:.6f}s',
             ha='center', va='bottom', fontsize=9, fontweight='bold')

plt.tight_layout()
plt.savefig('OutputFiles/detailed_analysis.png', dpi=300, bbox_inches='tight')
print("✓ Saved: OutputFiles/detailed_analysis.png")

# ============================================================================
# Figure 3: Summary Statistics Table
# ============================================================================

fig, ax = plt.subplots(figsize=(12, 6))
ax.axis('tight')
ax.axis('off')

# Calculate statistics
stats_data = [
    ['Method', 'Best Time (s)', 'Worst Time (s)', 'Avg Time (s)', 'Best Config', 'Speedup (vs worst)'],
    ['OpenMP', 
     f'{min(openmp_times):.6f}', 
     f'{max(openmp_times):.6f}', 
     f'{np.mean(openmp_times):.6f}',
     f'{openmp_threads[openmp_times.index(min(openmp_times))]} threads',
     f'{max(openmp_times)/min(openmp_times):.2f}x'],
    ['MPI', 
     f'{min(mpi_times):.6f}', 
     f'{max(mpi_times):.6f}', 
     f'{np.mean(mpi_times):.6f}',
     f'{mpi_processes[mpi_times.index(min(mpi_times))]} processes',
     f'{max(mpi_times)/min(mpi_times):.2f}x'],
    ['CUDA', 
     f'{min(cuda_times):.6f}', 
     f'{max(cuda_times):.6f}', 
     f'{np.mean(cuda_times):.6f}',
     f'{cuda_threads_per_block[cuda_times.index(min(cuda_times))]} threads/block',
     f'{max(cuda_times)/min(cuda_times):.2f}x'],
]

table = ax.table(cellText=stats_data, cellLoc='center', loc='center',
                colWidths=[0.15, 0.15, 0.15, 0.15, 0.2, 0.2])
table.auto_set_font_size(False)
table.set_fontsize(10)
table.scale(1, 2.5)

# Style header row
for i in range(6):
    cell = table[(0, i)]
    cell.set_facecolor('#34495e')
    cell.set_text_props(weight='bold', color='white')

# Style data rows
colors = ['#2E86AB', '#A23B72', '#F18F01']
for i in range(1, 4):
    for j in range(6):
        cell = table[(i, j)]
        cell.set_facecolor(colors[i-1])
        cell.set_alpha(0.3)
        cell.set_text_props(weight='bold')

plt.title('Performance Statistics Summary\nDataset: 2048 elements', 
          fontsize=14, fontweight='bold', pad=20)
plt.savefig('OutputFiles/statistics_summary.png', dpi=300, bbox_inches='tight')
print("✓ Saved: OutputFiles/statistics_summary.png")

# ============================================================================
# Print Summary to Console
# ============================================================================

print("\n" + "="*70)
print("PERFORMANCE SUMMARY")
print("="*70)
print(f"Dataset Size: {dataset_size} elements\n")

print("OpenMP Results:")
print(f"  Best:  {min(openmp_times):.6f}s ({openmp_threads[openmp_times.index(min(openmp_times))]} threads)")
print(f"  Worst: {max(openmp_times):.6f}s ({openmp_threads[openmp_times.index(max(openmp_times))]} threads)")

print("\nMPI Results:")
print(f"  Best:  {min(mpi_times):.6f}s ({mpi_processes[mpi_times.index(min(mpi_times))]} processes)")
print(f"  Worst: {max(mpi_times):.6f}s ({mpi_processes[mpi_times.index(max(mpi_times))]} processes)")

print("\nCUDA Results:")
print(f"  Best:  {min(cuda_times):.6f}s ({cuda_threads_per_block[cuda_times.index(min(cuda_times))]} threads/block)")
print(f"  Worst: {max(cuda_times):.6f}s ({cuda_threads_per_block[cuda_times.index(max(cuda_times))]} threads/block)")

print("\nOverall Winner:")
all_times = {'OpenMP': min(openmp_times), 'MPI': min(mpi_times), 'CUDA': min(cuda_times)}
winner = min(all_times, key=all_times.get)
print(f"  {winner}: {all_times[winner]:.6f}s")
print(f"  {min(mpi_times)/min(openmp_times):.2f}x faster than best OpenMP")
print(f"  {min(cuda_times)/min(mpi_times):.2f}x slower than best MPI")

print("\nKey Observations:")
print("  • MPI shows best scalability with multiple processes")
print("  • OpenMP performance degrades with more threads (overhead dominates)")
print("  • CUDA shows consistent performance across configurations")
print("  • Small dataset size (2048) limits parallel efficiency")
print("="*70)
print("\n✓ All graphs generated successfully!")
print("  View them in the OutputFiles/ directory")

# plt.show()  # Comment out to avoid blocking terminal
