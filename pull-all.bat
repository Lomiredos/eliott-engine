@echo off
setlocal enabledelayedexpansion

echo =================================
echo ^| pull de tous les sous-modules ^|
echo =================================
echo.

for %%i in (eliott-audio eliott-ecs eliott-input eliott-math eliott-physics eliott-renderer eliott-tmx-parser) do (
    echo [%%i]
    cd extern\%%i
    git checkout main 2>nul || git checkout master 2>nul
    git pull
    cd ..\..
    echo.
)

echo [eliott-engine]
git checkout main 2>nul || git checkout master 2>nul
git pull
echo.

echo =================================
echo ^| Termine !                     ^|
echo =================================
pause
endlocal
