@echo off

REM Add msys64 ucrt64 to PATH
set PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%

setlocal enabledelayedexpansion
set PACKAGE=0

REM Parse parameters for -upakuj flag
for %%A in (%*) do (
    if "%%A"=="-upakuj" set PACKAGE=1
)

:start_build
echo Compiling Resource File...
C:\msys64\ucrt64\bin\windres.exe app.rc -O coff -o app.res
if not %errorlevel% == 0 (
    echo Resource compilation failed!
    pause
    exit /b 1
)

echo.
echo Compiling CSV++ (64-bit)...
echo.

REM Compile using MSYS2 environment with direct include/library paths
echo Attempting build with wxWidgets...

C:\msys64\ucrt64\bin\g++.exe -o CSVPlusPlus.exe src/main.cpp src/MainFrame.cpp src/CSVParser.cpp src/CSVOptionsDialog.cpp src/Translations.cpp app.res ^
    -Iinclude ^
    -IC:/msys64/ucrt64/include/wx-3.2 ^
    -IC:/msys64/ucrt64/lib/wx/include/msw-unicode-3.2 ^
    -D__WXMSW__ ^
    -DUNICODE ^
    -D_UNICODE ^
    -DwxUSE_GUI=1 ^
    -fexec-charset=UTF-8 ^
    -finput-charset=UTF-8 ^
    -std=c++17 ^
    -mwindows ^
    -static-libgcc ^
    -static-libstdc++ ^
    -LC:/msys64/ucrt64/lib ^
    -lwx_mswu_html-3.2 ^
    -lwx_mswu_adv-3.2 ^
    -lwx_mswu_core-3.2 ^
    -lwx_baseu-3.2 ^
    -lwxpng-3.2 ^
    -lwxzlib-3.2 ^
    -lwxtiff-3.2 ^
    -lwxjpeg-3.2 ^
    -lwxregexu-3.2 ^
    -lwxexpat-3.2 ^
    -lkernel32 ^
    -luser32 ^
    -lgdi32 ^
    -lcomdlg32 ^
    -lwinspool ^
    -lwinmm ^
    -lshell32 ^
    -lshlwapi ^
    -lcomctl32 ^
    -lole32 ^
    -loleaut32 ^
    -luuid ^
    -lrpcrt4 ^
    -ladvapi32 ^
    -lversion ^
    -luxtheme ^
    -loleacc

if %errorlevel% == 0 (
    echo Dynamic build successful!
    set NEED_DLLS=1
) else (
    echo Build failed!
    pause
    exit /b 1
)

:package_check
if !PACKAGE! equ 1 (
    echo.
    echo Creating release package...
    if exist build rmdir /s /q build
    mkdir build
    
    echo Copying executable...
    copy CSVPlusPlus.exe build\
    
    echo Copying resources folder...
    xcopy /E /I /Y resources build\resources\
    
    if "!NEED_DLLS!"=="1" (
        echo Copying required DLLs...
        copy C:\msys64\ucrt64\bin\libwinpthread-1.dll build\
        copy "C:\msys64\ucrt64\bin\libstdc++-6.dll" build\
        copy C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll build\
        copy C:\msys64\ucrt64\bin\liblzma-5.dll build\
        copy C:\msys64\ucrt64\bin\libpcre2-16-0.dll build\
        copy C:\msys64\ucrt64\bin\zlib1.dll build\
        copy C:\msys64\ucrt64\bin\libjpeg-8.dll build\
        copy C:\msys64\ucrt64\bin\libpng16-16.dll build\
        copy C:\msys64\ucrt64\bin\libtiff-6.dll build\
        copy C:\msys64\ucrt64\bin\libjbig-0.dll build\
        copy C:\msys64\ucrt64\bin\libLerc.dll build\
        copy C:\msys64\ucrt64\bin\libwebp-7.dll build\
        copy C:\msys64\ucrt64\bin\libdeflate.dll build\
        copy C:\msys64\ucrt64\bin\libzstd.dll build\
        copy C:\msys64\ucrt64\bin\libsharpyuv-0.dll build\
        
        echo Copying wxWidgets DLLs...
        copy C:\msys64\ucrt64\bin\wxbase32u_gcc_custom.dll build\
        copy C:\msys64\ucrt64\bin\wxmsw32u_core_gcc_custom.dll build\
        copy C:\msys64\ucrt64\bin\wxmsw32u_html_gcc_custom.dll build\
    )
    
    echo Package created in build\
)

echo.
echo Build completed successfully!
pause
