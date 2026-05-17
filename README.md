# High Performance Computing

Exploring parallel programming with MPI. Mostly benchmarking how much faster things get when you throw more cores at a problem.

## Structure

```
openmpi/
├── intro-to-openmpi/     # MPI basics: hello world, scatter/gather, broadcast, reduce, pi
└── sorting-algorithms/
    ├── src/              # C sources + Python visualizer
    └── output/           # benchmark data (CSV) and plots (PNG)
```

## Sorting Algorithms

Five sorting algorithms benchmarked in both serial and parallel (MPI) modes on an Intel Core i5-12400F:
bubble, selection, quicksort, merge sort, and bitonic sort.

The parallel implementation uses **dynamic scattering** to distribute work across processes and a **tree reduction merge** (O(log P)) to collect results. So communication overhead scales gently with process count.

### Build

From `src/`:
```bash
# serial
gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DQUICK

# parallel
mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DQUICK
```

Replace `-DQUICK` with `-DBUBBLE`, `-DSELECTION`, `-DMERGE`, or `-DBITONIC`.

### Run

```bash
./serial 134217728
mpiexec -np 8 ./parallel 134217728
```

### Visualize

```bash
cd src && python3 visualize.py
```

Reads `../output/data.csv`, writes `execution.png` and `speedup.png` to `../output/`.

## Highlights

- Quicksort scales the best in parallel. 12 processes cuts serial time from 12s to ~3.8s
- Bubble sort is painful at any scale (1517s serial on 1M elements)