@echo off
rd /q /s gen
md gen\erlang
pac -o gen\Scenario.abs %1
absc -erlang -d gen\erlang\ src\*.abs gen\Scenario.abs
