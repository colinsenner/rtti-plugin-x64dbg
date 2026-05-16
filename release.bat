@echo off
mkdir bin\release\x32\plugins 2>nul
mkdir bin\release\x64\plugins 2>nul

xcopy /s /q bin\x32\Release\* bin\release\release\x32\plugins\
xcopy /s /q bin\x64\Release\* bin\release\release\x64\plugins\
