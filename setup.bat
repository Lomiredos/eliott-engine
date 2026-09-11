@echo off
setlocal enabledelayedexpansion

echo ============================================
echo    Setup eliott-engine ^| Gaming Campus
echo ============================================
echo.

:: -----------------------------------------------
:: 0. Verifier / Installer MinGW (WinLibs UCRT)
:: -----------------------------------------------
set "MINGW_ROOT=C:\Dev\mingw64"
set "MINGW_BIN=%MINGW_ROOT%\bin"
set "WINLIBS_ZIP=C:\Dev\mingw64.zip"
set "WINLIBS_URL=https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-14.0.0-ucrt-r7/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-14.0.0-r7.zip"

if exist "%MINGW_BIN%\g++.exe" goto mingw_ok

echo [*] MinGW non detecte. Recuperation de la derniere version WinLibs...
if not exist "C:\Dev" mkdir "C:\Dev"

echo [i] URL : !WINLIBS_URL!
echo [*] Telechargement en cours (environ 255 Mo)...
curl -L -o "%WINLIBS_ZIP%" "!WINLIBS_URL!"
if errorlevel 1 (
    echo [ERREUR] Echec du telechargement de WinLibs.
    pause
    exit /b 1
)
echo [+] Telechargement OK.

echo [*] Extraction de MinGW dans C:\Dev\ ...
tar -xf "%WINLIBS_ZIP%" -C "C:\Dev"
if errorlevel 1 (
    echo [ERREUR] Echec de l'extraction.
    pause
    exit /b 1
)
del "%WINLIBS_ZIP%"
echo [+] MinGW installe dans %MINGW_ROOT%.
echo.
goto mingw_suite

:mingw_ok
echo [+] MinGW deja present dans %MINGW_ROOT%.
echo.

:mingw_suite
:: Ajouter MinGW au PATH pour cette session
set "PATH=%MINGW_BIN%;%PATH%"

:: Rendre le PATH permanent (USER, sans admin)
for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "USER_PATH=%%B"
echo %USER_PATH% | find /i "%MINGW_BIN%" >nul
if errorlevel 1 (
    setx PATH "%MINGW_BIN%;%USER_PATH%" >nul
    echo [i] MinGW ajoute au PATH permanent.
)
echo.

:: -----------------------------------------------
:: 1. Determiner VCPKG_ROOT
:: -----------------------------------------------
if not defined VCPKG_ROOT (
    echo [!] VCPKG_ROOT non defini. Utilisation de C:\Dev\vcpkg par defaut.
    set "VCPKG_ROOT=C:\Dev\vcpkg"
)
echo [i] VCPKG_ROOT = %VCPKG_ROOT%
setx VCPKG_ROOT "%VCPKG_ROOT%" >nul
echo [i] VCPKG_ROOT enregistre en variable d'environnement permanente.
echo.

:: -----------------------------------------------
:: 2. Cloner vcpkg
:: -----------------------------------------------
if not exist "%VCPKG_ROOT%" (
    echo [*] Clonage de vcpkg dans %VCPKG_ROOT% ...
    git clone https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"
    if errorlevel 1 (
        echo [ERREUR] Echec du clonage. Verifiez votre connexion internet et que git est installe.
        pause
        exit /b 1
    )
    echo [+] Clonage OK.
    echo.
)

:: -----------------------------------------------
:: 3. Bootstrapper vcpkg
:: -----------------------------------------------
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [*] Bootstrap de vcpkg...
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
    if errorlevel 1 (
        echo [ERREUR] Echec du bootstrap vcpkg.
        pause
        exit /b 1
    )
    echo [+] Bootstrap OK.
    echo.
) else (
    echo [+] vcpkg.exe deja present.
    echo.
)

:: -----------------------------------------------
:: 4. Installer les dependances
:: -----------------------------------------------
echo Quel compilateur voulez-vous utiliser ?
echo   1) MinGW (VSCode / ligne de commande)
echo   2) MSVC  (Visual Studio 2022)
echo.
set /p CHOIX="Votre choix (1 ou 2) : "
echo.

if "%CHOIX%"=="1" goto install_mingw
if "%CHOIX%"=="2" goto install_msvc
echo [ERREUR] Choix invalide. Relancez le script.
pause
exit /b 1

:install_mingw
echo [*] Installation des dependances (x64-mingw-static)...
echo     - sdl3
echo     - sdl3-image
echo     - tinyxml2
echo     - doctest
echo     - miniaudio
echo.

"%VCPKG_ROOT%\vcpkg.exe" install ^
    sdl3:x64-mingw-static ^
    sdl3-image:x64-mingw-static ^
    tinyxml2:x64-mingw-static ^
    doctest:x64-mingw-static ^
    miniaudio:x64-mingw-static

if errorlevel 1 (
    echo [ERREUR] Echec de l'installation des dependances.
    pause
    exit /b 1
)
echo [+] Dependances installees.
echo.

echo [*] Configuration CMake (preset mingw)...
cmake --preset mingw
if errorlevel 1 (
    echo [ERREUR] Echec de la configuration CMake.
    echo Verifiez que CMake 3.20+ est installe et dans le PATH.
    pause
    exit /b 1
)

echo.
echo ============================================
echo    Setup termine !
echo    Build dans : build/mingw/
echo    Pour compiler : cmake --build build/mingw
echo ============================================
echo.
pause
goto fin

:install_msvc
echo [*] Installation des dependances (x64-windows-static)...
echo     - sdl3
echo     - sdl3-image
echo     - tinyxml2
echo     - doctest
echo     - miniaudio
echo.

"%VCPKG_ROOT%\vcpkg.exe" install ^
    sdl3:x64-windows-static ^
    sdl3-image:x64-windows-static ^
    tinyxml2:x64-windows-static ^
    doctest:x64-windows-static ^
    miniaudio:x64-windows-static

if errorlevel 1 (
    echo [ERREUR] Echec de l'installation des dependances.
    pause
    exit /b 1
)
echo [+] Dependances installees.
echo.

echo [*] Configuration CMake (preset msvc)...
cmake --preset msvc
if errorlevel 1 (
    echo [ERREUR] Echec de la configuration CMake.
    echo Verifiez que Visual Studio 2022 et CMake 3.20+ sont installes.
    pause
    exit /b 1
)

echo.
echo ============================================
echo    Setup termine !
echo    Ouvrez Visual Studio 2022, puis :
echo    File ^> Open ^> CMake...
echo    Selectionnez le CMakeLists.txt racine.
echo ============================================
echo.
pause

:fin
endlocal