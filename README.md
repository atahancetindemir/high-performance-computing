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

Two parallel approaches are included. `parallel.c` does dynamic scatter + tree reduction merge simple, but rank 0 ends up doing a big O(n) sequential merge at the end. `psrs.c` fixes that with **Parallel Regular Sampling Sort**: processes negotiate splitters, redistribute data all-to-all, and each one independently sorts its own slice. No bottleneck.

### Build

From `src/`:
```bash
# serial
gcc -O3 -march=native -Wall -Wextra sort.c serial.c -o serial -DQUICK

# parallel tree reduction
mpicc -O3 -march=native -Wall -Wextra sort.c parallel.c -o parallel -DQUICK

# parallel PSRS
mpicc -O3 -march=native -Wall -Wextra sort.c psrs.c -o psrs
```

Replace `-DQUICK` with `-DBUBBLE`, `-DSELECTION`, `-DMERGE`, or `-DBITONIC`.

### Run

```bash
./serial 134217728
mpiexec -np 8 ./parallel 134217728
mpiexec --use-hwthread-cpus -np 12 ./psrs 134217728
```

### Visualize

```bash
cd src && python3 visualize.py
```

Reads `../output/data.csv`, writes `execution.png`, `speedup.png`, and `psrs_comparison.png` to `../output/`.

## Highlights

- Quicksort scales the best of the tree-reduction group 12 cores cuts 12s down to ~3.8s
- PSRS does better: same 12 cores gets to ~1.8s (6.8x speedup vs 3.2x for tree reduction)
- Bubble sort is painful at any scale (1517s serial on 1M elements)