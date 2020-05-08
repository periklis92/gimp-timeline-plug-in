@echo off

SET def_path=C:\Program Files\GIMP 2\lib\gimp\2.0\plug-ins
SET plugin_name=timeline
SET filename=%~dp0bin\timeline-win.exe

echo Welcome!

:CHOOSE

echo Enter 1 to install to default location
echo Enter 2 to change the default plugin location
SET /p ch=

IF %ch% == 1 ( 
    GOTO :INSTALL
) ELSE IF %ch% == 2 (
    GOTO :CHANGE_PATH
) ELSE ( 
    GOTO :END 
)

:INSTALL

IF NOT EXIST %def_path% (
    echo Couldn't find the plug-in directory
    SET /P opt=Would you like to enter the path of your GIMP plug-ins folder? [y/n]: 
    IF "%opt%" == "Y" set TRUE=1
    IF "%opt%" == "y" set TRUE=1
    
    IF defined TRUE ( 
        GOTO :CHANGE_PATH
    ) ELSE (
        GOTO :END
    )
)
IF NOT EXIST %filename% (
    echo Couldn't find file 'timeline-win.exe'
    GOTO :CHOOSE
)

SET "full_path=%def_path%\%plugin_name%"
echo %full_path%
IF not EXIST %full_path% mkdir %full_path%
xcopy /I /Q %filename% %full_path%
GOTO :END


:CHANGE_PATH

SET /p def_path=Please enter the full path: 
GOTO :INSTALL

:END
PAUSE