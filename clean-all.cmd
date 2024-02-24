@echo off
set back=%cd%
for /d %%i in (*) do (
	cd "%%i"
	call w4-clean.cmd
        cd..
)
cd %back%