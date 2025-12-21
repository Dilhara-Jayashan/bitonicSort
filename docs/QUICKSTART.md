# 🚀 Quick Start Guide

**Get up and running with Parallel Bitonic Sort in 5 minutes!**

---

## ⚡ Super Quick Start (macOS)

```bash
# 1. Install dependencies (one-time setup)
brew install libomp open-mpi

# 2. Clone the project
git clone https://github.com/yourusername/bitonic-sort.git
cd bitonic-sort

# 3. Run OpenMP version
bash run_openmp.sh InputFiles/input.txt

# 4. Run MPI version
bash run_mpi.sh InputFiles/input.txt

# Done! Check OutputFiles/ for results
```

---

## 🐧 Super Quick Start (Linux - Ubuntu/Debian)

```bash
# 1. Install dependencies (one-time setup)
sudo apt-get update
sudo apt-get install libomp-dev libopenmpi-dev openmpi-bin gcc

# 2. Clone the project
git clone https://github.com/yourusername/bitonic-sort.git
cd bitonic-sort

# 3. Run OpenMP version
bash run_openmp.sh InputFiles/input.txt

# 4. Run MPI version
bash run_mpi.sh InputFiles/input.txt

# Done! Check OutputFiles/ for results
```

---

## 📊 View Results

After running the scripts, check these files:

```bash
# Sorted output
cat OutputFiles/openmp_output.txt
cat OutputFiles/mpi_output.txt

# Performance metrics
cat OutputFiles/openmp_times.txt
cat OutputFiles/mpi_times.txt
```

---

## 🎨 Generate Performance Graphs

```bash
# Install Python dependencies (one-time)
pip install matplotlib numpy

# Generate graphs
cd graph
python plot_comparison.py

# View generated images
open performance_comparison.png    # macOS
xdg-open performance_comparison.png  # Linux
```

---

## 🔧 Manual Compilation (Optional)

If you want to compile and run manually:

### Serial Version
```bash
gcc -O2 -std=c11 Serial/bitonic_serial.c -o serial_sort
./serial_sort InputFiles/input.txt
```

### OpenMP Version
```bash
# macOS
clang -O2 -std=c11 -Xpreprocessor -fopenmp \
  -I/opt/homebrew/opt/libomp/include \
  -L/opt/homebrew/opt/libomp/lib -lomp \
  OpenMP/bitonic_openmp.c -o OpenMP/bitonic_openmp

# Linux
gcc -O2 -std=c11 -fopenmp OpenMP/bitonic_openmp.c -o OpenMP/bitonic_openmp

# Run with 4 threads
export OMP_NUM_THREADS=4
./OpenMP/bitonic_openmp InputFiles/input.txt
```

### MPI Version
```bash
# Compile
mpicc -O2 -std=c11 MPI/bitonic_mpi.c -o MPI/bitonic_mpi

# Run with 4 processes
mpirun -np 4 MPI/bitonic_mpi InputFiles/input.txt

# If you get "not enough slots" error
mpirun --oversubscribe -np 4 MPI/bitonic_mpi InputFiles/input.txt
```

---

## 📝 Create Custom Input

```bash
# Create a custom dataset
echo "42 17 93 8 55 23 71 34" > InputFiles/my_data.txt

# Run with your data
bash run_openmp.sh InputFiles/my_data.txt
```

---

## ❓ Common Issues & Quick Fixes

### Issue: "command not found: gcc" or "clang"

**macOS:**
```bash
xcode-select --install
```

**Linux:**
```bash
sudo apt-get install build-essential
```

### Issue: "library not found for -lomp"

**macOS:**
```bash
brew install libomp
```

**Linux:**
```bash
sudo apt-get install libomp-dev
```

### Issue: "mpicc: command not found"

**macOS:**
```bash
brew install open-mpi
```

**Linux:**
```bash
sudo apt-get install openmpi-bin libopenmpi-dev
```

### Issue: "Permission denied" when running scripts

```bash
chmod +x run_openmp.sh run_mpi.sh
```

### Issue: Slow performance

- For OpenMP: Reduce thread count
  ```bash
  export OMP_NUM_THREADS=4
  ```
  
- For MPI: Use fewer processes than CPU cores
  ```bash
  mpirun -np 4 MPI/bitonic_mpi InputFiles/input.txt
  ```

---

## 📚 What's Next?

1. **Understand the Code**: Read through the C source files in `Serial/`, `OpenMP/`, and `MPI/`
2. **Explore Documentation**: Check out [README.md](README.md) for detailed information
3. **View Results**: Look at [SCREENSHOTS.md](SCREENSHOTS.md) for visual execution examples
4. **Analyze Performance**: Study the generated graphs to understand speedup patterns
5. **Experiment**: Try different dataset sizes and thread/process counts

---

## 🎯 Project Structure Overview

```
bitonicSort/
├── Serial/           → Sequential implementation
├── OpenMP/           → Multi-threaded (shared memory)
├── MPI/              → Multi-process (distributed memory)
├── Cuda/             → GPU implementation (Colab notebook)
├── InputFiles/       → Test datasets
├── OutputFiles/      → Results and timings
├── graph/            → Visualization scripts and images
└── run_*.sh          → Automated benchmark scripts
```

---

## 💡 Quick Performance Comparison

| Implementation | Best For | Typical Speedup |
|----------------|----------|-----------------|
| **Serial** | Baseline | 1.0x |
| **OpenMP** | Multi-core CPUs | 0.8x - 1.5x |
| **MPI** | Distributed systems | 5x - 10x ⭐ |
| **CUDA** | GPU processing | 0.7x - 2x |

*Note: Speedup depends on dataset size and hardware*

---

## 🆘 Need Help?

- 📖 [Full Documentation](../README.md)
- 🏃 [Detailed Run Guide](RUN.md)
- 🐛 [Report Issues](https://github.com/yourusername/bitonic-sort/issues)
- 💬 [Ask Questions](https://github.com/yourusername/bitonic-sort/discussions)

---

<div align="center">

**Happy Sorting! 🎉**

[Back to Main README](../README.md)

</div>
