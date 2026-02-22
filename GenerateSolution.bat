@echo off
setlocal

set "BUILD_DIR=build\vs2022-x64"
set "SLN_PATH=%BUILD_DIR%\UETracesRecorder.sln"

where cmake >nul 2>nul
if errorlevel 1 (
  echo [ERROR] CMake is not found in PATH.
  echo Install CMake and run this script again.
  echo.
  echo Also make sure Visual Studio 2022 is installed with:
  echo   - Desktop development with C++
  echo   - MSVC v143 build tools
  echo   - Windows 10/11 SDK
  exit /b 1
)

echo Generating Visual Studio solution...
cmake --preset vs2022-x64
if errorlevel 1 (
  echo.
  echo [ERROR] CMake configure failed.
  echo Most common reason: Visual Studio C++ toolchain components are missing.
  echo Install Visual Studio 2022 workload "Desktop development with C++" and retry.
  exit /b 1
)

if not exist "%SLN_PATH%" (
  echo.
  echo [ERROR] Configure command completed, but solution file was not found:
  echo   %SLN_PATH%
  echo.
  echo Ensure Visual Studio 2022 (with C++ workload) is installed.
  exit /b 1
)

echo.
echo Done. Open this file in Visual Studio:
echo   %SLN_PATH%
endlocal
