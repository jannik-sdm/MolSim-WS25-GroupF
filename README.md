MolSim-GroupF
===

The Molecular Dynamics teaching code.

## Build
```
cmake -B build -S .
cd build
make -j <number-of-threads-used-for-compilation>
```
## Run
Run the program
```
./MolSim -f <input-file> [Options]
```

### Options

| Short | Long                | Argument | Description                                                             |
|:------|:--------------------|:---------|:------------------------------------------------------------------------|
| `-w`  | `--week`            | *UINT*   | Select which week's simulation to run (default: 2).                     |
| `-f`  | `--file`            | *FILE*   | Reads particles from the specified file in xyz format.                  |
| `-c`  | `--cuboid`          | *FILE*   | Reads particles from the specified file in cuboid format.               |
| `-o`  | `--out`             | *FILE*   | path and name of the output files. Path has to exist! (default: MD_vtk) |
| `-e`  | `--t_end`           | *DOUBLE* | Sets `t_end` (default: **1000**).                                       |
| `-d`  | `--delta_t`         | *DOUBLE* | Sets `delta_t` (default: **0.014**).                                    |
| `-b`  | `--BrownMotionMean` | *DOUBLE* | Sets the mean for the Brownian motion.                                  |
| `-h`  | `--help`            | —        | Shows this text and terminates the program.                             |

Format the Code
```
make format
```
Get a detailed documentation with doxygen
```
make doc_doxygen
```
