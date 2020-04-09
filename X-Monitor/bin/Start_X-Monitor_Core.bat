@echo off
start /d "%~dp0" X-Monitor.exe
timeout 1 /nobreak >null
start /min /d "%~dp0..\..\X-Monitor_RecPlay\bin" X-Monitor_Rec.exe 127.0.0.1 53799
timeout 1 /nobreak >null
start /min /d "%~dp0..\..\X-Monitor_RecPlay\bin" X-Monitor_Play.exe 127.0.0.1 53799
exit
