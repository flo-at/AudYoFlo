@echo off

REM Check current folder
set ABS_PATH_INSTALL="%CD%"
set RTAP_SDK_PATH="C:\Users\%USERNAME%\ownCloud\jvx-sdks\release__x64_vs2015_qt571_mat2016a_debug"
set PTSED=sed

echo "--> Running standalone project generation in folder %ABS_PATH_INSTALL%"

for %%a in (%ABS_PATH_INSTALL%) do (
    set project_token=%%~nxa
)    
echo "--> Project token: %project_token%"

REM Create build folder
if not exist %ABS_PATH_INSTALL%\..\build md %ABS_PATH_INSTALL%\..\build

REM Specify path reference to SDK folder
set /p sdkpath="--> Specify location of SDK folder [%RTAP_SDK_PATH%]:"
if "%sdkpath%"=="" GOTO cmakegetgenerator
set RTAP_SDK_PATH="%sdkpath%"

if exist %JVX_SDK_PATH%\tools\sed.exe set PTSED=%JVX_SDK_PATH%\tools\sed

echo "--> RTProc SDK folder: %RTAP_SDK_PATH%"

:cmakegetgenerator
if not exist %RTAP_SDK_PATH%\ GOTO error_no_sdk_path

REM look for sed and add to path
%PTSED% --help >nul 2>nul || if exist %RTAP_SDK_PATH%\tools\ set PATH=%PATH%;%RTAP_SDK_PATH%\tools
%PTSED% --help >nul 2>nul || echo "Error: sed.exe was not found. Copy sed to %RTAP_SDK_PATH%\tools\ or to your path." && GOTO :leave_script

REM Specify cmake generator token
set /p cmakegenerator="--> Specify cmake Generator token[1:NMake Makefiles; 2: Visual Studio 2015 Win64; 3: Visual Studio 2012 Win64]:"

if "%cmakegenerator%"=="1" GOTO nmake
if "%cmakegenerator%"=="2" GOTO vs2015
if "%cmakegenerator%"=="3" GOTO vs2012

:nmake
set CMAKE_TOKEN="NMake Makefiles"
GOTO replaceandleave

:vs2012
set CMAKE_TOKEN="Visual Studio 11 2012 Win64"
GOTO replaceandleave

:vs2015
set CMAKE_TOKEN="Visual Studio 14 2015 Win64"
GOTO replaceandleave

REM Involve the settings to become part of the compile script
:replaceandleave
set "RTAP_SDK_PATH=%RTAP_SDK_PATH:\=/%"

echo "--> RTProc SDK path is %RTAP_SDK_PATH%"
echo "--> CMake Generator token choice: %CMAKE_TOKEN%"

%PTSED% s@RTAP_RPL_GENERATOR_TOKEN@%CMAKE_TOKEN%@g standalone\build\compile.bat > ___j__v__x__tmp.bat || ( goto :error_sed_failed )
%PTSED% s@RTAP_RPL_SDK_PATH@%RTAP_SDK_PATH%@g ___j__v__x__tmp.bat > ..\build\compile.bat || ( goto :error_sed_failed )
del /Q ___j__v__x__tmp.bat

echo "--> Copy CMakeLists file to project folder"
if not exist ..\CMakeLists.txt xcopy /Y /Q /E standalone\cmake\CMakeLists.txt ..\  || ( goto :error_xcopy_failed )

cd ..\build
echo "--> Type <compile.bat> to run cmake"

GOTO leave_script

:error_no_sdk_path
echo "Error: <%RTAP_SDK_PATH%> is not a valid sdk path!"
GOTO leave_script

:error_no_cmake_token
echo "Error: <%cmakegenerator%> is not a valid cmake generator token!"
GOTO leave_script

:error_sed_failed
echo "Error: sed command failed."
GOTO leave_script

:error_xcopy_failed
echo "Error: xcopy /Y /Q /E standalone\cmake\CMakeLists.txt ..\ failed!"

:leave_script


