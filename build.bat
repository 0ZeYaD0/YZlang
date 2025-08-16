@echo off
SetLocal EnableDelayedExpansion

set outputAssembly=yzlang

echo ------------------------------
echo C++ build script for %outputAssembly%
echo      by Yassin Ibrahim (DeltaY)
echo ------------------------------
echo.

if not exist "./bin/" (
    mkdir bin
)

if not exist "./obj/" (
    mkdir obj
)

set defines=-DYDEBUG

set srcDir=src
set srcPath=%cd%/src
set objDir=obj

rem calculate the length of the directory(in chars) until srcDir

set length=0

:loop
if not "!srcPath:~%length%,1!"=="" (
    set /a length+=1
    goto loop
) else (
    goto end
)

:end

set lenSrc=0

:loop2
if not "!srcDir:~%lenSrc%, 1!"=="" (
    set /a lenSrc+=1
    goto loop2
) else (
    goto end2
)

:end2

set /a length-=%lenSrc%

rem ___________________________________________________________________________________

set cppFiles=

pushd %srcDir%

for /r %%f in (*.cpp) do (
    set filepath=%%~f

    set "cppFiles=!cppFiles! !filePath:~%length%!"
)

if defined cppFiles set cppFiles=!cppFiles:~1!

echo C++ files to compile =^>

for %%i in (%cppFiles%) do (
    rem trick to sleep for milliseconds
    ping 127.0.0.1 -n 1 -w 100 > nul
    echo %%i
)
echo ----------------------------
echo.

popd

echo compiling source code...

set compilerFlags=
rem -g -Wvarargs -Wall -Werror
set includeFlags=
rem -I./thirdparty/include/

clang++ %cppFiles%  %compilerFlags% %includeFlags% %defines% -o bin/%outputAssembly%

echo linking...

rem Check if the compilation was successful
if %ERRORLEVEL% equ 0 (
    echo compilation/linking successful.
) else (
    echo compilation/linking failed.
    exit
)

exit

for %%f in (%cppFiles%) do (
    set dir=%%~dpf
    set dir=!dir:~%length%!
    set dir=!dir:~%lenSrc%!
    set dir=!dir:~1!
    set dir=%objDir%\!dir!

    if not exist !dir! mkdir !dir!

    set file=%%f
    set objFile=%%~dpf%%~nf.o
    set objFile=!objFile:~%length%!
    set objFile=!objFile:~%lenSrc%!
    set objFile=%objDir%/!objFile:~1!

    clang++ %%f -o %compilerFlags% %includeFlags% %defines%
)

echo linking...

rem Check if the compilation was successful
if %ERRORLEVEL% equ 0 (
    echo compilation/linking successful.
) else (
    echo compilation/linking failed.
    exit
)

echo ----------------------------
echo.

echo linking...
echo.

set objFiles=

pushd %objDir%

for /r %%f in (*.o) do (
    set filepath=%%~f
    set "objFiles=!objFiles! !filepath:~%length%!"
)

if defined objFiles set objFiles=!objFiles:~1!

echo Translation units to link =^>

for %%i in (%objFiles%) do (
    rem trick to sleep for milliseconds
    ping 127.0.0.1 -n 1 -w 100 > nul
    echo %%i
)
echo ----------------------------

popd

rem SET THESE FLAGS FOR EXTERNAL DEPENDECIES
set staticLibs=
set dynamicLibs=

set linkerFlags=-
set linkingLibs=

clang++ -x c++ %objFiles% -o ./bin/%outputAssembly%.exe %linkerFlags% %staticLibs% %dynamicLibs% %linkingLibs%

rem Check if the compilation was successful
if %ERRORLEVEL% equ 0 (
    echo compilation successful: %OUTPUT%
) else (
    echo compilation/linking failed.
)

exit