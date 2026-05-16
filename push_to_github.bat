@echo off
setlocal

cd /d E:\xiangmuji\watch
if errorlevel 1 (
    echo Failed to enter project directory.
    exit /b 1
)

echo.
set /p COMMIT_MSG=Please input commit message: 
if "%COMMIT_MSG%"=="" (
    echo Commit message cannot be empty.
    exit /b 1
)

echo.
echo Push target:
echo   1. origin main ^(default^)
echo   2. custom remote and branch
set /p PUSH_CHOICE=Choose 1 or 2: 

if "%PUSH_CHOICE%"=="2" (
    set /p PUSH_REMOTE=Remote name or URL: 
    set /p PUSH_BRANCH=Branch name: 
) else (
    set PUSH_REMOTE=origin
    set PUSH_BRANCH=main
)

if "%PUSH_REMOTE%"=="" (
    echo Push remote cannot be empty.
    exit /b 1
)

if "%PUSH_BRANCH%"=="" (
    echo Push branch cannot be empty.
    exit /b 1
)

git add .
if errorlevel 1 (
    echo git add failed.
    exit /b 1
)

git commit -m "%COMMIT_MSG%"
if errorlevel 1 (
    echo git commit failed. Maybe there is nothing to commit.
    exit /b 1
)

git push "%PUSH_REMOTE%" "%PUSH_BRANCH%"
if errorlevel 1 (
    echo git push failed.
    exit /b 1
)

echo Push finished.
endlocal
