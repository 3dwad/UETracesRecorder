@echo off
setlocal

where cmake >nul 2>nul
if errorlevel 1 (
  echo [ERROR] CMake is not found in PATH.
  echo Install CMake or run this from a "Developer Command Prompt for VS 2022".
  exit /b 1
)

echo Generating Visual Studio solution via preset: vs2022-x64
cmake --preset vs2022-x64
if errorlevel 1 exit /b 1

echo.
echo Done. Open this file in Visual Studio:
echo   build\vs2022-x64\UETracesRecorder.sln
endlocal
