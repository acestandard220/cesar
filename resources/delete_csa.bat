@echo off
setlocal

set "EXT=.csa"

echo Deleting *%EXT% files under "%CD%"...

for /r %%F in (*%EXT%) do (
    echo Deleting: "%%F"
    del /f /q "%%F"
)

echo Done.
pause