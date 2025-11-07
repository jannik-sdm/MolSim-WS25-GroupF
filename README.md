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
./MolSim <input-file>
```

Format the Code
```
make format
```
Get a detailed documentation with doxygen
```
make doc_doxygen
```
