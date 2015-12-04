@echo off
rem del /F /Q 1.exe
cd %~dp0\
.\g++.exe ..\..\..\bin\%1 -S -O2 -w -o %2 -m32 -std=c++11
cd ..\..\..\bin\