@echo off
mkdir bin\release\x32\plugins 2>nul
mkdir bin\release\x64\plugins 2>nul

for %%f in (*.exe *.dp32) do xcopy /s /q bin\x32\Release\%%f bin\release\release\x32\plugins\
for %%f in (*.exe *.dp64) do xcopy /s /q bin\x64\Release\%%f bin\release\release\x64\plugins\
