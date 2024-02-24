@echo off
set back=%cd%
for /d %%i in (*) do (
	cd "%%i"
	call w4-build.cmd
        cd .. 
)
cd %back%