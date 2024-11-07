@echo off
rd /q /s gen
md gen\erlang
absc -erlang -d gen\erlang\ ..\src\*.abs %1
