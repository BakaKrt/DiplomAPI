@echo off
setlocal

REM Проверяем, переданы ли оба аргумента
if "%1"=="" (
    echo Error: Source file path not provided as first argument.
    exit /b 1
)
if "%2"=="" (
    echo Error: Target directory path not provided as second argument.
    exit /b 1
)

REM Извлекаем имя файла из исходного пути (например, C++DLL.dll)
for %%F in ("%1") do set "FILENAME=%%~nxF"

REM Формируем полный путь к файлу назначения: TARGET_DIR\FILENAME
set "TARGET_FILE_PATH=%2\%FILENAME%"

REM Создаём целевую директорию, если она не существует
echo Ensuring target directory exists: "%2"
if not exist "%2" mkdir "%2"

if not exist "%2" (
    echo Error: Failed to create target directory "%2".
    exit /b 1
)

echo Source file path: "%1"
echo Target directory: "%2"
echo Target file path: "%TARGET_FILE_PATH%"

REM Проверяем существование исходного файла
if not exist "%1" (
    echo Error: Source file does not exist: "%1"
    exit /b 1
)

echo Copying output file "%1" to "%TARGET_FILE_PATH%"
copy /Y "%1" "%TARGET_FILE_PATH%"

if %errorlevel% equ 0 (
    echo File copied successfully.
    exit /b 0
) else (
    echo Error occurred while copying the file.
    echo Copy command exit code: %errorlevel%
    exit /b 1
)

endlocal