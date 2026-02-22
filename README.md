# UE Traces Recorder

UE Traces Recorder is a simple portable Windows desktop app for launching a packaged Unreal Engine 5 build with trace recording arguments.

## Features

- Select packaged build `.exe` path via simple UI.
- Save selected `.exe` path between launches.
- Fixed list of trace channels via checkboxes. Default selected channels are:
  - `cpu`
  - `gpu`
  - `frame`
  - `log`
  - `bookmark`
  - `screenshot`
- Additional available channels in UI:
  - `task`
  - `taskgraph`
  - `counter`
  - `stats`
  - `memory`
  - `module`
  - `metadata`
  - `loadtime`
  - `assetloadtime`
  - `cook`
  - `file`
  - `net`
  - `object`
  - `animation`
  - `audio`
  - `niagara`
  - `rendercommands`
  - `rhicommands`
  - `rdg`
- Always passes `-statnamedevents`.
- Writes trace file with timestamp near `UETracesRecorder.exe` as:
  - `UETrace_YYYY-MM-DD_HH-MM-SS.utrace`
- Single-instance app behavior (prevents launching a second recorder window).

## Command format

The app launches your packaged build with this format:

```txt
"<YourGame.exe>" -trace=<selected channels> -tracefile="<path-to-UETracesRecorder-folder>/UETrace_... .utrace" -statnamedevents
```

## Build (Windows)

### Visual Studio 2022 (without CMake, recommended)

You can build directly from committed Visual Studio project files (no CMake required):

1. Open `UETracesRecorder.sln` in Visual Studio 2022.
2. Select configuration `Debug` or `Release` and platform `x64`.
3. Build (`Build -> Build Solution`).

Resulting executable:
- `build/Release/UETracesRecorder.exe`

### Visual Studio 2022 (CMake alternative)

If you prefer CMake workflow (`Open Folder` or generated solution), install:

- Visual Studio 2022
- Visual Studio workload **Desktop development with C++**
- Component **MSVC v143 - VS 2022 C++ x64/x86 build tools**
- Component **Windows 10/11 SDK**
- CMake 3.20+ available in `PATH`

Then use either:

```powershell
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-release
```

or:

```powershell
cmake --preset msvc-x64
cmake --build --preset msvc-x64-release
```

### Generic CMake

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Then use `build/Release/UETracesRecorder.exe` as a portable executable.
