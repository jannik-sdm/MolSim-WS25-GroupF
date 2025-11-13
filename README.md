MolSim-GroupF
===

The Molecular Dynamics teaching code.

## Build
```
cmake -B build -S . [-D<Build-Option-Name>=<value>]
cd build
make -j <number-of-threads-used-for-compilation>
//or
mkdir build
cd build
ccmake .. //configure build Options
make -j <number-of-threads-used-for-compilation>
```
## Build Options
| Name                  | Argument  Type | Default | Description                                                                                                                                                                                                                                                                                       |
|:----------------------|:---------------|:--------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ENABLE_DOXYGEN_TARGET | *BOOLEAN*      | ON      | make doc_doxygen will only work if set to ON. If you don't have Doxygen on your machine disable this target                                                                                                                                                                                       |
| ENABLE_TEST_TARGET    | *BOOLEAN*      | ON      | ctest will only work if this target is set to ON. You can disable it to make compiling faster, if you don't want to test the code                                                                                                                                                                 |
| ENABLE_TIME_MEASURE   | *BOOLEAN*      | OFF     | If this target is set to ON, the program will measure and print the time it took to execute everything. To make this time more statistically significant, the program can be executed mor then one by changing the parameter iterations.                                                          |
| ITERATIONS            | *INTEGER*      | 1       | This Target will only be visible, if ENABLE_TIME_MEASURE is set to ON. If you want to execute this program several times, to get better results in measuring the time, you can modify this value.                                                                                                 |
| ENABLE_VTK_OUTPUT     | *BOOLEAN*      | OFF     | The program can either generate a .xyz-file or an .vtu-file. Generating the .xyz-file is faster and the output is human readable while .vtu-files can be used do generate a animation with ParaView. .vtu-files will only be generated, if this target is set to ON and if you have installed vtk |

## Run
Run the program
```
./MolSim -f <input-file> [Options]
```

### Options

| Short | Long                | Argument | Description                                                                                   |
|:------|:--------------------|:---------|:----------------------------------------------------------------------------------------------|
| `-w`  | `--week`            | *UINT*   | select which week's simulation to run (1=PlanetSimulation, 2=Collision Simulation (Default))  |
| `-s`  | `--single`          | *FILE*   | Reads single particles from the specified file in xyz format.                                 |
| `-c`  | `--cuboid`          | *FILE*   | Reads particles from the specified file in cuboid format.                                     |
| `-o`  | `--out`             | *FILE*   | path and name of the output files. Path has to exist! (default: MD_vtk)                       |
| `-e`  | `--t_end`           | *DOUBLE* | Sets `t_end` (default: **1000**).                                                             |
| `-d`  | `--delta_t`         | *DOUBLE* | Sets `delta_t` (default: **0.014**).                                                          |
| `-b`  | `--BrownMotionMean` | *DOUBLE* | Sets the mean for the Brownian motion.                                                        |
| `-h`  | `--help`            | —        | Shows this text and terminates the program.                                                   |

Format the Code
```
make format
```
Get a detailed documentation with doxygen
```
make doc_doxygen
```
Test the Code
```
ctest
```
