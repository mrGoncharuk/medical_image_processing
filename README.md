# medical_image_processing
Labs from Medical Image Processing course

## Cloning project(with submodules)
```git clone --recursive https://github.com/mrGoncharuk/medical_image_processing.git```

If you cloned project in common way use
```
git submodule init
git submodule update
```
to clone submodules.

## Building imebra (for more information visit: https://bitbucket.org/binarno/imebra_git/src/master/)
```
cd imebra_git
mkdir artifacts
cd artifacts
cmake ../
cmake --build .
```

## Building project
```
cd medical_image_processing/
make
```

## Running binary
```
./MIP
```
If you have an error like:
```./MIP: error while loading shared libraries: libimebra.so.5: cannot open shared object file: No such file or directory```
go to repo folder and run this command
```
cd medical_image_processing/
export LD_LIBRARY_PATH=./imebra_git/artifacts/
```

It is possible that after reopening terminal window this error will return.
It is because library installed locally in your system and after you close your terminal
window the environment variable ```LD_LIBRARY_PATH``` is deleted.

If you want ability to run binary from any folder but you get ```error while loading shared libraries: libimebra.so.5```
then qualify **full path** lo imebra shared library in ```LD_LIBRARY_PATH``` variable
```
export LD_LIBRARY_PATH=/home/your_username/projects/medical_image_processing/imebra_git/artifacts/
```