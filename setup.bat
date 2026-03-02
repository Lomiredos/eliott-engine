@echo off
setlocal enabledelayedexpansion

echo ============================================
echo    Setup eliott-engine ^| Gaming Campus
echo ============================================
echo.

:: -----------------------------------------------
:: 1. Determiner VCPKG_ROOT
::    Si la variable d'env n'est pas definie,
::    on utilise C:\Dev\vcpkg par defaut
:: -----------------------------------------------
if not defined VCPKG_ROOT (
    echo [!] VCPKG_ROOT non defini. Utilisation de C:\Dev\vcpkg par defaut.
    set "VCPKG_ROOT=C:\Dev\vcpkg"
)
echo [i] VCPKG_ROOT = %VCPKG_ROOT%

:: Rendre VCPKG_ROOT permanent (USER) pour VSCode et les prochains terminaux
setx VCPKG_ROOT "%VCPKG_ROOT%" >nul
echo [i] VCPKG_ROOT enregistre en variable d'environnement permanente.
echo.

:: -----------------------------------------------
:: 2. Cloner vcpkg si le dossier n'existe pas
:: -----------------------------------------------
if not exist "%VCPKG_ROOT%\" (
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
:: 3. Bootstrapper vcpkg si vcpkg.exe n'existe pas
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
:: 4. Installer les dependances (MinGW statique)
:: -----------------------------------------------
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
echo.
echo [+] Dependances installees.
echo.

:: -----------------------------------------------
:: 5. Configurer CMake avec le preset mingw
:: -----------------------------------------------
echo [*] Configuration CMake (preset mingw)...
cmake --preset mingw

if errorlevel 1 (
    echo [ERREUR] Echec de la configuration CMake.
    echo Verifiez que CMake 3.20+ et MinGW sont installes et dans le PATH.
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
endlocal
