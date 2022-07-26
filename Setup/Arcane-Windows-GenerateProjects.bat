@echo off
pushd %~dp0\..\
call Dependencies\premake\bin\premake5.exe vs2019
popd
PAUSE