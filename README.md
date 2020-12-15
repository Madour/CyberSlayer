## NasNas game template

This repository is a template you can use to setup a project that uses [NasNas](https://github.com/Madour/NasNas) game framework.

*Requirements :*

- SFML2.5 installed.
- CMake >=3.10 installed (check with `cmake --version`).

Follow these 3 steps :

1. Clone this repository and update NasNas :
```bash
git clone --recurse https://github.com/Madour/NasNas-game-template MyNewGame
cd MyNewGame
git submodule update --remote
```
2. Open `CMakeLists.txt` file and set your custom configurations in the "User Configuration" section.

3. Now, you are ready to build :

```bash
mkdir build && cd build
cmake ..
cmake --build . -j4
```

(You can refer to the **[Build](https://github.com/Madour/NasNas#build)** section for the list of possible cmake options you can use)


If everything went fine, go ahead and run the binary file in `build/bin`. You'll see a black window with "Hello World !" displayed.

Now you are ready to use NasNas to create awesome games :tada:!

# 

A complete **[tutorial](https://github.com/Madour/NasNas/wiki)** is available to get you started ! 

#

If you got errors during compilation, please open a [new GitHub Issue](https://github.com/Madour/NasNas/issues), I'll try to help you :)
