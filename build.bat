@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /O2 /W4 /GS- /Zl /LD /Fobuild\handoff.obj src\handoff.c /link /NODEFAULTLIB /ENTRY:DllMain /DEF:src\dbghelp.def /OUT:build\dbghelp.dll /IMPLIB:build\dbghelp.lib kernel32.lib || exit /b 1
