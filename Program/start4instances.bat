@echo off
setlocal enabledelayedexpansion

:: Path to the exe
set "program_path=solver.exe"

:: Get available numbers of threads
for /f %%a in ('echo %NUMBER_OF_PROCESSORS%') do set "num_cores=%%a"

:: SET PROGRAM INSTANCES (can be modify)
set "num_programs=4"

set /a "programs_per_core = num_programs / num_cores"

:: Run program in real time priority
for /l %%i in (1,1,%num_programs%) do (
    timeout /t 2 /nobreak
    set /a "instance_num = %%i - 1"
    set "window_title=ctvrp_%%i%"

:: Start program command. You can edit input parameters, for specified algorithm
    start "!window_title!" /realtime cmd /c "!program_path!" -p Benchmarks\B_n51_k7_1032_100.csv -v Benchmarks\Vehicle_100.csv -SA 100 0.1 0.90 100
)

endlocal