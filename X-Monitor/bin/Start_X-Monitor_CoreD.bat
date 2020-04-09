@echo off
start /d "%~dp0" X-MonitorD.exe
timeout 1 /nobreak >null
start /min /d "%~dp0..\..\X-Monitor_RecPlay\bin" X-Monitor_RecD.exe 127.0.0.1 53799
timeout 1 /nobreak >null
start /min /d "%~dp0..\..\X-Monitor_RecPlay\bin" X-Monitor_PlayD.exe 127.0.0.1 53799
exit
