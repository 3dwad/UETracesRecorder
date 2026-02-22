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

### Visual Studio 2022 (.sln workflow, recommended)

If Open Folder does not show build configurations, use the explicit solution workflow:

1. Run `GenerateSolution.bat` (or execute the commands below in terminal).
2. Open `build/vs2022-x64/UETracesRecorder.sln` in Visual Studio.
3. Choose `Debug` or `Release` in the standard Visual Studio configuration dropdown and build.

Terminal equivalent:

```powershell
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-release
```

Resulting executable:
- `build/vs2022-x64/Release/UETracesRecorder.exe`

### Visual Studio 2022 (Open Folder alternative)

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
