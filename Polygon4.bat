@echo off

setlocal ENABLEEXTENSIONS
set KEY_NAME=HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\4.17
set VALUE_NAME=InstalledDirectory

FOR /F "skip=2 tokens=1,2*" %%A IN ('%windir%\Sysnative\REG QUERY "%KEY_NAME%" /v "%VALUE_NAME%" 2^>nul') DO (
    set ValueName=%%A
    set ValueType=%%B
    set ValueValue=%%C
)

if not defined ValueName (
	FOR /F "skip=2 tokens=1,2*" %%A IN ('%windir%\System32\REG QUERY "%KEY_NAME%" /v "%VALUE_NAME%" 2^>nul') DO (
		set ValueName=%%A
		set ValueType=%%B
		set ValueValue=%%C
	)

	if not defined ValueName (
		FOR /F "skip=2 tokens=1,2*" %%A IN ('%windir%\SysWOW64\REG QUERY "%KEY_NAME%" /v "%VALUE_NAME%" 2^>nul') DO (
			set ValueName=%%A
			set ValueType=%%B
			set ValueValue=%%C
		)

		if not defined ValueName (
			@echo "%KEY_NAME%"\"%VALUE_NAME%" not found.
		)
	)
)

set exe=%ValueValue%\Engine\Binaries\Win64\UE4Editor.exe
start "" "%exe%" %cd%\Polygon4.uproject -ansimalloc %*
