# cs163-project

Data Structure Visualizer built with C++17 and SFML 3.

## Build

Configure and build from the repository root:

```powershell
cmake -S Visualizer -B build
cmake --build build --config Debug
```

The build copies `Visualizer/assets` next to the executable.

## Run

After building, run:

```powershell
.\build\Visualizer.exe
```

The music playlist is loaded from `Visualizer/assets/playlist`.
