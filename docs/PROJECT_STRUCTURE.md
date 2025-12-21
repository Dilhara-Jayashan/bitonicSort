# 📁 Project Structure

This document provides an overview of the project organization and file structure.

## 🗂️ Root Directory

```
bitonicSort/
├── 📄 README.md              # Main project documentation
├── 📄 LICENSE                # MIT License
├── 📄 .gitignore            # Git ignore rules
├── 🔧 run_openmp.sh          # OpenMP benchmarking script
├── 🔧 run_mpi.sh             # MPI benchmarking script
├── 💾 serial_sort            # Compiled serial binary
├── 📚 docs/                  # Documentation files
├── 💻 Serial/                # Serial implementation
├── 🔀 OpenMP/                # OpenMP implementation
├── 🌐 MPI/                   # MPI implementation
├── 🎮 Cuda/                  # CUDA implementation
├── 📊 graph/                 # Performance visualization
├── 📥 InputFiles/            # Test datasets
├── 📤 OutputFiles/           # Results and timings
└── 📸 Screenshot/            # Execution screenshots
```

## 📚 Documentation (`docs/`)

All supplementary documentation is organized in the `docs` folder:

```
docs/
├── QUICKSTART.md           # 5-minute setup guide
├── RUN.md                  # Detailed build and run instructions
├── SCREENSHOTS.md          # Visual execution examples
├── PROJECT_SUMMARY.md      # Recruiter-friendly overview
├── CHANGELOG.md            # Version history
├── CONTRIBUTING.md         # Contribution guidelines
└── PROJECT_STRUCTURE.md    # This file
```

### Documentation Purpose

| File | Purpose | Audience |
|------|---------|----------|
| **QUICKSTART.md** | Fast setup for developers | Developers, Students |
| **RUN.md** | Comprehensive build guide | Technical users |
| **SCREENSHOTS.md** | Visual results showcase | Recruiters, General audience |
| **PROJECT_SUMMARY.md** | Portfolio highlight | Recruiters, HR |
| **CHANGELOG.md** | Development timeline | Developers, Maintainers |
| **CONTRIBUTING.md** | How to contribute | Contributors |

## 💻 Implementation Directories

### Serial (`Serial/`)
```
Serial/
└── bitonic_serial.c        # Sequential C implementation
```

### OpenMP (`OpenMP/`)
```
OpenMP/
├── bitonic_openmp.c        # OpenMP parallel implementation
└── bitonic_openmp          # Compiled binary
```

### MPI (`MPI/`)
```
MPI/
├── bitonic_mpi.c           # MPI distributed implementation
└── bitonic_mpi             # Compiled binary
```

### CUDA (`Cuda/`)
```
Cuda/
└── bitonic.ipynb - Colab.pdf  # GPU implementation (Colab notebook)
```

## 📊 Data and Results

### Input Files (`InputFiles/`)
```
InputFiles/
└── input.txt               # Test dataset (2048 elements)
```

Contains integer datasets for sorting. You can add custom datasets here.

### Output Files (`OutputFiles/`)
```
OutputFiles/
├── serial_output.txt       # Serial sorted results
├── openmp_output.txt       # OpenMP sorted results
├── openmp_times.txt        # OpenMP performance metrics
├── mpi_output.txt          # MPI sorted results
├── mpi_times.txt           # MPI performance metrics
└── cuda_output.txt         # CUDA sorted results
```

## 📊 Visualization (`graph/`)

```
graph/
├── plot_comparison.py           # Python visualization script
├── performance_comparison.png   # Overall comparison graph
├── detailed_analysis.png        # Detailed metrics graph
└── statistics_summary.png       # Statistical analysis graph
```

Python script and generated graphs for performance analysis.

## 📸 Screenshots (`Screenshot/`)

```
Screenshot/
├── serial.png              # Serial execution screenshot
├── openmp.png              # OpenMP execution screenshot
├── mpi.png                 # MPI execution screenshot
└── Cuda.png                # CUDA execution screenshot
```

Visual evidence of program execution and results.

## 🔧 Build and Run Scripts

### `run_openmp.sh`
- Compiles OpenMP implementation
- Runs with 1, 2, 4, 8, 16 threads
- Saves performance metrics
- Platform-aware (macOS/Linux)

### `run_mpi.sh`
- Compiles MPI implementation
- Runs with 1, 2, 4, 8, 16 processes
- Saves performance metrics
- Handles oversubscription

## 📦 File Types Overview

| Extension | Purpose | Count |
|-----------|---------|-------|
| `.c` | C source code | 3 |
| `.sh` | Shell scripts | 2 |
| `.md` | Markdown documentation | 7 |
| `.py` | Python scripts | 1 |
| `.txt` | Data/results files | 7+ |
| `.png` | Images/graphs | 7 |
| `.pdf` | Documentation | 2 |

## 🚀 Getting Started

1. **Read First**: [`README.md`](../README.md) - Main documentation
2. **Quick Setup**: [`docs/QUICKSTART.md`](QUICKSTART.md) - 5-minute guide
3. **Deep Dive**: [`docs/RUN.md`](RUN.md) - Detailed instructions
4. **See Results**: [`docs/SCREENSHOTS.md`](SCREENSHOTS.md) - Visual examples

## 📝 For Recruiters

- **Overview**: [`docs/PROJECT_SUMMARY.md`](PROJECT_SUMMARY.md)
- **Main Docs**: [`README.md`](../README.md)
- **Visual Proof**: [`docs/SCREENSHOTS.md`](SCREENSHOTS.md)

## 🤝 For Contributors

- **Guidelines**: [`docs/CONTRIBUTING.md`](CONTRIBUTING.md)
- **Build Guide**: [`docs/RUN.md`](RUN.md)
- **Changes**: [`docs/CHANGELOG.md`](CHANGELOG.md)

## 🎯 Navigation Tips

### From Root Directory
```bash
# View main documentation
cat README.md

# Run OpenMP version
bash run_openmp.sh InputFiles/input.txt

# View quick start guide
cat docs/QUICKSTART.md
```

### From Documentation
All documentation files in `docs/` link back to the main README and to each other for easy navigation.

## 📐 Design Principles

This structure follows these principles:

1. **Clarity** - Clear separation of code, docs, data, and results
2. **Simplicity** - Logical grouping with intuitive names
3. **Accessibility** - Easy navigation for all audiences
4. **Maintainability** - Organized structure for updates
5. **Professionalism** - Industry-standard layout

---

<div align="center">

**[← Back to Main README](../README.md)**

</div>
