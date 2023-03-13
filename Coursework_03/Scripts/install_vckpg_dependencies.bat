@echo off 

call ValidateVCPKG.bat

if not "%errorlevel%"=="0" (
  exit /b %errorlevel%
)

%VCPKG_ROOT%\vcpkg update
%VCPKG_ROOT%\vcpkg install boost eigen3 suitesparse opencv4 glew
