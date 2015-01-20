setlocal

@echo off
if "%1"=="rel" (
   set CONF=rel
) else (
   set CONF=dbg
)
@echo on
arm-eabi-insight -x jlink.gdb %CONF%\blinky.elf

@echo off
endlocal
