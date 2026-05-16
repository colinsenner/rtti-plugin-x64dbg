@echo off

xcopy /s /q bin\x32\Release\* bin\release\x32\plugins\
xcopy /s /q bin\x64\Release\* bin\release\x64\plugins\
