# MolSim-GroupF

The Molecular Dynamics teaching code.

## Getting started
Build the program:
```sh
$ cmake -B build/
$ cmake --build build/
```

Run this worksheet’s simulation:
```sh
$ build/Molsim -w 3 -y input/assignment3.yaml -o out/
```

## Usage
There are two possible ways to configure the simulation settings:

### Over the command line
This passes all necessary parameters over the command line.
The available options are:

| Option                     | Argument | Description                                                                                   |
|:---------------------------|:---------|:----------------------------------------------------------------------------------------------|
| `-o`, `--out`              | `str`    | Path and name of the output files
| `-f`, `--frequency`        | `uint`   | Frequency at which output files are written
| `-w`, `--worksheet`        | `uint`   | Select which worksheet’s simulation to run
| `-e`, `--end-time`         | `double` | Sets the end time
| `-d`, `--delta-t`          | `double` | Sets the timestep
| `-b`, `--brown-motion-avg` | `double` | Sets the average mean for the Brownian motion.
| `-s`, `--single`           | `str`    | Reads particles from the specified file in xyz format
| `-c`, `--cuboid`           | `str`    | Reads particles from the specified file in cuboid format
| `-y`, `--yaml`             | `str`    | Reads particles and settings from the specified file in yaml format
| `-l`, `--loglevel`         | `str`    | Set the log level (trace, debug, info, warn, error)
| `-h`, `--help`             |          | Show a help text and terminates the program.

> [!IMPORTANT]
> When using yaml files with command line parameters, the order of options matters.
> Parameters passed before the YAML file can be overwritten by parameters in the file.

### With YAML Files
YAML files contain all informations for a given scenario and can be read like this:
```
./MolSim -y <input-file> [Options]
```

In general, YAML files allow for more simulation parameters that can not be specified over the command line.
It is recommended to use YAML files.

The structure of a YAML file is following:
```yaml
output:
  folder: "out/" # Where to store xyz/vtk files
  frequency: 10 # After how many iterations output is plotted

# Parameters for running the simulation
simulation:
  worksheet: 3 # Which worksheet's simulation to use
  delta_t: 0.0005 # Timestep to use in the simulation
  end_time: 20 # Time when the simulation ends
  brown_motion_avg_velocity: 0.1 # Average velocity to use in brownian motion
  cutoff_radius: 3.0 # Cutoff radius for LinkedCells simulations
  domain: [180, 90, 1] # Domain for LinkedCells simulations
  borders: [outflow,outflow,outflow,outflow,outflow,outflow] #Border Types for the 6 Borders of the Domain. Possible Border Types are: outflow, period, naive Reflection, reflection
  dimension: 2D #2D or 3D Domain

# Instructions to spawn particles
particles:
  # Spawn a single particle
  - single: 
      position: [0.0, 0.0, 0.0]
      velocity: [0.0, 0.0, 0.0]
      mass: 1
  # Spawn a cuboid
  - cuboid:
      position: [20, 20, 0] # Lower left corner
      size: [100, 20, 1] # Number of particles along each axis
      distance: 1.1225 # Distance between particles
      mass: 1 # Mass for each particle
      velocity: [0, 0, 0] # Initial velocity for each particle
  # Spawn a 2D-disc on the (x, y) plane
  - disc: 
      position: [70, 60, 0] # Center of the disc
      radius: 15 # Number of particles in each direction
      distance: 1.1225 # Distance between particles
      mass: 1 # Mass for each particle
      velocity: [0, -10, 0] # Initial velocity for each particle

```

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
| Name                    | Argument  Type | Default | Description                                                                                                                                                                                                                                                                                       |
|:------------------------|:---------------|:--------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `ENABLE_DOXYGEN_TARGET` | *BOOLEAN*      | ON      | make doc_doxygen will only work if set to ON. If you don't have Doxygen on your machine disable this target                                                                                                                                                                                       |
| `ENABLE_TEST_TARGET`    | *BOOLEAN*      | ON      | ctest will only work if this target is set to ON. You can disable it to make compiling faster, if you don't want to test the code                                                                                                                                                                 |
| `ENABLE_TIME_MEASURE`   | *BOOLEAN*      | OFF     | If this target is set to ON, the program will measure and print the time it took to execute everything. To make this time more statistically significant, the program can be executed mor then one by changing the parameter iterations.                                                          |
| `ITERATIONS`            | *INTEGER*      | 1       | This Target will only be visible, if ENABLE_TIME_MEASURE is set to ON. If you want to execute this program several times, to get better results in measuring the time, you can modify this value.                                                                                                 |
| `ENABLE_VTK_OUTPUT`     | *BOOLEAN*      | OFF     | The program can either generate a .xyz-file or an .vtu-file. Generating the .xyz-file is faster and the output is human readable while .vtu-files can be used do generate a animation with ParaView. .vtu-files will only be generated, if this target is set to ON and if you have installed vtk |


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

![Implementation worksheet 2 vs. worksheet 3](./material/Graphik_Iterationen.png)