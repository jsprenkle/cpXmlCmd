@echo off
rem
rem unit test for cpXmlCmd
rem Author: Jay Sprenkle
rem

rem
rem where is the binary?
rem
set cpXmlCmd=dist\release\MinGW-Windows\cpXmlCmd.exe

rem
rem test directory creation on windows network file system
rem
echo Begin windows network file system directory creation test
set src="one.file"
set dest="\\svd0bizt01\shared folder\x\y\z\two.file"

echo "test" >%src%
del outputdocument.xml

%cpXmlCmd% -l outputdocument.xml %src% %dest%
if %ERRORLEVEL% GTR 0 (
    echo cpXmlCmd command failed
    goto :eof
)
IF NOT EXIST outputdocument.xml (
    echo "outputdocument.xml was not created"
    goto :eof
)

IF NOT EXIST %src% (
    echo "%src% does not exist but should"
    goto :eof
)
IF NOT EXIST %dest% (
    echo "x/y/z/two.file does not exist but should"
    goto :eof
)

echo   Succeeded.

del %src%
del %dest%

echo Finished.
