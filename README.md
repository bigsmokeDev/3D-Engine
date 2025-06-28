# Simple 3D Engine made in C and OpenGL 3.3
simple template for making 3D games in C.

# Building
The project uses Premake5, you can generate project files easily (premake is already included
inside the `tools` directory). Using the generated project files you can build and run the project.

## Windows (Visual Studio 2022):
```
.\tools\premake5.exe vs2022
MSBuild 3D-Engine.sln
```

## Windows (MinGW):
```
.\tools\premake5.exe gmake --os=windows
mingw32-make
```

## Linux:
```
./tools/premake5 gmake
make
```