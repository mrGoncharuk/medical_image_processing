# medical_image_processing
Labs from Medical Image Processing course


## Lab 1
### Tasks
1. Create events from the keyboard, during the processing of which you can separately perform the specified pixel operations on the active image and restore the original image of the loaded tomographic slice.
2. Perform a logical transformation of the medical image, applying the specified logical operation to the data of the original image and background texture, which is created as a bitmap in the program in accordance with the task scheme in the option.
3. Perform color modeling of the medical image to transition from grayscale representation of pixel data to the RGBA color model (use the gradient change rule and color channel according to the specified options).
![image](https://user-images.githubusercontent.com/28892426/109531754-f6ed7a00-7ac0-11eb-8d9e-8ea51ee3ade5.png)

### Demonstarion of result

![Peek 2021-02-28 20-29](https://user-images.githubusercontent.com/28892426/109531083-1fc13f80-7ac0-11eb-9470-ff6bfcb2e86f.gif)


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
