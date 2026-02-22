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

### Visual Studio 2022 (recommended)

1. Open the repository folder in Visual Studio (`File -> Open -> Folder...`).
2. Visual Studio will detect `CMakePresets.json` and create the `msvc-x64` configure preset automatically.
3. In the configuration dropdown choose either **x64-Debug** or **x64-Release** and build.

Or from terminal:

```powershell
cmake --preset msvc-x64
cmake --build --preset msvc-x64-release
```

Resulting executable:
- `build/msvc-x64/Release/UETracesRecorder.exe`

### Generic CMake

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Then use `build/Release/UETracesRecorder.exe` as a portable executable.
