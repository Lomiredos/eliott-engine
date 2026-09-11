@echo off
setlocal enabledelayedexpansion

echo =================================
echo ^| push de tous les sous-modules ^|
echo =================================
echo.

set /p SAME_MSG="Meme message pour tous les commits ? (o/n) : "
echo.

if /i not "%SAME_MSG%"=="o" goto boucle_msg_custom

set /p GLOBAL_MSG="Message de commit global : "
echo.

for %%i in (eliott-audio eliott-ecs eliott-input eliott-math eliott-physics eliott-renderer eliott-tmx-parser) do (
    echo [%%i]
    cd extern\%%i

    set HAS_CHANGES=
    for /f %%j in ('git status --porcelain') do set HAS_CHANGES=1

    if not defined HAS_CHANGES (
        echo Rien a commiter.
    ) else (
        git add .
        git commit -m "%GLOBAL_MSG%"
        git push
    )

    cd ..\..
    echo.
)
goto fin

:boucle_msg_custom
for %%i in (eliott-audio eliott-ecs eliott-input eliott-math eliott-physics eliott-renderer eliott-tmx-parser) do (
    echo [%%i]
    cd extern\%%i

    set HAS_CHANGES=
    for /f %%j in ('git status --porcelain') do set HAS_CHANGES=1

    if not defined HAS_CHANGES (
        echo Rien a commiter.
    ) else (
        set /p CUSTOM_MSG="Message de commit pour %%i : "
        git add .
        git commit -m "!CUSTOM_MSG!"
        git push
    )

    cd ..\..
    echo.
)

:fin
echo [eliott-engine]
set HAS_CHANGES=
for /f %%j in ('git status --porcelain') do set HAS_CHANGES=1

if not defined HAS_CHANGES (
    echo Rien a commiter.
) else (
    if /i "%SAME_MSG%"=="o" (
        git add .
        git commit -m "%GLOBAL_MSG%"
        git push
    ) else (
        set /p ENGINE_MSG="Message de commit pour eliott-engine : "
        git add .
        git commit -m "!ENGINE_MSG!"
        git push
    )
)
echo.

echo =================================
echo ^| Termine !                     ^|
echo =================================
pause
endlocal
