@REM #change current directory to this file

@rem change driver letter
@%~d0

@rem change current directory
@cd %~dp0

call deps\premake\premake5.exe vs2022

PAUSE